// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallCrowdProcessors.h"

#include "Crowd/RecallCrowdAgentBlackboard.h"
#include "Crowd/RecallCrowdAgentTypes.h"
#include "Desync/RecallDesyncLog.h"
#include "MassExtendedExecutionContext.h"
#include "Physics/RecallPhysicsObjects.h"
#include "Simulation/Crowd/RecallCrowdFragments.h"
#include "Simulation/Movement/RecallMovementFragments.h"
#include "Simulation/Movement/RecallMovementProcessorGroupTypes.h"
#include "Simulation/Navigation/RecallPathFollowingFragments.h"
#include "Simulation/Physics/RecallPhysicsBodyFragment.h"
#include "Simulation/Physics/RecallPhysicsProcessorGroupTypes.h"
#include "Simulation/Transform/RecallTransformFragments.h"
#include "System/Crowd/RecallCrowdSubsystem.h"
#include "System/Physics/RecallPhysicsSubsystem.h"
#include "Utility/Math/RecallMathUtils.h"

//----------------------------------------------------------------------//
// URecallCrowdAgentConstructor
//----------------------------------------------------------------------//
URecallCrowdAgentConstructor::URecallCrowdAgentConstructor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EExtendedProcessorExecutionFlags::All);
	ObservedType = FRecallCrowdAgentFragment::StaticStruct();
	Operation = EMassExtendedObservedOperation::Add;
}

void URecallCrowdAgentConstructor::InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallCrowdAgentConstructor::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallCrowdAgentFragment>(EMassExtendedFragmentAccess::ReadWrite);
	EntityQuery.AddSubsystemRequirement<URecallCrowdSubsystem>(EMassExtendedFragmentAccess::ReadWrite);
}

void URecallCrowdAgentConstructor::Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_CrowdAgent_Constructor);

	EntityQuery.ForEachEntityChunk(Context, [](FMassExtendedExecutionContext& Context)
	{
		URecallCrowdSubsystem& CrowdSystem = Context.GetMutableSubsystemChecked<URecallCrowdSubsystem>();

		const TArrayView<FRecallCrowdAgentFragment> CrowdList = Context.GetMutableFragmentView<FRecallCrowdAgentFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			FRecallCrowdAgentFragment& CrowdFragment = CrowdList[EntityIndex];
			CrowdFragment.CrowdAgentHandle = CrowdSystem.RegisterCrowdAgent();
		}
	});
}

//----------------------------------------------------------------------//
// URecallCrowdAgentDestructor
//----------------------------------------------------------------------//
URecallCrowdAgentDestructor::URecallCrowdAgentDestructor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EExtendedProcessorExecutionFlags::All);
	ObservedType = FRecallCrowdAgentFragment::StaticStruct();
	Operation = EMassExtendedObservedOperation::Remove;
}

void URecallCrowdAgentDestructor::InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallCrowdAgentDestructor::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallCrowdAgentFragment>(EMassExtendedFragmentAccess::ReadWrite);
	EntityQuery.AddSubsystemRequirement<URecallCrowdSubsystem>(EMassExtendedFragmentAccess::ReadWrite);
}

void URecallCrowdAgentDestructor::Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_CrowdAgent_Destructor);

	EntityQuery.ForEachEntityChunk(Context, [](FMassExtendedExecutionContext& Context)
	{
		URecallCrowdSubsystem& CrowdSystem = Context.GetMutableSubsystemChecked<URecallCrowdSubsystem>();

		const TArrayView<FRecallCrowdAgentFragment> CrowdList = Context.GetMutableFragmentView<FRecallCrowdAgentFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			FRecallCrowdAgentFragment& CrowdFragment = CrowdList[EntityIndex];
			CrowdSystem.ReleaseCrowdAgent(CrowdFragment.CrowdAgentHandle);
		}
	});
}

//----------------------------------------------------------------------//
// URecallCrowdAgentNavLinkTraversalConstructor
//----------------------------------------------------------------------//
URecallCrowdAgentNavLinkTraversalConstructor::URecallCrowdAgentNavLinkTraversalConstructor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EExtendedProcessorExecutionFlags::All);
	ObservedType = FRecallNavLinkTraversalTag::StaticStruct();
	Operation = EMassExtendedObservedOperation::Add;
}

