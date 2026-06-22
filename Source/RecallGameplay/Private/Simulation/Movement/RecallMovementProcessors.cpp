// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallMovementProcessors.h"

#include "Async/ParallelFor.h"
#include "Desync/RecallDesyncLog.h"
#include "MassExtendedExecutionContext.h"
#include "Physics/Common/RecallPhysicsCommonObjects.h"
#include "Simulation/Attribute/RecallAttributeFragments.h"
#include "Simulation/Controller/RecallControllerFragments.h"
#include "Simulation/GameplayTag/RecallGameplayTagFragments.h"
#include "Simulation/Movement/RecallMovementFragments.h" 
#include "Simulation/Movement/RecallMovementProcessorGroupTypes.h"
#include "Simulation/Physics/RecallPhysicsBodyFragment.h" 
#include "Simulation/Physics/RecallPhysicsProcessorGroupTypes.h"
#include "System/Physics/RecallPhysicsSubsystem.h"
#include "Utility/Movement/RecallMovementUtils.h"

//----------------------------------------------------------------------//
// URecallMovementProcessor
//----------------------------------------------------------------------//
URecallMovementProcessor::URecallMovementProcessor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EExtendedProcessorExecutionFlags::All);
	ProcessingPhase = EMassExtendedProcessingPhase::StartPhysics;
	ExecutionOrder.ExecuteInGroup = Recall::Movement::ProcessorGroupNames::StartPhysics::Update;
	ExecutionOrder.ExecuteAfter.Add(Recall::Physics::ProcessorGroupNames::Initialize);
}

void URecallMovementProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallMovementProcessor::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager)
{
	FMassExtendedTagBitSet InvalidTags;

	// Block movement while traversing a nav link.
	InvalidTags.Add(*FRecallNavLinkTraversalTag::StaticStruct());
	
	EntityQuery.AddRequirement<FRecallPhysicsBodyFragment>(EMassExtendedFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FRecallMovementFragment>(EMassExtendedFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FRecallGameplayTagFragment>(EMassExtendedFragmentAccess::ReadWrite, EMassExtendedFragmentPresence::Optional);
	EntityQuery.AddRequirement<FRecallAttributeFragment>(EMassExtendedFragmentAccess::ReadOnly, EMassExtendedFragmentPresence::Optional);	
	EntityQuery.AddRequirement<FRecallControllerFragment>(EMassExtendedFragmentAccess::ReadOnly, EMassExtendedFragmentPresence::Optional);
	EntityQuery.AddRequirement<FRecallPhysicsCharacterFragment>(EMassExtendedFragmentAccess::ReadOnly, EMassExtendedFragmentPresence::Optional);
	EntityQuery.AddTagRequirements<EMassExtendedFragmentPresence::None>(InvalidTags);
	EntityQuery.AddSubsystemRequirement<URecallPhysicsSubsystem>(EMassExtendedFragmentAccess::ReadWrite);
	EntityQuery.AddConstSharedRequirement<FRecallMovementSharedFragment>();
}

void URecallMovementProcessor::Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_Movement_Execute);

	EntityQuery.ForEachEntityChunk(Context, [](FMassExtendedExecutionContext& Context)
	{
		URecallPhysicsSubsystem& PhysicsSystem = Context.GetMutableSubsystemChecked<URecallPhysicsSubsystem>();

		const FRecallMovementSharedFragment& MovementConstSharedFragment = Context.GetConstSharedFragment<FRecallMovementSharedFragment>();

		const TConstArrayView<FRecallPhysicsBodyFragment> BodyList = Context.GetFragmentView<FRecallPhysicsBodyFragment>();
		
		const TArrayView<FRecallMovementFragment> MovementList = Context.GetMutableFragmentView<FRecallMovementFragment>();
		const TArrayView<FRecallGameplayTagFragment> GameplayTagsList = Context.GetMutableFragmentView<FRecallGameplayTagFragment>();
		const TConstArrayView<FRecallAttributeFragment> AttributeList = Context.GetFragmentView<FRecallAttributeFragment>();
		const TConstArrayView<FRecallControllerFragment> ControllerList = Context.GetFragmentView<FRecallControllerFragment>();
		const TConstArrayView<FRecallPhysicsCharacterFragment> CharacterList = Context.GetFragmentView<FRecallPhysicsCharacterFragment>();

		ParallelFor(Context.GetNumEntities(), [&](int32 EntityIndex)
		{
			const FRecallPhysicsBodyFragment& BodyFragment = BodyList[EntityIndex];
			const TWeakPtr<FRecallPhysicsBody> PhysicsBody = PhysicsSystem.GetMutableBody(BodyFragment.BodyHandle);
			if (!ensure(PhysicsBody.IsValid()) || !PhysicsBody.Pin()->IsEnabled())
			{
				return;
			}
		
			FRecallMovementFragment& MovementFragment = MovementList[EntityIndex];
			
			FRecallGameplayTagFragment* const GameplayTagsFragmentPtr = GameplayTagsList.IsValidIndex(EntityIndex) ? &GameplayTagsList[EntityIndex] : nullptr;
			const FRecallAttributeFragment* const AttributeFragmentPtr = AttributeList.IsValidIndex(EntityIndex) ? &AttributeList[EntityIndex] : nullptr;
			const FRecallControllerFragment* const ControllerFragmentPtr = ControllerList.IsValidIndex(EntityIndex) ? &ControllerList[EntityIndex] : nullptr;
			const FRecallPhysicsCharacterFragment* const CharacterFragmentPtr = CharacterList.IsValidIndex(EntityIndex) ? &CharacterList[EntityIndex] : nullptr;

			const FRecallMovementContext MovementContext = { Context,	
				MovementFragment, MovementConstSharedFragment, PhysicsBody,
				CharacterFragmentPtr, GameplayTagsFragmentPtr, AttributeFragmentPtr, ControllerFragmentPtr };
			
			Recall::Movement::Utils::HandleMovement(MovementContext);
			Recall::Movement::Utils::HandleJump(MovementContext);
		});

#if RECALL_DESYNC_LOG
		const FMassExtendedEntityManager& EntityManager = Context.GetEntityManagerChecked();
		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			const FMassExtendedEntityHandle Entity = Context.GetEntity(EntityIndex);
			const FMassExtendedArchetypeHandle ArchetypeHandle = EntityManager.GetArchetypeForEntity(Entity);
			int32 AbsoluteIndex, ChunkIndex;
			EntityManager.GetArchetypeInternalIndexForEntity(Entity, ArchetypeHandle, AbsoluteIndex, ChunkIndex);
			const FRecallMovementFragment& MovementFragment = MovementList[EntityIndex];
			RECALL_DESYNC_LOG_STR(Context.GetWorld(), Movement,
				FString::Printf(TEXT("%s, MovementDirection: %s, AbsoluteIndex: %d, ChunkIndex: %d"),
				*Entity.DebugGetDescription(), *MovementFragment.MovementDirection.ToString(), AbsoluteIndex, ChunkIndex));
		}
#endif // RECALL_DESYNC_LOG
	});
}
