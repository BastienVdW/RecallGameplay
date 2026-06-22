// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallInventoryProcessors.h"

#include "Data/Inventory/RecallInventoryInputTypes.h"
#include "MassExecutionContext.h"
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
	ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::All);
	ProcessingPhase = EMassProcessingPhase::PrePhysics;
}

void URecallInventoryInputProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallInventoryInputProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallControllerFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FRecallGameplayTagFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FRecallStateTreeInstanceFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddSubsystemRequirement<URecallInputQueueSubsystem>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddSubsystemRequirement<URecallStateTreeSubsystem>(EMassFragmentAccess::ReadWrite);
}

void URecallInventoryInputProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_Equipment_Representation);

	EntityQuery.ForEachEntityChunk(Context, [](FMassExecutionContext& Context)
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
