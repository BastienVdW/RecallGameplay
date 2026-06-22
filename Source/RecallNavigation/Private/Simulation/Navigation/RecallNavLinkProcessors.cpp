// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallNavLinkProcessors.h"

#include "MassExecutionContext.h"
#include "NavigationSystem.h"
#include "Navigation/RecallNavLinkProxy.h"
#include "Navigation/RecallNavLinkTraversal.h"
#include "Physics/RecallPhysicsObjects.h"
#include "Simulation/Movement/RecallMovementFragments.h"
#include "Simulation/Navigation/RecallNavLinkFragments.h"
#include "Simulation/Physics/RecallPhysicsBodyFragment.h"
#include "Simulation/Transform/RecallTransformFragments.h"
#include "System/Physics/RecallPhysicsSubsystem.h"

static const FRecallNavLinkTraversalBase* GetNavLinkTraversal(const UWorld* World, FNavLinkId CustomNavLinkId)
{
	const UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
	if (ensure(IsValid(NavSys)))
	{
		if (const INavLinkCustomInterface* MoveSegmentCustomLink = NavSys->GetCustomLink(CustomNavLinkId))
		{
			const URecallNavLinkProxy* NavLinkProxy = Cast<URecallNavLinkProxy>(MoveSegmentCustomLink);
			if (IsValid(NavLinkProxy))
			{
				return NavLinkProxy->NavLinkTraversal.GetPtr<FRecallNavLinkTraversalBase>();
			}
		}
	}

	return nullptr;
}

//----------------------------------------------------------------------//
// URecallNavLinkConstructor
//----------------------------------------------------------------------//
URecallNavLinkConstructor::URecallNavLinkConstructor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::All);
	ObservedTypes.Add(FRecallNavLinkTraversalTag::StaticStruct());
	ObservedOperations = EMassObservedOperationFlags::Add;
}

void URecallNavLinkConstructor::InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallNavLinkConstructor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) 
{
	EntityQuery.AddRequirement<FRecallNavLinkTraversalFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FRecallPhysicsBodyFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FRecallTransformFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddSubsystemRequirement<URecallPhysicsSubsystem>(EMassFragmentAccess::ReadWrite);
}

void URecallNavLinkConstructor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_NavLink_Constructor);

	EntityQuery.ForEachEntityChunk(Context, [](FMassExecutionContext& Context)
	{
		URecallPhysicsSubsystem& PhysicsSystem = Context.GetMutableSubsystemChecked<URecallPhysicsSubsystem>();
		
		const TConstArrayView<FRecallPhysicsBodyFragment> BodyList = Context.GetFragmentView<FRecallPhysicsBodyFragment>();
		const TConstArrayView<FRecallTransformFragment> TransformList = Context.GetFragmentView<FRecallTransformFragment>();
		const TArrayView<FRecallNavLinkTraversalFragment> NavLinkTraversalList = Context.GetMutableFragmentView<FRecallNavLinkTraversalFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{			
			FRecallNavLinkTraversalFragment& NavLinkTraversalFragment = NavLinkTraversalList[EntityIndex];
			NavLinkTraversalFragment.TraversalDuration = 0;

			const FRecallNavLinkTraversalBase* NavLinkTraversal = GetNavLinkTraversal(Context.GetWorld(), FNavLinkId(NavLinkTraversalFragment.CustomNavLinkId));
			if (NavLinkTraversal == nullptr)
			{
				continue;
			}
			
			const FMassEntityHandle Entity = Context.GetEntity(EntityIndex);
			const FRecallTransformFragment& TransformFragment = TransformList[EntityIndex];
			const FRecallPhysicsBodyFragment& BodyFragment = BodyList[EntityIndex];
			const TWeakPtr<FRecallPhysicsBody> Body = PhysicsSystem.GetMutableBody(BodyFragment.BodyHandle);
			
			const FRecallNavLinkTraversalContext TraversalContext{ Context, Entity,
				TransformFragment, NavLinkTraversalFragment, Body };

			NavLinkTraversal->OnEnter(TraversalContext);
		}
	});
}

//----------------------------------------------------------------------//
// URecallNavLinkTraversalProcessor
//----------------------------------------------------------------------//
URecallNavLinkTraversalProcessor::URecallNavLinkTraversalProcessor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::All);
	ProcessingPhase = EMassProcessingPhase::StartPhysics;
}

void URecallNavLinkTraversalProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallNavLinkTraversalProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) 
{
	FMassTagBitSet RequiredTags;
	RequiredTags.Add(FRecallNavLinkTraversalTag::StaticStruct());
	
	EntityQuery.AddRequirement<FRecallNavLinkTraversalFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FRecallPhysicsBodyFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FRecallTransformFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddTagRequirements<EMassFragmentPresence::All>(RequiredTags);
	EntityQuery.AddSubsystemRequirement<URecallPhysicsSubsystem>(EMassFragmentAccess::ReadWrite);
}

void URecallNavLinkTraversalProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_NavLinkTraversal_Execute);

	EntityQuery.ForEachEntityChunk(Context, [](FMassExecutionContext& Context)
	{
		URecallPhysicsSubsystem& PhysicsSystem = Context.GetMutableSubsystemChecked<URecallPhysicsSubsystem>();
		
		const TConstArrayView<FRecallPhysicsBodyFragment> BodyList = Context.GetFragmentView<FRecallPhysicsBodyFragment>();
		const TConstArrayView<FRecallTransformFragment> TransformList = Context.GetFragmentView<FRecallTransformFragment>();
		const TArrayView<FRecallNavLinkTraversalFragment> NavLinkTraversalList = Context.GetMutableFragmentView<FRecallNavLinkTraversalFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			FRecallNavLinkTraversalFragment& NavLinkTraversalFragment = NavLinkTraversalList[EntityIndex];
			NavLinkTraversalFragment.TraversalDuration++;

			const FRecallNavLinkTraversalBase* NavLinkTraversal = GetNavLinkTraversal(Context.GetWorld(), FNavLinkId(NavLinkTraversalFragment.CustomNavLinkId));
			if (NavLinkTraversal == nullptr)
			{
				continue;
			}
			
			const FMassEntityHandle Entity = Context.GetEntity(EntityIndex);
			const FRecallTransformFragment& TransformFragment = TransformList[EntityIndex];
			const FRecallPhysicsBodyFragment& BodyFragment = BodyList[EntityIndex];
			const TWeakPtr<FRecallPhysicsBody> Body = PhysicsSystem.GetMutableBody(BodyFragment.BodyHandle);

			const FRecallNavLinkTraversalContext TraversalContext{ Context, Entity,
				TransformFragment, NavLinkTraversalFragment, Body };

			if (!NavLinkTraversal->OnTick(TraversalContext))
			{
				NavLinkTraversal->OnExit(TraversalContext);
				Context.Defer().RemoveTag<FRecallNavLinkTraversalTag>(Entity);
			}
		}
	});
}
