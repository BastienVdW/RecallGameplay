// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallNavigationObstacleProcessors.h"

#include "MassExtendedExecutionContext.h"
#include "Simulation/Navigation/RecallNavigationObstacleFragments.h"
#include "Simulation/Navigation/RecallPathFollowingFragments.h"
#include "Simulation/StateTree/RecallStateTreeProcessorGroupTypes.h"
#include "Simulation/Transform/RecallTransformFragments.h"
#include "System/Obstacle/RecallNavigationObstacleSubsystem.h"

//----------------------------------------------------------------------//
// URecallNavigationObstacleInitializeProcessor
//----------------------------------------------------------------------//
URecallNavigationObstacleInitializeProcessor::URecallNavigationObstacleInitializeProcessor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EExtendedProcessorExecutionFlags::All);
	ProcessingPhase = EMassExtendedProcessingPhase::PrePhysics;
	ExecutionOrder.ExecuteBefore.Add(Recall::StateTree::ProcessorGroupNames::StateTreeUpdate);
}

void URecallNavigationObstacleInitializeProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallNavigationObstacleInitializeProcessor::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallTransformFragment>(EMassExtendedFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FRecallPathFollowerFragment>(EMassExtendedFragmentAccess::ReadOnly, EMassExtendedFragmentPresence::Optional);
	EntityQuery.AddRequirement<FRecallNavigationObstacleFragment>(EMassExtendedFragmentAccess::ReadOnly);
	EntityQuery.AddSubsystemRequirement<URecallNavigationObstacleSubsystem>(EMassExtendedFragmentAccess::ReadWrite);
}

void URecallNavigationObstacleInitializeProcessor::Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_NavigationObstacleInitialize_Execute);

	EntityQuery.ForEachEntityChunk(Context, [](FMassExtendedExecutionContext& Context)
	{
		URecallNavigationObstacleSubsystem& NavigationObstacleSystem = Context.GetMutableSubsystemChecked<URecallNavigationObstacleSubsystem>();

		const TConstArrayView<FRecallNavigationObstacleFragment> ObstacleList = Context.GetFragmentView<FRecallNavigationObstacleFragment>();
		const TConstArrayView<FRecallPathFollowerFragment> PathFollowerList = Context.GetFragmentView<FRecallPathFollowerFragment>();
		const TConstArrayView<FRecallTransformFragment> TransformList = Context.GetFragmentView<FRecallTransformFragment>();

		FRecallNavigationObstacleHashGrid2D& HashGrid = NavigationObstacleSystem.GetObstacleGridMutable();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			const FRecallTransformFragment& TransformFragment = TransformList[EntityIndex];
			const FRecallPathFollowerFragment* const PathFollowerPtr = PathFollowerList.IsValidIndex(EntityIndex) ? &PathFollowerList[EntityIndex] : nullptr;
			const FRecallNavigationObstacleFragment& ObstacleFragment = ObstacleList[EntityIndex];

			const float Radius = ObstacleFragment.Radius;
			FVector NewPos = TransformFragment.Position;

			if (PathFollowerPtr != nullptr && PathFollowerPtr->IsPathRequested())
			{
				NewPos = PathFollowerPtr->GetDestination();
			}
			
			FRecallNavigationObstacleItem ObstacleItem;
			ObstacleItem.Entity = Context.GetEntity(EntityIndex);
			ObstacleItem.ItemFlags |= ERecallNavigationObstacleFlags::HasColliderData;
			ObstacleItem.Location = NewPos;			
			
			const FBox NewBounds(NewPos - FVector(Radius, Radius, 0.f), NewPos + FVector(Radius, Radius, 0.f));
			HashGrid.Add(ObstacleItem, NewBounds);
		}
	});
}

//----------------------------------------------------------------------//
// URecallNavigationObstacleResetProcessor
//----------------------------------------------------------------------//
URecallNavigationObstacleResetProcessor::URecallNavigationObstacleResetProcessor()
{
	ExecutionFlags = static_cast<int32>(EExtendedProcessorExecutionFlags::All);
	ProcessingPhase = EMassExtendedProcessingPhase::FrameEnd;
}

void URecallNavigationObstacleResetProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

bool URecallNavigationObstacleResetProcessor::ShouldAllowQueryBasedPruning(const bool bRuntimeMode /*= true*/) const
{
	return false;
}

void URecallNavigationObstacleResetProcessor::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager)
{
	ProcessorRequirements.AddSubsystemRequirement<URecallNavigationObstacleSubsystem>(EMassExtendedFragmentAccess::ReadWrite);
}

void URecallNavigationObstacleResetProcessor::Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_NavigationObstacleReset_Execute);

	// Prevent the grid from persisting between frames, and we do not want to have to rollback the whole grid.
	URecallNavigationObstacleSubsystem& NavigationObstacleSystem = Context.GetMutableSubsystemChecked<URecallNavigationObstacleSubsystem>();
	NavigationObstacleSystem.GetObstacleGridMutable().Reset();
}
