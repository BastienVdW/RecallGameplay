// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "StateTree/RecallStateTreeTaskBase.h"
#include "MassExtendedEntityTypes.h"
#include "System/Asset/RecallAssetManagerTypes.h"

#include "RecallDeviceTasks.generated.h"

USTRUCT()
struct RECALLDEVICEMODULE_API FRecallPlaceDeviceTaskInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category=Parameter, meta=(GameplayTagFilter="Device"))
	FGameplayTag DeviceTag;
	
	UPROPERTY(EditAnywhere, Category=Output)
	TObjectPtr<class UMassExtendedEntityConfigAsset> DeviceEntityConfig;
	
	UPROPERTY(EditAnywhere, Category=Output)
	FVector DevicePosition = FVector::ZeroVector;

	/**
	 * True if the device can be placed on the DevicePosition.
	 */
	UPROPERTY(EditAnywhere, Category=Output)
	bool bValidPosition = false;

	UPROPERTY(Transient)
	FRecallAssetLoadHandle DeviceAssetHandle;
};

USTRUCT(meta=(DisplayName="Place Device"))
struct RECALLDEVICEMODULE_API FRecallPlaceDeviceTask : public FRecallStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallPlaceDeviceTaskInstanceData;

public:
	FRecallPlaceDeviceTask();

protected:
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

protected:
	/**
	 * Name of the parameter to adjust the color of the device material if placement position is valid, or not.
	 */
	UPROPERTY(EditAnywhere)
	FName DeviceColorParameterName = TEXT("Color");

	/**
	 * Color of the device when placement position is valid.
	 */
	UPROPERTY(EditAnywhere)
	FColor PlacementValidColor = FColor::Green;
	
	/**
	 * Color of the device when placement position is invalid.
	 */
	UPROPERTY(EditAnywhere)
	FColor PlacementInvalidColor = FColor::Red;

	/**
	 * Whether the device should snap to the grid.
	 */
	UPROPERTY(EditAnywhere)
	bool bSnapToGrid = true;

	/**
	 * Size of the grid to use to snap the device.
	 */
	UPROPERTY(EditAnywhere, meta=(Units=Centimeters, EditCondition=bSnapToGrid))
	float GridSize = 100.0f;

	/**
	 * Instantly complete the task as soon as the device can be placed.
	 * Succeed if the device can be placed, or fail if invalid.
	 */
	UPROPERTY(EditAnywhere)
	bool bCompleteOnPlace = false;

private:
	TStateTreeExternalDataHandle<struct FRecallTransformFragment> TransformFragmentHandle;
	TStateTreeExternalDataHandle<struct FRecallGameplayTagFragment> GameplayTagFragmentHandle;
	TStateTreeExternalDataHandle<struct FRecallDevicePlacerFragment> DevicePlacerFragmentHandle;
	TStateTreeExternalDataHandle<struct FRecallPhysicsBodyFragment, EStateTreeExternalDataRequirement::Optional> BodyFragmentHandle;
	TStateTreeExternalDataHandle<class URecallEntitySubsystem> EntitySystemHandle;
	TStateTreeExternalDataHandle<class URecallPhysicsSubsystem> PhysicsSystemHandle;
	TStateTreeExternalDataHandle<class URecallAssetManagerSubsystem> AssetManagerSystemHandle;
	TStateTreeExternalDataHandle<class URecallRepresentationEventSubsystem> RepresentationEventSystemHandle;

	bool GetOrSpawnDeviceEntityEntityAsync(FStateTreeExecutionContext& Context, FMassExtendedEntityHandle& OutEntity) const;
	void SpawnDeviceEntityEntityChecked(FStateTreeExecutionContext& Context) const;
	void UpdateDeviceEntityLocation(FStateTreeExecutionContext& Context, const FMassExtendedEntityHandle& DeviceEntity) const;
};
