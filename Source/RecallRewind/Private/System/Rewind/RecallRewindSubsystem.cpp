// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "System/Rewind/RecallRewindSubsystem.h"

#include "RecallRewindSnapshot.h"
#include "System/Simulation/RecallSimulationSubsystem.h"
#include "System/Snapshot/RecallSnapshotSubsystem.h"
#include "Utility/Simulation/RecallSimulationUtils.h"

void URecallRewindSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	Collection.InitializeDependency<URecallSimulationSubsystem>();
	Collection.InitializeDependency<URecallSnapshotSubsystem>();

	SnapshotSystem = UWorld::GetSubsystem<URecallSnapshotSubsystem>(GetWorld());

	if (bEnableRewind)
	{
		if (URecallSimulationSubsystem* SimulationSystem = UWorld::GetSubsystem<URecallSimulationSubsystem>(GetWorld()))
		{
			SimulationSystem->OnFrameEnd.AddUObject(this, &ThisClass::OnFrameEnd);
		}

		ensureAlwaysMsgf(GetRewindMaxFrameCount() <= RECALL_REWIND_MAX, TEXT("Not enough room to allocate rewind snapshots"));
	}
}

void URecallRewindSubsystem::Deinitialize()
{
	Super::Deinitialize();

	SnapshotSystem.Reset();

	if (bEnableRewind)
	{
		if (URecallSimulationSubsystem* SimulationSystem = UWorld::GetSubsystem<URecallSimulationSubsystem>(GetWorld()))
		{
			SimulationSystem->OnFrameEnd.RemoveAll(this);
		}
	}
}

void URecallRewindSubsystem::Reset()
{
	Snapshots.Reset();
}

void URecallRewindSubsystem::Save(const FRecallSnapshotContext& Context, FInstancedStruct& OutSnapshot)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR(TEXT("URecallRewindSubsystem::Save"));
	QUICK_SCOPE_CYCLE_COUNTER(Recall_Rewind_Save);

	if (bEnableRewind && Context.IsSnapshot() && bSaveSnapshot)
	{
		OutSnapshot.InitializeAs<FRecallRewindSnapshot>();

		FRecallRewindSnapshot& Data = OutSnapshot.GetMutable<FRecallRewindSnapshot>();
		Data.Frames.SetNum(Snapshots.Num());

		for (int32 FrameIndex = 0; FrameIndex < Data.Frames.Num(); FrameIndex++)
		{
			check(Snapshots[FrameIndex].IsValid());
			Data.Frames[FrameIndex] = *Snapshots[FrameIndex].Get();
		}
	}
}

void URecallRewindSubsystem::Restore(const FRecallSnapshotContext& Context, const FInstancedStruct& InSnapshot)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR(TEXT("URecallRewindSubsystem::Restore"));
	QUICK_SCOPE_CYCLE_COUNTER(Recall_Rewind_Restore);

	if (Context.IsSnapshot())
	{
		if (const FRecallRewindSnapshot* DataPtr = InSnapshot.GetPtr<FRecallRewindSnapshot>())
		{
			Snapshots.Empty(DataPtr->Frames.Num());

			for (int32 FrameIndex = 0; FrameIndex < DataPtr->Frames.Num(); FrameIndex++)
			{
				Snapshots.Add(MakeShared<FRecallSimulationSnapshot>(DataPtr->Frames[FrameIndex]));
			}
		}
	}
	else
	{
		for (int32 SnapshotIndex = Snapshots.Num() - 1; SnapshotIndex >= 0; SnapshotIndex--)
		{
			const TSharedPtr<FRecallSimulationSnapshot>& Snapshot = Snapshots[SnapshotIndex];
			if (!ensure(Snapshot.IsValid()) || Snapshot->Frame >= Context.Frame)
			{
				Snapshots.RemoveAt(SnapshotIndex);
			}
			else
			{
				break;
			}
		}
	}
}

void URecallRewindSubsystem::Rewind()
{
	if (bEnableRewind)
	{
		FScopeLock Lock(&DataGuard);
		bIsRewinding = true;
	}
}

void URecallRewindSubsystem::OnFrameEnd(uint32 Frame)
{
	if (bIsRewinding)
	{
		if (Snapshots.Num())
		{
			const TSharedPtr<FRecallSimulationSnapshot> Snapshot = Snapshots.PopValue();
			if (ensure(Snapshot.IsValid()) && ensure(SnapshotSystem.IsValid()))
			{
				SnapshotSystem->LoadSnapshot(*Snapshot.Get(), RewindReason);
			}
		}

		bIsRewinding = false;
	}
	else
	{
		const bool bIsSnapshotFrame = (Frame % GetRewindSnapshotFrequency()) == 0;
		if (SnapshotSystem.IsValid() && bIsSnapshotFrame && !bIsRewinding)
		{
			const int32 RewindMaxFrameCount = FMath::Min(GetRewindMaxFrameCount(), RECALL_REWIND_MAX);
			if (Snapshots.Num() >= RewindMaxFrameCount)
			{
				Snapshots.PopFront();
			}

			TSharedPtr<FRecallSimulationSnapshot> NewSnapshot = MakeShared<FRecallSimulationSnapshot>();
			SnapshotSystem->TakeSnapshot(*NewSnapshot.Get(), RewindReason);
			Snapshots.Add(NewSnapshot);
		}
	}
}

int32 URecallRewindSubsystem::GetRewindFrameRate() const
{
	const int32 FramesPerSeconds = Recall::Simulation::Utils::GetFramesPerSeconds(this);
	return FMath::CeilToInt(static_cast<float>(FramesPerSeconds) / FMath::Max(1.0f, RewindSpeed));
}

int32 URecallRewindSubsystem::GetRewindSnapshotFrequency() const
{
	const int32 FramesPerSeconds = Recall::Simulation::Utils::GetFramesPerSeconds(this);
	const int32 RewindFrameRate = GetRewindFrameRate();
	return FMath::Max(1, FMath::CeilToInt(static_cast<float>(FramesPerSeconds) / static_cast<float>(RewindFrameRate)));
}

int32 URecallRewindSubsystem::GetRewindMaxFrameCount() const
{
	return FMath::CeilToInt(static_cast<float>(GetRewindFrameRate()) * RewindMaxDuration);
}
