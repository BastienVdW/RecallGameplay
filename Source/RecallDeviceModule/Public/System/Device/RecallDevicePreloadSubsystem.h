// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "System/Interface/RecallSimulationReactSystemInterface.h"
#include "System/Asset/RecallAssetManagerTypes.h"

#include "RecallDevicePreloadSubsystem.generated.h"

class URecallDeviceAsset;

UCLASS()
class RECALLDEVICEMODULE_API URecallDevicePreloadSubsystem :
	public UWorldSubsystem,
	public IRecallSimulationReactSystemInterface
{
	GENERATED_BODY()

public:
	// UWorldSubsystem implementation Begin
protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override final;
	virtual void Deinitialize() override final;
	// UWorldSubsystem implementation End

	// IRecallSimulationReactSystemInterface implementation Begin
public:
	virtual void Start(const FRecallSimulationStartParams& Params) override final;
	virtual void Reset() override final;
	virtual void Save(const FRecallSnapshotContext& Context, FInstancedStruct& OutSnapshot) override final;
	virtual void Restore(const FRecallSnapshotContext& Context, const FInstancedStruct& InSnapshot) override final;
	// IRecallSimulationReactSystemInterface implementation End

private:
	// Entity config preloading data
	UPROPERTY(Transient)
	TMap<FName, FRecallAssetLoadHandle> PreloadedEntityConfigHandles;

	UPROPERTY(Transient)
	TWeakObjectPtr<class URecallAssetManagerSubsystem> AssetManagerSystem;

	// Helper methods for entity config preloading
	void PreloadDeviceEntityConfigs();
}; 