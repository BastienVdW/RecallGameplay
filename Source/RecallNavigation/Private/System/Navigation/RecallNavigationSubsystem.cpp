// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "System/Navigation/RecallNavigationSubsystem.h"

#include "RecallNavigationSnapshot.h"
#include "Settings/RecallGameplaySettings.h"
#include "System/Simulation/RecallMultiSimSubsystem.h"
#include "Utility/Simulation/RecallSimulationUtils.h"
#include "Utility/MultiWorldUtils.h"
#include "Utility/Navigation/RecallNavigationUtils.h"

void URecallNavigationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	Collection.InitializeDependency<URecallMultiSimSubsystem>();

	const UWorld* MainWorld = MultiWorld::Utils::GetMainWorld(this);

	if (URecallMultiSimSubsystem* MultiSimSystem = UWorld::GetSubsystem<URecallMultiSimSubsystem>(MainWorld))
	{
		MultiSimSystem->GetOnTickStartEvent().AddUObject(this, &ThisClass::OnTickStart);
	}

	UWorld* World = GetWorld();
	if (IsValid(World) && World->IsGameWorld())
	{
		World->OnWorldPreBeginPlay.AddUObject(this, &URecallNavigationSubsystem::OnWorldPreBeginPlay);
	}

	NavigationGamethread = NewObject<URecallNavigationGamethreadQueue>(this);
}

void URecallNavigationSubsystem::Deinitialize()
{
	Super::Deinitialize();

	const UWorld* MainWorld = MultiWorld::Utils::GetMainWorld(this);

	if (URecallMultiSimSubsystem* MultiSimSystem = UWorld::GetSubsystem<URecallMultiSimSubsystem>(MainWorld))
	{
		MultiSimSystem->GetOnTickStartEvent().RemoveAll(this);
	}

	UWorld* World = GetWorld();
	if (IsValid(World) && World->IsGameWorld())
	{
		World->OnWorldPreBeginPlay.RemoveAll(this);
	}

	if (NavigationGamethread)
	{
		NavigationGamethread->ReleaseAllRunners();
		NavigationGamethread = nullptr;
	}
}

void URecallNavigationSubsystem::Reset()
{
	SerialNumberGenerator = 0;
	NavigationDataMap.Reset();

	if (NavigationGamethread)
	{
		NavigationGamethread->ReleaseAllRunners();
	}
}

void URecallNavigationSubsystem::Save(const FRecallSnapshotContext& Context, FInstancedStruct& OutSnapshot)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_Navigation_Save);

	OutSnapshot.InitializeAs<FRecallNavigationSnapshot>();

	FRecallNavigationSnapshot& Data = OutSnapshot.GetMutable<FRecallNavigationSnapshot>();
	Data.SerialNumberGenerator = SerialNumberGenerator;
	Data.NavigationDataMap = NavigationDataMap;
}

void URecallNavigationSubsystem::Restore(const FRecallSnapshotContext& Context, const FInstancedStruct& InSnapshot)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_Navigation_Restore);

	if (const FRecallNavigationSnapshot* DataPtr = InSnapshot.GetPtr<FRecallNavigationSnapshot>())
	{
		SerialNumberGenerator = DataPtr->SerialNumberGenerator;
		NavigationDataMap = DataPtr->NavigationDataMap;
	}
}

void URecallNavigationSubsystem::OnWorldPreBeginPlay()
{
	Recall::Navigation::Utils::RegisterGeneratedLinksProxy(GetWorld());
}

void URecallNavigationSubsystem::OnTickStart(float DeltaTime)
{
	// Keep our env query up-to-date before stepping our simulation
	if (NavigationGamethread)
	{
		FScopeLock Lock(&DataGuard);
		NavigationGamethread->UpdateNavigationRunners(NavigationDataMap);
	}
}

void URecallNavigationSubsystem::TickPathQueue()
{
}

