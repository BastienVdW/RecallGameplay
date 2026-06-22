// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallNavLinkProcessors.h"

#include "MassExtendedExecutionContext.h"
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
	ExecutionFlags = static_cast<int32>(EExtendedProcessorExecutionFlags::All);
	ObservedType = FRecallNavLinkTraversalTag::StaticStruct();
	Operation = EMassExtendedObservedOperation::Add;
}

void URecallNavLinkConstructor::InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallNavLinkConstructor::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager) 
{
	EntityQuery.AddRequirement<FRecallNavLinkTraversalFragment>(EMassExtendedFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FRecallPhysicsBodyFragment>(EMassExtendedFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FRecallTransformFragment>(EMassExtendedFragmentAccess::ReadOnly);
	EntityQuery.AddSubsystemRequirement<URecallPhysicsSubsystem>(EMassExtendedFragmentAccess::ReadWrite);
}

void URecallNavLinkConstructor::Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_NavLink_Constructor);

	EntityQuery.ForEachEntityChunk(Context, [](FMassExtendedExecutionContext& Context)
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
			
			const FMassExtendedEntityHandle Entity = Context.GetEntity(EntityIndex);
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
	ExecutionFlags = static_cast<int32>(EExtendedProcessorExecutionFlags::All);
	ProcessingPhase = EMassExtendedProcessingPhase::StartPhysics;
}

void URecallNavLinkTraversalProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallNavLinkTraversalProcessor::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager) 
{
	FMassExtendedTagBitSet RequiredTags;
	RequiredTags.Add(*FRecallNavLinkTraversalTag::StaticStruct());
	
	EntityQuery.AddRequirement<FRecallNavLinkTraversalFragment>(EMassExtendedFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FRecallPhysicsBodyFragment>(EMassExtendedFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FRecallTransformFragment>(EMassExtendedFragmentAccess::ReadOnly);
	EntityQuery.AddTagRequirements<EMassExtendedFragmentPresence::All>(RequiredTags);
	EntityQuery.AddSubsystemRequirement<URecallPhysicsSubsystem>(EMassExtendedFragmentAccess::ReadWrite);
}

void URecallNavLinkTraversalProcessor::Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_NavLinkTraversal_Execute);

	EntityQuery.ForEachEntityChunk(Context, [](FMassExtendedExecutionContext& Context)
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
			
			const FMassExtendedEntityHandle Entity = Context.GetEntity(EntityIndex);
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
