// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallInventoryProcessors.h"

#include "Data/Inventory/RecallInventoryInputTypes.h"
#include "MassExtendedExecutionContext.h"
#include "Simulation/GameplayTag/RecallGameplayTagFragments.h"
#include "Simulation/Controller/RecallControllerFragments.h"
#include "Simulation/StateTree/RecallStateTreeFragments.h"
#include "System/AI/RecallStateTreeSubsystem.h"
#include "System/Input/RecallInputQueueSubsystem.h"

UE_DEFINE_GAMEPLAY_TAG_COMMENT(StateTreeEvent_UseInventoryItem, 	"StateTreeEvent.UseInventoryItem",		"Use Inventory Item");

//----------------------------------------------------------------------//
// URecallInventoryInputProcessor
//----------------------------------------------------------------------//
URecallInventoryInputProcessor::URecallInventoryInputProcessor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EExtendedProcessorExecutionFlags::All);
	ProcessingPhase = EMassExtendedProcessingPhase::PrePhysics;
}

void URecallInventoryInputProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallInventoryInputProcessor::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallControllerFragment>(EMassExtendedFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FRecallGameplayTagFragment>(EMassExtendedFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FRecallStateTreeInstanceFragment>(EMassExtendedFragmentAccess::ReadOnly);
	EntityQuery.AddSubsystemRequirement<URecallInputQueueSubsystem>(EMassExtendedFragmentAccess::ReadOnly);
	EntityQuery.AddSubsystemRequirement<URecallStateTreeSubsystem>(EMassExtendedFragmentAccess::ReadWrite);
}

void URecallInventoryInputProcessor::Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_Equipment_Representation);

	EntityQuery.ForEachEntityChunk(Context, [](FMassExtendedExecutionContext& Context)
	{
		const URecallInputQueueSubsystem& InputQueueSystem = Context.GetSubsystemChecked<URecallInputQueueSubsystem>();
		URecallStateTreeSubsystem& StateTreeSystem = Context.GetMutableSubsystemChecked<URecallStateTreeSubsystem>();
		
		const TConstArrayView<FRecallGameplayTagFragment> GameplayTagList = Context.GetFragmentView<FRecallGameplayTagFragment>();
		const TConstArrayView<FRecallControllerFragment> PlayerControllerList = Context.GetFragmentView<FRecallControllerFragment>();
		const TConstArrayView<FRecallStateTreeInstanceFragment> StateTreeInstanceList = Context.GetFragmentView<FRecallStateTreeInstanceFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			const FRecallControllerFragment& PlayerControllerFragment = PlayerControllerList[EntityIndex];

			FRecallInput Input;
			if (!InputQueueSystem.GetFrameInput(PlayerControllerFragment.ControllerID, Input))
			{
				continue;
			}

			const FRecallInventoryInputCommand InventoryInputCommand(Input.Options);
			if (!InventoryInputCommand.IsValid())
			{
				continue;
			}

			const FRecallGameplayTagFragment& GameplayTagFragment = GameplayTagList[EntityIndex];
			if (!GameplayTagFragment.GameplayTagCountMap.HasTag(InventoryInputCommand.ItemTag))
			{
				continue;
			}

			const FRecallStateTreeInstanceFragment& StateTreeInstanceFragment = StateTreeInstanceList[EntityIndex];

			FStateTreeEvent Event;
			Event.Tag = StateTreeEvent_UseInventoryItem;
			Event.Payload = FInstancedStruct::Make(InventoryInputCommand.ItemTag);
			Event.Origin = TEXT("Use Inventory Item");
			
			StateTreeSystem.SendStateTreeEvent(StateTreeInstanceFragment.InstanceHandle, Event);
		}
	});
}
