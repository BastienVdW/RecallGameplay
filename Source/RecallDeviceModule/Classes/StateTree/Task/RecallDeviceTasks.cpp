// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallDeviceTasks.h"

#include "Data/Device/RecallDeviceAsset.h"
#include "MassExtendedCommandBuffer.h"
#include "MassExtendedEntityConfigAsset.h"
#include "MassExtendedEntityView.h"
#include "MassExtendedExecutionContext.h"
#include "RecallSignalSubsystem.h"
#include "StateTree/RecallStateTreeExecutionContext.h"
#include "StateTree/Task/RecallCommonTasks.h"
#include "StateTreeLinker.h"
#include "Simulation/Device/RecallDeviceFragments.h"
#include "Simulation/Device/RecallDeviceTypes.h"
#include "Simulation/GameplayTag/RecallGameplayTagFragments.h"
#include "Simulation/StateTree/RecallStateTreeSignalTypes.h"
#include "Simulation/Transform/RecallTransformFragments.h"
#include "System/Asset/RecallAssetManagerSubsystem.h"
#include "System/Device/RecallDeviceSubsystem.h"
#include "System/Entity/RecallEntitySubsystem.h"
#include "System/Physics/RecallPhysicsSubsystem.h"
#include "System/Representation/RecallRepresentationEventSubsystem.h"
#include "Utility/Device/RecallDeviceUtils.h"

//----------------------------------------------------------------------//
// FRecallPlaceDeviceTask
//----------------------------------------------------------------------//
FRecallPlaceDeviceTask::FRecallPlaceDeviceTask()
	: Super()
{
	bShouldStateChangeOnReselect = false;
}

bool FRecallPlaceDeviceTask::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(TransformFragmentHandle);
	Linker.LinkExternalData(GameplayTagFragmentHandle);
	Linker.LinkExternalData(DevicePlacerFragmentHandle);
	Linker.LinkExternalData(BodyFragmentHandle);
	Linker.LinkExternalData(EntitySystemHandle);
	Linker.LinkExternalData(PhysicsSystemHandle);
	Linker.LinkExternalData(AssetManagerSystemHandle);
	Linker.LinkExternalData(RepresentationEventSystemHandle);
	return true;
}

EStateTreeRunStatus FRecallPlaceDeviceTask::EnterState(FStateTreeExecutionContext& Context,
                                                      const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.bValidPosition = false;
	
	const URecallDeviceSubsystem& DeviceSystem = URecallDeviceSubsystem::GetRef(Context.GetWorld());
	const TObjectPtr<const URecallDeviceAsset> DeviceAsset = DeviceSystem.GetDeviceAsset(InstanceData.DeviceTag);
	if (!DeviceAsset || DeviceAsset->EntityConfig.IsNull())
	{
		UE_LOG(LogRecallAsset, Warning, TEXT("%hs Invalid device asset"), __FUNCTION__);
		return EStateTreeRunStatus::Failed;
	}

	FRecallGameplayTagFragment& GameplayTagFragment = Context.GetExternalData(GameplayTagFragmentHandle);

	if (!GameplayTagFragment.GameplayTagCountMap.HasTag(State_DeviceInvalid))
	{
		GameplayTagFragment.GameplayTagCountMap.AddTag(State_DeviceInvalid);
	}
	
	URecallAssetManagerSubsystem& AssetManagerSystem = Context.GetExternalData(AssetManagerSystemHandle);
	InstanceData.DeviceAssetHandle = AssetManagerSystem.RequestAsset(DeviceAsset->EntityConfig);

	FRecallStateTreeExecutionContext& RecallContext = static_cast<FRecallStateTreeExecutionContext&>(Context);
	RecallContext.GetSignalSystem().DelaySignalEntity(
		Recall::StateTree::Signals::TickRequired, RecallContext.GetEntity(), 0.1f);
	
	return Super::EnterState(Context, Transition);
}

void FRecallPlaceDeviceTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	FRecallGameplayTagFragment& GameplayTagFragment = Context.GetExternalData(GameplayTagFragmentHandle);
	if (GameplayTagFragment.GameplayTagCountMap.HasTag(State_DevicePlace))
	{
		GameplayTagFragment.GameplayTagCountMap.RemoveTag(State_DevicePlace);
	}
	if (GameplayTagFragment.GameplayTagCountMap.HasTag(State_DeviceInvalid))
	{
		GameplayTagFragment.GameplayTagCountMap.RemoveTag(State_DeviceInvalid);
	}

	// Release device entity
	FRecallDevicePlacerFragment& DevicePlacerFragment = Context.GetExternalData(DevicePlacerFragmentHandle);
	if (DevicePlacerFragment.PlaceDeviceEntity.IsSet())
	{
		FRecallStateTreeExecutionContext& RecallContext = static_cast<FRecallStateTreeExecutionContext&>(Context);
		RecallContext.GetMassExecutionContext().Defer().DestroyEntity(DevicePlacerFragment.PlaceDeviceEntity);
	}	
	DevicePlacerFragment.PlaceDeviceEntity.Reset();
	
	// Release entity config asset
	URecallAssetManagerSubsystem& AssetManagerSystem = Context.GetExternalData(AssetManagerSystemHandle);
	AssetManagerSystem.ReleaseAsset(InstanceData.DeviceAssetHandle);
	
	return Super::ExitState(Context, Transition);
}

EStateTreeRunStatus FRecallPlaceDeviceTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	const FRecallStateTreeExecutionContext& RecallContext = static_cast<FRecallStateTreeExecutionContext&>(Context);

	FMassExtendedEntityHandle DeviceEntity;
	if (!GetOrSpawnDeviceEntityEntityAsync(Context, DeviceEntity))
	{
		RecallContext.GetSignalSystem().DelaySignalEntity(
			Recall::StateTree::Signals::TickRequired, RecallContext.GetEntity(), 0.1f);
		return Super::Tick(Context, DeltaTime);
	}
	
	UpdateDeviceEntityLocation(Context, DeviceEntity);

	if (bCompleteOnPlace)
	{
		return InstanceData.bValidPosition ? EStateTreeRunStatus::Succeeded : EStateTreeRunStatus::Failed;
	}
	
	return Super::Tick(Context, DeltaTime);
}

bool FRecallPlaceDeviceTask::GetOrSpawnDeviceEntityEntityAsync(FStateTreeExecutionContext& Context, FMassExtendedEntityHandle& OutEntity) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	
	FRecallDevicePlacerFragment& DevicePlacerFragment = Context.GetExternalData(DevicePlacerFragmentHandle);
	if (DevicePlacerFragment.PlaceDeviceEntity.IsSet())
	{
		OutEntity = DevicePlacerFragment.PlaceDeviceEntity;
		return true;
	}
	
	URecallAssetManagerSubsystem& AssetManagerSystem = Context.GetExternalData(AssetManagerSystemHandle);
	if (!AssetManagerSystem.IsAssetLoaded(InstanceData.DeviceAssetHandle))
	{
		return false;
	}
	
	FRecallGameplayTagFragment& GameplayTagFragment = Context.GetExternalData(GameplayTagFragmentHandle);
	if (!GameplayTagFragment.GameplayTagCountMap.HasTag(State_DevicePlace))
	{		
		GameplayTagFragment.GameplayTagCountMap.AddTag(State_DevicePlace);
	}

	SpawnDeviceEntityEntityChecked(Context);

	// Must wait until PlaceDeviceEntity is set by the deferred command
	return false;
}

