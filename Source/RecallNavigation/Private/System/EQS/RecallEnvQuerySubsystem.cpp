// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "System/EQS/RecallEnvQuerySubsystem.h"

#include "RecallEnvQuerySnapshot.h"
#include "RecallSignalSubsystem.h"
#include "Navigation/RecallNavigationSignalTypes.h"
#include "Settings/RecallGameplaySettings.h"
#include "System/Random/RecallRandomNumberSubsystem.h"
#include "System/Simulation/RecallMultiSimSubsystem.h"
#include "Utility/Simulation/RecallSimulationUtils.h"
#include "Utility/MultiWorldUtils.h"

void URecallEnvQuerySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	Collection.InitializeDependency<URecallSignalSubsystem>();
	Collection.InitializeDependency<URecallMultiSimSubsystem>();
	Collection.InitializeDependency<URecallRandomNumberSubsystem>();

	const UWorld* MainWorld = MultiWorld::Utils::GetMainWorld(this);

	if (URecallMultiSimSubsystem* MultiSimSystem = UWorld::GetSubsystem<URecallMultiSimSubsystem>(MainWorld))
	{
		MultiSimSystem->GetOnTickStartEvent().AddUObject(this, &ThisClass::OnTickStart);
	}

	SignalSystem = UWorld::GetSubsystem<URecallSignalSubsystem>(GetWorld());
	RandomNumberSystem = UWorld::GetSubsystem<URecallRandomNumberSubsystem>(GetWorld());
	EnvQueryGamethread = NewObject<URecallEnvQueryGamethreadQueue>(this);
}

void URecallEnvQuerySubsystem::Deinitialize()
{
	Super::Deinitialize();

	const UWorld* MainWorld = MultiWorld::Utils::GetMainWorld(this);

	if (URecallMultiSimSubsystem* MultiSimSystem = UWorld::GetSubsystem<URecallMultiSimSubsystem>(MainWorld))
	{
		MultiSimSystem->GetOnTickStartEvent().RemoveAll(this);
	}

	if (EnvQueryGamethread)
	{
		EnvQueryGamethread->ReleaseAllRunners();
		EnvQueryGamethread = nullptr;
	}

	SignalSystem.Reset();
}

void URecallEnvQuerySubsystem::Reset()
{
	SerialNumberGenerator = 0;
	EnvQueryDataMap.Reset();

	if (EnvQueryGamethread)
	{
		EnvQueryGamethread->ReleaseAllRunners();
	}
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	EnvQueryDebugCache.Reset();
#endif // UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
}

void URecallEnvQuerySubsystem::Save(const FRecallSnapshotContext& Context, FInstancedStruct& OutSnapshot)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR(TEXT("URecallEnvQuerySubsystem::Save"));
	QUICK_SCOPE_CYCLE_COUNTER(Recall_EnvQuery_Save);

	OutSnapshot.InitializeAs<FRecallEnvQuerySnapshot>();

	FScopeLock EnvQueryLock(&EnvQueryGuard);
	FRecallEnvQuerySnapshot& Data = OutSnapshot.GetMutable<FRecallEnvQuerySnapshot>();
	Data.SerialNumberGenerator = SerialNumberGenerator;
	Data.EnvQueryDataMap = EnvQueryDataMap;
}

void URecallEnvQuerySubsystem::Restore(const FRecallSnapshotContext& Context, const FInstancedStruct& InSnapshot)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR(TEXT("URecallEnvQuerySubsystem::Restore"));
	QUICK_SCOPE_CYCLE_COUNTER(Recall_EnvQuery_Restore);

	if (const FRecallEnvQuerySnapshot* DataPtr = InSnapshot.GetPtr<FRecallEnvQuerySnapshot>())
	{
		FScopeLock EnvQueryLock(&EnvQueryGuard);

		SerialNumberGenerator = DataPtr->SerialNumberGenerator;
		EnvQueryDataMap = DataPtr->EnvQueryDataMap;
	}

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	if (Context.IsSnapshot())
	{
		EnvQueryDebugCache.Reset();
	}
#endif // UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
}

void URecallEnvQuerySubsystem::OnTickStart(float DeltaTime)
{
	// Keep our env query up to date before stepping our simulation
	if (EnvQueryGamethread)
	{
		FScopeLock EnvQueryLock(&EnvQueryGuard);
		EnvQueryGamethread->UpdateEnvQueryRunners(EnvQueryDataMap);
	}
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	const URecallGameplaySettings* GameplaySettings = GetDefault<URecallGameplaySettings>();
	const uint32 Frame = Recall::Simulation::Utils::GetFrame(this);

	TArray<FRecallEnvQueryHandle> DebugHandles;
	EnvQueryDebugCache.GenerateKeyArray(DebugHandles);

	for (const FRecallEnvQueryHandle& Handle : DebugHandles)
	{
		const uint32 CutoffFrame = EnvQueryDebugCache[Handle].LastUpdateFrame + static_cast<uint32>(GameplaySettings->EnvQueryDebugCacheDuration);			
		if (Frame >= CutoffFrame)
		{
			EnvQueryDebugCache.Remove(Handle);
		}
	}
#endif // UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
}