void URecallCrowdAgentNavLinkTraversalConstructor::InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallCrowdAgentNavLinkTraversalConstructor::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallCrowdAgentFragment>(EMassExtendedFragmentAccess::ReadWrite);
	EntityQuery.AddSubsystemRequirement<URecallCrowdSubsystem>(EMassExtendedFragmentAccess::ReadWrite);
}

void URecallCrowdAgentNavLinkTraversalConstructor::Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_CrowdAgentNavLinkTraversal_Constructor);

	EntityQuery.ForEachEntityChunk(Context, [](FMassExtendedExecutionContext& Context)
	{
		URecallCrowdSubsystem& CrowdSystem = Context.GetMutableSubsystemChecked<URecallCrowdSubsystem>();

		const TArrayView<FRecallCrowdAgentFragment> CrowdList = Context.GetMutableFragmentView<FRecallCrowdAgentFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			FRecallCrowdAgentFragment& CrowdFragment = CrowdList[EntityIndex];
			CrowdSystem.SetCrowdAgentSimulationState(CrowdFragment.CrowdAgentHandle, ERecallCrowdSimulationState::Disabled);
		}
	});
}

//----------------------------------------------------------------------//
// URecallCrowdAgentNavLinkTraversalDestructor
//----------------------------------------------------------------------//
URecallCrowdAgentNavLinkTraversalDestructor::URecallCrowdAgentNavLinkTraversalDestructor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EExtendedProcessorExecutionFlags::All);
	ObservedType = FRecallNavLinkTraversalTag::StaticStruct();
	Operation = EMassExtendedObservedOperation::Remove;
}

void URecallCrowdAgentNavLinkTraversalDestructor::InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallCrowdAgentNavLinkTraversalDestructor::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallCrowdAgentFragment>(EMassExtendedFragmentAccess::ReadWrite);
	EntityQuery.AddSubsystemRequirement<URecallCrowdSubsystem>(EMassExtendedFragmentAccess::ReadWrite);
}

void URecallCrowdAgentNavLinkTraversalDestructor::Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_CrowdAgentNavLinkTraversal_Destructor);

	EntityQuery.ForEachEntityChunk(Context, [](FMassExtendedExecutionContext& Context)
	{
		URecallCrowdSubsystem& CrowdSystem = Context.GetMutableSubsystemChecked<URecallCrowdSubsystem>();

		const TArrayView<FRecallCrowdAgentFragment> CrowdList = Context.GetMutableFragmentView<FRecallCrowdAgentFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			FRecallCrowdAgentFragment& CrowdFragment = CrowdList[EntityIndex];
			CrowdSystem.SetCrowdAgentSimulationState(CrowdFragment.CrowdAgentHandle, ERecallCrowdSimulationState::Enabled);
		}
	});
}

//----------------------------------------------------------------------//
// URecallCrowdAgentSetBlackboardProcessor
//----------------------------------------------------------------------//
URecallCrowdAgentSetBlackboardProcessor::URecallCrowdAgentSetBlackboardProcessor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EExtendedProcessorExecutionFlags::All);
	ProcessingPhase = EMassExtendedProcessingPhase::StartPhysics;
	ExecutionOrder.ExecuteAfter.Add(Recall::Movement::ProcessorGroupNames::StartPhysics::Update);
}

void URecallCrowdAgentSetBlackboardProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallCrowdAgentSetBlackboardProcessor::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallTransformFragment>(EMassExtendedFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FRecallCrowdAgentFragment>(EMassExtendedFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FRecallMovementFragment>(EMassExtendedFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FRecallPhysicsBodyFragment>(EMassExtendedFragmentAccess::ReadOnly);
	EntityQuery.AddConstSharedRequirement<FRecallPathFollowingConstSharedFragment>();
	EntityQuery.AddConstSharedRequirement<FRecallCrowdAgentCollisionConstSharedFragment>();
	EntityQuery.AddConstSharedRequirement<FRecallMovementSharedFragment>();
	EntityQuery.AddSubsystemRequirement<URecallCrowdSubsystem>(EMassExtendedFragmentAccess::ReadWrite);
	EntityQuery.AddSubsystemRequirement<URecallPhysicsSubsystem>(EMassExtendedFragmentAccess::ReadOnly);
}

void URecallCrowdAgentSetBlackboardProcessor::Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_CrowdAgentSetBlackboard_Execute);

	EntityQuery.ParallelForEachEntityChunk(Context, [](FMassExtendedExecutionContext& Context)
	{
		const URecallPhysicsSubsystem& PhysicsSystem = Context.GetSubsystemChecked<URecallPhysicsSubsystem>();
		URecallCrowdSubsystem& CrowdSystem = Context.GetMutableSubsystemChecked<URecallCrowdSubsystem>();

		const FRecallPathFollowingConstSharedFragment& PathFollowingFragment = Context.GetConstSharedFragment<FRecallPathFollowingConstSharedFragment>();
		const FRecallCrowdAgentCollisionConstSharedFragment& CrowdAgentCollisionFragment = Context.GetConstSharedFragment<FRecallCrowdAgentCollisionConstSharedFragment>();
		const FRecallMovementSharedFragment& MovementConstSharedFragment = Context.GetConstSharedFragment<FRecallMovementSharedFragment>();

		const TConstArrayView<FRecallTransformFragment> TransformList = Context.GetFragmentView<FRecallTransformFragment>();
		const TConstArrayView<FRecallPhysicsBodyFragment> BodyList = Context.GetFragmentView<FRecallPhysicsBodyFragment>();
		const TConstArrayView<FRecallCrowdAgentFragment> CrowdAgentList = Context.GetFragmentView<FRecallCrowdAgentFragment>();
		const TConstArrayView<FRecallMovementFragment> MovementList = Context.GetFragmentView<FRecallMovementFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			const FRecallTransformFragment& TransformFragment = TransformList[EntityIndex];
			const FRecallPhysicsBodyFragment& BodyFragment = BodyList[EntityIndex];
			const FRecallMovementFragment& MovementFragment = MovementList[EntityIndex];
			const FRecallCrowdAgentFragment& CrowdAgentFragment = CrowdAgentList[EntityIndex];

			FRecallCrowdAgentBlackboard& AgentBlackboard = CrowdSystem.GetMutableAgentBlackboardChecked(CrowdAgentFragment.CrowdAgentHandle);

			// Use crowd agent settings (common to all navigation systems)
			AgentBlackboard.CylinderHalfHeight = CrowdAgentCollisionFragment.Settings.AgentHeight * 0.5f;
			AgentBlackboard.CylinderRadius = PathFollowingFragment.Settings.Radius;

			AgentBlackboard.MaxSpeedCentimetersPerSecond = MovementConstSharedFragment.MovementSettings.GetSpeedCentimetersPerSecond();
			
			AgentBlackboard.Location = TransformFragment.Position;
			AgentBlackboard.bDirtyLocation = false;

			const TWeakPtr<const FRecallPhysicsBody> Body = PhysicsSystem.GetBody(BodyFragment.BodyHandle);
			if (ensure(Body.IsValid()))
			{
				AgentBlackboard.VelocityCentimetersPerSecond = Recall::Math::Utils::UnitsPerFrameToPerSecond(
					Body.Pin()->GetLinearVelocity());
				AgentBlackboard.bDirtyVelocity = false;
			}

#if RECALL_DESYNC_LOG
				RECALL_DESYNC_LOG_VEC(Context.GetWorld(), GetAgentBlackboardLocation, AgentBlackboard.Location);
				RECALL_DESYNC_LOG_VEC(Context.GetWorld(), GetAgentBlackboardVelocity, AgentBlackboard.VelocityCentimetersPerSecond);
#endif // RECALL_DESYNC_LOG
			
			const FVector MoveDirection(MovementFragment.MovementDirection.X, MovementFragment.MovementDirection.Y, 0.0);
			CrowdSystem.SetAgentMoveDirection(CrowdAgentFragment.CrowdAgentHandle, MoveDirection);
		}
	});
}

//----------------------------------------------------------------------//
// URecallCrowdAgentGetBlackboardProcessor
//----------------------------------------------------------------------//
URecallCrowdAgentGetBlackboardProcessor::URecallCrowdAgentGetBlackboardProcessor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EExtendedProcessorExecutionFlags::All);
	ProcessingPhase = EMassExtendedProcessingPhase::EndPhysics;
	ExecutionOrder.ExecuteBefore.Add(Recall::Physics::ProcessorGroupNames::CopyLocation);
}

void URecallCrowdAgentGetBlackboardProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallCrowdAgentGetBlackboardProcessor::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallCrowdAgentFragment>(EMassExtendedFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FRecallPhysicsBodyFragment>(EMassExtendedFragmentAccess::ReadOnly);
	EntityQuery.AddSubsystemRequirement<URecallCrowdSubsystem>(EMassExtendedFragmentAccess::ReadOnly);
	EntityQuery.AddSubsystemRequirement<URecallPhysicsSubsystem>(EMassExtendedFragmentAccess::ReadWrite);
}

void URecallCrowdAgentGetBlackboardProcessor::Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_CrowdAgentGetBlackboard_Execute);
	
	EntityQuery.ParallelForEachEntityChunk(Context, [](FMassExtendedExecutionContext& Context)
	{
		const URecallCrowdSubsystem& CrowdSystem = Context.GetSubsystemChecked<URecallCrowdSubsystem>();
		URecallPhysicsSubsystem& PhysicsSystem = Context.GetMutableSubsystemChecked<URecallPhysicsSubsystem>();
		
		const TConstArrayView<FRecallCrowdAgentFragment> CrowdAgentList = Context.GetFragmentView<FRecallCrowdAgentFragment>();
		const TConstArrayView<FRecallPhysicsBodyFragment> BodyList = Context.GetFragmentView<FRecallPhysicsBodyFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			const FRecallCrowdAgentFragment& CrowdAgentFragment = CrowdAgentList[EntityIndex];

			const FRecallCrowdAgentBlackboard& AgentBlackboard = CrowdSystem.GetAgentBlackboardChecked(CrowdAgentFragment.CrowdAgentHandle);
			if (!AgentBlackboard.HasChanged())
			{
				continue;
			}

			const FRecallPhysicsBodyFragment& BodyFragment = BodyList[EntityIndex];
			const TWeakPtr<FRecallPhysicsBody> Body = PhysicsSystem.GetMutableBody(BodyFragment.BodyHandle);
			if (!ensure(Body.IsValid()))
			{
				continue;
			}

			if (AgentBlackboard.HasPositionChanged())
			{
#if RECALL_DESYNC_LOG
				RECALL_DESYNC_LOG_VEC(Context.GetWorld(), GetAgentBlackboardLocation, AgentBlackboard.Location);
#endif // RECALL_DESYNC_LOG
				Body.Pin()->SetPosition(AgentBlackboard.Location);
			}

			if (AgentBlackboard.HasVelocityChanged())
			{
#if RECALL_DESYNC_LOG
				RECALL_DESYNC_LOG_VEC(Context.GetWorld(), GetAgentBlackboardVelocity, AgentBlackboard.VelocityCentimetersPerSecond);
#endif // RECALL_DESYNC_LOG
				Body.Pin()->SetLinearVelocity(Recall::Math::Utils::UnitsPerSecondToPerFrame(
					AgentBlackboard.VelocityCentimetersPerSecond));
			}
		}
	});
}

//----------------------------------------------------------------------//
// URecallCrowdUpdateProcessor
//----------------------------------------------------------------------//
URecallCrowdUpdateProcessor::URecallCrowdUpdateProcessor()
{
	ExecutionFlags = static_cast<int32>(EExtendedProcessorExecutionFlags::All);
	ProcessingPhase = EMassExtendedProcessingPhase::DuringPhysics;
}

void URecallCrowdUpdateProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

bool URecallCrowdUpdateProcessor::ShouldAllowQueryBasedPruning(const bool bRuntimeMode) const
{
	return false;
}

void URecallCrowdUpdateProcessor::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager)
{
	ProcessorRequirements.AddSubsystemRequirement<URecallCrowdSubsystem>(EMassExtendedFragmentAccess::ReadWrite);
}

void URecallCrowdUpdateProcessor::Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_CrowdUpdate_Execute);

	URecallCrowdSubsystem& CrowdSystem = Context.GetMutableSubsystemChecked<URecallCrowdSubsystem>();
	CrowdSystem.TickCrowdManager(Context.GetDeltaTimeSeconds());
}
