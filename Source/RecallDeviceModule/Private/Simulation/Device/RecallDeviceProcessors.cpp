// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallDeviceProcessors.h"

#include "Data/Device/RecallDeviceAsset.h"
#include "Entity/RecallDeviceSpawnCommand.h"
#include "Input/RecallDeviceInputOptionTypes.h"
#include "Kismet/GameplayStatics.h"
#include "MassExtendedEntityView.h"
#include "MassExtendedExecutionContext.h"
#include "RecallSignalSubsystem.h"
#include "Simulation/Device/RecallDeviceFragments.h"
#include "Simulation/GameplayTag/RecallGameplayTagFragments.h"
#include "Simulation/Player/RecallPlayerProcessorGroupTypes.h"
#include "Simulation/Controller/RecallControllerFragments.h"
#include "Simulation/Representation/RecallActorRepresentationFragments.h"
#include "Simulation/StateTree/RecallStateTreeProcessorGroupTypes.h"
#include "Simulation/StateTree/RecallStateTreeSignalTypes.h"
#include "Simulation/Transform/RecallTransformFragments.h"
#include "System/Device/RecallDeviceSubsystem.h"
#include "System/Entity/RecallEntityAsyncSpawnSubsystem.h"
#include "System/Input/RecallInputQueueSubsystem.h"
#include "Utility/Device/RecallDeviceUtils.h"
#include "Utility/GameplayTag/RecallGameplayTagUtils.h"

//----------------------------------------------------------------------//
// URecallDeviceBuildProcessor
//----------------------------------------------------------------------//
URecallDeviceBuildProcessor::URecallDeviceBuildProcessor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EExtendedProcessorExecutionFlags::All);
	ProcessingPhase = EMassExtendedProcessingPhase::PrePhysics;
	ExecutionOrder.ExecuteAfter.Add(Recall::Player::ProcessorGroupNames::Input);
	ExecutionOrder.ExecuteBefore.Add(Recall::StateTree::ProcessorGroupNames::StateTreeUpdate);
}

void URecallDeviceBuildProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallDeviceBuildProcessor::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallDevicePlacerFragment>(EMassExtendedFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FRecallControllerFragment>(EMassExtendedFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FRecallGameplayTagFragment>(EMassExtendedFragmentAccess::ReadOnly, EMassExtendedFragmentPresence::Optional);
	EntityQuery.AddSubsystemRequirement<URecallSignalSubsystem>(EMassExtendedFragmentAccess::ReadWrite);
	EntityQuery.AddSubsystemRequirement<URecallInputQueueSubsystem>(EMassExtendedFragmentAccess::ReadWrite);
	EntityQuery.AddSubsystemRequirement<URecallEntityAsyncSpawnSubsystem>(EMassExtendedFragmentAccess::ReadWrite);
	EntityQuery.RegisterWithProcessor(*this);	
}

/**
 * Spawn the device attached to the device slot entity.
 */
static void BuildDevice(FMassExtendedExecutionContext& Context, const FString& DeviceName,
	const FMassExtendedEntityHandle DevicePlacerEntity,
	const FRecallDevicePlacerFragment& DevicePlacerFragment, const FRecallGameplayTagFragment* GameplayTagFragmentPtr)
{
	const URecallDeviceSubsystem& DeviceSystem = URecallDeviceSubsystem::GetRef(Context.GetWorld());
	const TObjectPtr<const URecallDeviceAsset> DeviceAsset = DeviceSystem.GetDeviceAssetByTagName(*DeviceName);
	if (!Recall::Device::Utils::EvaluateDeviceCost(Context.GetWorld(), DevicePlacerEntity, DeviceAsset))
	{
		return;
	}	
	
	const FMassExtendedEntityManager& EntityManager = Context.GetEntityManagerChecked();
	if (!ensure(EntityManager.IsEntityValid(DevicePlacerFragment.BuildDeviceSlotEntity)))
	{
		return;
	}
	
	const FMassExtendedEntityView BuildDeviceSlotView(EntityManager, DevicePlacerFragment.BuildDeviceSlotEntity);
	const FRecallTransformFragment& BuildDeviceSlotTransformFragment = BuildDeviceSlotView.GetFragmentData<FRecallTransformFragment>();

	// Hide the device slot when device is built.
	FRecallActorRepresentationFragment* const BuildDeviceSlotActorFragmentPtr = BuildDeviceSlotView.GetFragmentDataPtr<FRecallActorRepresentationFragment>();
	if (BuildDeviceSlotActorFragmentPtr != nullptr)
	{
		BuildDeviceSlotActorFragmentPtr->bHideActor = true;
	}
					
	FRecallDeviceSpawnCommand SpawnCommand;

	if (GameplayTagFragmentPtr != nullptr)
	{
		SpawnCommand.SpawnTags.AppendTags(Recall::GameplayTag::Utils::GetFactionTags(
			GameplayTagFragmentPtr->GameplayTagCountMap.GetTags()));
	}

	FRecallEntityAsyncSpawnParameters SpawnParameters;
	SpawnParameters.ParentEntity = DevicePlacerFragment.BuildDeviceSlotEntity;
	SpawnParameters.SpawnCommand = FInstancedStruct::Make<FRecallDeviceSpawnCommand>(SpawnCommand);
					
	URecallEntityAsyncSpawnSubsystem& EntityAsyncSpawnSystem = Context.GetMutableSubsystemChecked<URecallEntityAsyncSpawnSubsystem>();
	EntityAsyncSpawnSystem.SpawnEntityAsync(DeviceAsset->EntityConfig, BuildDeviceSlotTransformFragment.Position,
		BuildDeviceSlotTransformFragment.Rotation, SpawnParameters);

	Recall::Device::Utils::ConsumeDeviceCost(Context.GetWorld(), DevicePlacerEntity, DeviceAsset);
}

void URecallDeviceBuildProcessor::Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_DeviceBuild_Execute);

	EntityQuery.ForEachEntityChunk(Context, [](FMassExtendedExecutionContext& Context)
	{		
		URecallInputQueueSubsystem& InputQueueSystem = Context.GetMutableSubsystemChecked<URecallInputQueueSubsystem>();
		URecallSignalSubsystem& SignalSystem = Context.GetMutableSubsystemChecked<URecallSignalSubsystem>();

		const TConstArrayView<FRecallControllerFragment> PlayerControllerList = Context.GetFragmentView<FRecallControllerFragment>();
		const TConstArrayView<FRecallGameplayTagFragment> GameplayTagList = Context.GetFragmentView<FRecallGameplayTagFragment>();

		const TArrayView<FRecallDevicePlacerFragment> DevicePlacerList = Context.GetMutableFragmentView<FRecallDevicePlacerFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			FRecallDevicePlacerFragment& DevicePlacerFragment = DevicePlacerList[EntityIndex];
			if (!DevicePlacerFragment.bBuildDevice)
			{
				continue;
			}

			const FRecallControllerFragment& PlayerControllerFragment = PlayerControllerList[EntityIndex];

			// Wait for any input to be received.
			FRecallInput Input;
			if (!InputQueueSystem.GetFrameInput(PlayerControllerFragment.ControllerID, Input))
			{
				continue;
			}
			
			const FMassExtendedEntityHandle Entity = Context.GetEntity(EntityIndex);
			const FRecallGameplayTagFragment* const GameplayTagFragmentPtr = GameplayTagList.IsValidIndex(EntityIndex) ?
				&GameplayTagList[EntityIndex] : nullptr;

			// Wait for the player to release the build device input.
			if (UGameplayStatics::HasOption(Input.Options, Recall::Input::Option::BuildDevice))
			{
				const FString DeviceName = UGameplayStatics::ParseOption(Input.Options, Recall::Input::Option::BuildDevice);

				// Spawn the device.
				BuildDevice(Context, DeviceName, Entity, DevicePlacerFragment, GameplayTagFragmentPtr);

				// End the device build process.
				SignalSystem.SignalEntity(Recall::StateTree::Signals::TickRequired, Entity);
				DevicePlacerFragment.bBuildDevice = false;
			}
		}
	});
}