void URecallEnvQuerySubsystem::TickEnvQueryQueue()
{
	const uint32 Frame = Recall::Simulation::Utils::GetFrame(this);

	FScopeLock EnvQueryLock(&EnvQueryGuard);

	for (const TPair<FRecallEnvQueryHandle, FRecallEnvQueryData>& EnvQueryData : EnvQueryDataMap)
	{
		if (Frame == EnvQueryData.Value.AsyncEndFrame)
		{
			if (SignalSystem.IsValid())
			{
				SignalSystem->SignalEntity(Recall::Navigation::Signals::EnvQueryDone, EnvQueryData.Value.OwnerEntity);
			}
		}
	}
}

FRecallEnvQueryHandle URecallEnvQuerySubsystem::RequestEnvQuery(const FMassExtendedEntityHandle& Entity,
	const FRecallEnvQueryRequest& Request)
{
	FRecallEnvQueryHandle NewHandle;

	if (!Request.EnvQuery)
	{
		return NewHandle;
	}

	const URecallGameplaySettings* GameplaySettings = GetDefault<URecallGameplaySettings>();
	const uint32 Frame = Recall::Simulation::Utils::GetFrame(this);
	const FRandomStream& RandomStream = RandomNumberSystem->GetRandomStream();

	{
		FScopeLock EnvQueryLock(&EnvQueryGuard);

		const int32 EnvQueryCount = GetConcurrentEnvQuery(Frame);
		const int32 EnvQueryCycle = EnvQueryCount / GameplaySettings->ConcurrentEnvQueryLimit;
		constexpr int32 RandomDeviation = 5;
		const int32 Duration = GetEnvQueryDuration() + RandomDeviation;

		const uint32 AsyncStartFrame = Frame + static_cast<uint32>(Duration * EnvQueryCycle);
		const uint32 AsyncEndFrame = AsyncStartFrame + static_cast<uint32>(Duration) + RandomStream.RandRange(0, RandomDeviation);

		NewHandle = FRecallEnvQueryHandle(++SerialNumberGenerator);

		FRecallEnvQueryData& NewEnvQueryData = EnvQueryDataMap.Add(NewHandle);
		NewEnvQueryData.OwnerEntity = Entity;
		NewEnvQueryData.Request = Request;
		NewEnvQueryData.AsyncStartFrame = AsyncStartFrame;
		NewEnvQueryData.AsyncEndFrame = AsyncEndFrame;
	}

	return NewHandle;
}

bool URecallEnvQuerySubsystem::IsEnvQueryFinished(const FRecallEnvQueryHandle& Handle) const
{
	const uint32 Frame = Recall::Simulation::Utils::GetFrame(this);

	FScopeLock EnvQueryLock(&EnvQueryGuard);
	if (const FRecallEnvQueryData* EnvQueryDataPtr = EnvQueryDataMap.Find(Handle))
	{
		return EnvQueryDataPtr->IsFinished(Frame);
	}
	return false;
}

void URecallEnvQuerySubsystem::ReleaseEnvQuery(FRecallEnvQueryHandle& Handle)
{
	if (!Handle.IsValid())
	{
		return;
	}

	{
		FScopeLock EnvQueryLock(&EnvQueryGuard);
		EnvQueryDataMap.Remove(Handle);
	}

	Handle.Reset();
}

bool URecallEnvQuerySubsystem::GetEnvQueryBestResult(const FRandomStream& RandomStream, FRecallEnvQueryHandle& Handle, FVector& OutLocation)
{
	bool bFound = false;

	if (ensure(EnvQueryGamethread) && ensureAlwaysMsgf(EnvQueryDataMap.Contains(Handle), TEXT("EnvQuery does not exist")))
	{
		TArray<FRecallEnvQueryItem>* Items = nullptr;
		
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		FRecallEnvQueryDebugCache& DebugCache = EnvQueryDebugCache.FindOrAdd(Handle);
		DebugCache.LastUpdateFrame = EnvQueryDataMap[Handle].AsyncEndFrame;
		Items = &DebugCache.Items;
#endif // UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		
		const TArray<FRecallEnvQueryItem> SelectedItems = EnvQueryGamethread->GetSelectedItems(Handle, Items);
		if (SelectedItems.Num())
		{		
			OutLocation = SelectedItems[RandomStream.RandRange(0, SelectedItems.Num() - 1)].Location;
			bFound = true;
			
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
			DebugCache.QueryResult = OutLocation;
#endif // UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		}
	}

	ReleaseEnvQuery(Handle);
	return bFound;
}

int32 URecallEnvQuerySubsystem::GetEnvQueryDuration() const
{
	const URecallGameplaySettings* GameplaySettings = GetDefault<URecallGameplaySettings>();
	const int32 MaxStepCount = Recall::Simulation::Utils::GetMaxStepCount(this);
	return GameplaySettings->EnvQueryDuration + MaxStepCount;
}

int32 URecallEnvQuerySubsystem::GetConcurrentEnvQuery(uint32 Frame) const
{
	int32 Result = 0;

	for (const TPair<FRecallEnvQueryHandle, FRecallEnvQueryData>& EnvQueryData : EnvQueryDataMap)
	{
		if (!EnvQueryData.Value.IsFinished(Frame))
		{
			Result++;
		}
	}

	return Result;
}
