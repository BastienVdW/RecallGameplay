// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "System/Interface/RecallSimulationReactSystemInterface.h"
#include "Mass/ExternalSubsystemTraits.h"

#include "RecallTrackSubsystem.generated.h"

class ARecallTrackEntityActor;

/*
* System to access the track.
*/
UCLASS()
class RECALLTRACK_API URecallTrackSubsystem : 
	public UWorldSubsystem,
	public IRecallSimulationReactSystemInterface
{
	GENERATED_BODY()

public:
	const TWeakObjectPtr<ARecallTrackEntityActor>& GetTrackActor() const;
	TWeakObjectPtr<ARecallTrackEntityActor> GetTrackActor(const FName& AssetName) const;

public:
	void RegisterTrackActor(const TObjectPtr<ARecallTrackEntityActor>& TrackActor);
	void UnregisterTrackActor(const TObjectPtr<ARecallTrackEntityActor>& TrackActor);

protected:
	// UWorldSubsystem implementation Begin
	virtual void Initialize(FSubsystemCollectionBase& Collection) override final;
	virtual void Deinitialize() override final;
	// UWorldSubsystem implementation End

	// IRecallSimulationReactSystemInterface implementation Begin
	virtual void Start(const FRecallSimulationStartParams& Params) override final;
	virtual void Reset() override final;
	virtual void Save(const FRecallSnapshotContext& Context, FInstancedStruct& OutSnapshot) override final;
	virtual void Restore(const FRecallSnapshotContext& Context, const FInstancedStruct& InSnapshot) override final;
	// IRecallSimulationReactSystemInterface implementation End

private:
	UPROPERTY(Transient)
	TMap<FName, TWeakObjectPtr<ARecallTrackEntityActor>> TrackActorMap;
	UPROPERTY(Transient)
	TWeakObjectPtr<ARecallTrackEntityActor> DefaultTrackActor;
};

template<>
struct TMassExternalSubsystemTraits<URecallTrackSubsystem> final
{
	enum
	{
		GameThreadOnly = false
	};
};