void FRecallPlaceDeviceTask::SpawnDeviceEntityEntityChecked(FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	
	const URecallAssetManagerSubsystem& AssetManagerSystem = Context.GetExternalData(AssetManagerSystemHandle);
	InstanceData.DeviceEntityConfig = AssetManagerSystem.GetLoadedAsset<UMassExtendedEntityConfigAsset>(
		InstanceData.DeviceAssetHandle);
	checkf(IsValid(InstanceData.DeviceEntityConfig), TEXT("%hs Invalid entity config asset"), __FUNCTION__);
	
	FRecallStateTreeExecutionContext& RecallContext = static_cast<FRecallStateTreeExecutionContext&>(Context);
	const FMassExtendedEntityHandle OwnerEntity = RecallContext.GetEntity();

	const FRecallTransformFragment& TransformFragment = Context.GetExternalData(TransformFragmentHandle);
	const FRecallPhysicsBodyFragment* BodyFragmentPtr = Context.GetExternalDataPtr(BodyFragmentHandle);
	InstanceData.DevicePosition = Recall::Device::Utils::GetDevicePosition(InstanceData.DeviceEntityConfig,
		TransformFragment, BodyFragmentPtr, bSnapToGrid, GridSize);

	URecallEntitySubsystem& EntitySystem = Context.GetExternalData(EntitySystemHandle);
	URecallRepresentationEventSubsystem& RepresentationEventSystem = Context.GetExternalData(RepresentationEventSystemHandle);
	Recall::Device::Utils::SpawnDevicePlaceEntity(RecallContext.GetMassExecutionContext(), OwnerEntity, InstanceData.DeviceEntityConfig,
		InstanceData.DevicePosition, DeviceColorParameterName, PlacementInvalidColor, EntitySystem, RepresentationEventSystem);
}

void FRecallPlaceDeviceTask::UpdateDeviceEntityLocation(FStateTreeExecutionContext& Context, const FMassExtendedEntityHandle& DeviceEntity) const
{
	FRecallStateTreeExecutionContext& RecallContext = static_cast<FRecallStateTreeExecutionContext&>(Context);
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	const FRecallTransformFragment& TransformFragment = Context.GetExternalData(TransformFragmentHandle);
	const FRecallPhysicsBodyFragment* BodyFragmentPtr = Context.GetExternalDataPtr(BodyFragmentHandle);

	const FMassExtendedEntityView DeviceView(RecallContext.GetEntityManager(), DeviceEntity);
	FRecallTransformFragment& DeviceTransformFragment = DeviceView.GetFragmentData<FRecallTransformFragment>();
	InstanceData.DevicePosition = Recall::Device::Utils::GetDevicePosition(InstanceData.DeviceEntityConfig,
		TransformFragment, BodyFragmentPtr,	bSnapToGrid, GridSize);
	DeviceTransformFragment.Position = InstanceData.DevicePosition;

	const URecallPhysicsSubsystem& PhysicsSystem = Context.GetExternalData(PhysicsSystemHandle);
	InstanceData.bValidPosition = Recall::Device::Utils::CheckDevicePosition(RecallContext.GetEntityManager(),
		PhysicsSystem, DeviceEntity, InstanceData.DevicePosition);

	URecallRepresentationEventSubsystem& RepresentationEventSystem = Context.GetExternalData(RepresentationEventSystemHandle);
	Recall::Device::Utils::RequestChangeDeviceColor(RecallContext.GetEntityManager(), DeviceEntity, DeviceColorParameterName,
		InstanceData.bValidPosition ? PlacementValidColor : PlacementInvalidColor, RepresentationEventSystem);

	FRecallGameplayTagFragment& GameplayTagFragment = Context.GetExternalData(GameplayTagFragmentHandle);
	if (InstanceData.bValidPosition == GameplayTagFragment.GameplayTagCountMap.HasTag(State_DeviceInvalid))
	{
		if (InstanceData.bValidPosition)
		{
			GameplayTagFragment.GameplayTagCountMap.RemoveTag(State_DeviceInvalid);
		}
		else
		{
			GameplayTagFragment.GameplayTagCountMap.AddTag(State_DeviceInvalid);
		}
	}
}