bool URecallNavigationSubsystem::RequestPathSync(const FVector& Start, const FVector& End, TArray<FRecallNavigationPathPoint>& OutPathPoints, float Radius /*= -1.f*/, float Height /*= -1.f*/)
{
	FRecallNavigationData NewData;
	NewData.Start = Start;
	NewData.End = End;
	NewData.Radius = Radius;
	NewData.Height = Height;

	return URecallNavigationRunnerTask::RunSync(GetWorld(), NewData, OutPathPoints);
}

void URecallNavigationSubsystem::RequestPathAsync(FRecallNavigationHandle& Handle,
	const FVector& Start, const FVector& End, float Radius /*= -1.f*/, float Height /*= -1.f*/)
{
	ReleaseAsyncPath(Handle);

	{
		const URecallGameplaySettings* GameplaySettings = GetDefault<URecallGameplaySettings>();
		const uint32 Frame = Recall::Simulation::Utils::GetFrame(this);
		
		FScopeLock Lock(&DataGuard);

		Handle = FRecallNavigationHandle(++SerialNumberGenerator);

		const int32 NavigationCount = GetConcurrentPath(Frame);
		const int32 NavigationCycle = NavigationCount / GameplaySettings->ConcurrentPathLimit;
		const int32 Duration = GetPathWaitDuration();

		const uint32 AsyncStartFrame = Frame + static_cast<uint32>(Duration * NavigationCycle);
		const uint32 AsyncEndFrame = AsyncStartFrame + static_cast<uint32>(Duration);

		FRecallNavigationData& NewData = NavigationDataMap.Add(Handle);
		NewData.AsyncStartFrame = AsyncStartFrame;
		NewData.AsyncEndFrame = AsyncEndFrame;
		NewData.Start = Start;
		NewData.End = End;
		NewData.Radius = Radius;
		NewData.Height = Height;
	}
}

bool URecallNavigationSubsystem::IsRequestFinished(const FRecallNavigationHandle& Handle) const
{
	if (!Handle.IsValid())
	{
		return false;
	}

	const uint32 Frame = Recall::Simulation::Utils::GetFrame(this);

	FScopeLock Lock(&DataGuard);
	if (const FRecallNavigationData* Data = NavigationDataMap.Find(Handle))
	{
		return Data->IsFinished(Frame);
	}
	return false;
}

void URecallNavigationSubsystem::ReleaseAsyncPath(FRecallNavigationHandle& Handle)
{
	if (!Handle.IsValid())
	{
		return;
	}

	{
		FScopeLock Lock(&DataGuard);
		NavigationDataMap.Remove(Handle);
	}

	Handle.Reset();
}

bool URecallNavigationSubsystem::GetAsyncPathResult(const FRecallNavigationHandle& Handle, TArray<FRecallNavigationPathPoint>& OutPathPoints) const
{
	FScopeLock Lock(&DataGuard);
	if (ensure(NavigationGamethread) && ensureAlwaysMsgf(NavigationDataMap.Contains(Handle), TEXT("Navigation does not exist")))
	{
		const TArray<FRecallNavigationPathPoint> PathPoints = NavigationGamethread->GetPathPoints(Handle);
		if (PathPoints.Num() > 1)
		{
			OutPathPoints.Reset(PathPoints.Num());
			OutPathPoints.Append(PathPoints);
			return true;
		}
	}
	
	return false;
}

int32 URecallNavigationSubsystem::GetPathWaitDuration() const
{
	const URecallGameplaySettings* GameplaySettings = GetDefault<URecallGameplaySettings>();
	const int32 MaxStepCount = Recall::Simulation::Utils::GetMaxStepCount(this);
	return GameplaySettings->PathWaitDuration + MaxStepCount;
}

int32 URecallNavigationSubsystem::GetConcurrentPath(uint32 Frame) const
{
	int32 Result = 0;

	for (const TPair<FRecallNavigationHandle, FRecallNavigationData>& NavigationData : NavigationDataMap)
	{
		if (!NavigationData.Value.IsFinished(Frame))
		{
			Result++;
		}
	}

	return Result;
}
