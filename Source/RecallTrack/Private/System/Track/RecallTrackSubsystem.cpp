// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "System/Track/RecallTrackSubsystem.h"

#include "Actor/RecallTrackEntityActor.h"

void URecallTrackSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void URecallTrackSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void URecallTrackSubsystem::Start(const FRecallSimulationStartParams& Params)
{
	TArray<TWeakObjectPtr<ARecallTrackEntityActor>> TrackActors;
	TrackActorMap.GenerateValueArray(TrackActors);

	if (TrackActors.Num())
	{
		DefaultTrackActor = TrackActors[0];
	}
}

void URecallTrackSubsystem::Reset()
{
	DefaultTrackActor.Reset();;
}

void URecallTrackSubsystem::Save(const FRecallSnapshotContext& Context, FInstancedStruct& OutSnapshot)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR(TEXT("URecallTrackSubsystem::Save"));
	QUICK_SCOPE_CYCLE_COUNTER(Recall_Track_Save);
}

void URecallTrackSubsystem::Restore(const FRecallSnapshotContext& Context, const FInstancedStruct& InSnapshot)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR(TEXT("URecallTrackSubsystem::Restore"));
	QUICK_SCOPE_CYCLE_COUNTER(Recall_Track_Restore);
}

const TWeakObjectPtr<ARecallTrackEntityActor>& URecallTrackSubsystem::GetTrackActor() const
{
	return DefaultTrackActor;
}

TWeakObjectPtr<ARecallTrackEntityActor> URecallTrackSubsystem::GetTrackActor(const FName& AssetName) const
{
	return TrackActorMap.FindRef(AssetName);
}

void URecallTrackSubsystem::RegisterTrackActor(const TObjectPtr<ARecallTrackEntityActor>& TrackActor)
{
	if (!ensure(TrackActor))
	{
		return;
	}
	
	TrackActorMap.Add(TrackActor->GetEntityAssetName(), TrackActor);
}

void URecallTrackSubsystem::UnregisterTrackActor(const TObjectPtr<ARecallTrackEntityActor>& TrackActor)
{
	if (!ensure(TrackActor))
	{
		return;
	}
	
	TrackActorMap.Remove(TrackActor->GetEntityAssetName());
}
