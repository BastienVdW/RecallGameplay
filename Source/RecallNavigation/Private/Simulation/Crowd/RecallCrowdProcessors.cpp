// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallCrowdProcessors.h"

#include "Crowd/RecallCrowdAgentBlackboard.h"
#include "Crowd/RecallCrowdAgentTypes.h"
#include "Desync/RecallDesyncLog.h"
#include "MassExecutionContext.h"
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
	ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::All);
	ObservedTypes.Add(FRecallCrowdAgentFragment::StaticStruct());
	ObservedOperations = EMassObservedOperationFlags::Add;
}

void URecallCrowdAgentConstructor::InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallCrowdAgentConstructor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallCrowdAgentFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddSubsystemRequirement<URecallCrowdSubsystem>(EMassFragmentAccess::ReadWrite);
}

void URecallCrowdAgentConstructor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_CrowdAgent_Constructor);

	EntityQuery.ForEachEntityChunk(Context, [](FMassExecutionContext& Context)
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
	ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::All);
	ObservedTypes.Add(FRecallCrowdAgentFragment::StaticStruct());
	ObservedOperations = EMassObservedOperationFlags::Remove;
}

void URecallCrowdAgentDestructor::InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallCrowdAgentDestructor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallCrowdAgentFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddSubsystemRequirement<URecallCrowdSubsystem>(EMassFragmentAccess::ReadWrite);
}

void URecallCrowdAgentDestructor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_CrowdAgent_Destructor);

	EntityQuery.ForEachEntityChunk(Context, [](FMassExecutionContext& Context)
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
	ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::All);
	ObservedTypes.Add(FRecallNavLinkTraversalTag::StaticStruct());
	ObservedOperations = EMassObservedOperationFlags::Add;
}

void URecallCrowdAgentNavLinkTraversalConstructor::InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallCrowdAgentNavLinkTraversalConstructor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallCrowdAgentFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddSubsystemRequirement<URecallCrowdSubsystem>(EMassFragmentAccess::ReadWrite);
}

void URecallCrowdAgentNavLinkTraversalConstructor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_CrowdAgentNavLinkTraversal_Constructor);

	EntityQuery.ForEachEntityChunk(Context, [](FMassExecutionContext& Context)
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
	ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::All);
	ObservedTypes.Add(FRecallNavLinkTraversalTag::StaticStruct());
	ObservedOperations = EMassObservedOperationFlags::Remove;
}

void URecallCrowdAgentNavLinkTraversalDestructor::InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallCrowdAgentNavLinkTraversalDestructor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallCrowdAgentFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddSubsystemRequirement<URecallCrowdSubsystem>(EMassFragmentAccess::ReadWrite);
}

void URecallCrowdAgentNavLinkTraversalDestructor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_CrowdAgentNavLinkTraversal_Destructor);

	EntityQuery.ForEachEntityChunk(Context, [](FMassExecutionContext& Context)
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
	ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::All);
	ProcessingPhase = EMassProcessingPhase::StartPhysics;
	ExecutionOrder.ExecuteAfter.Add(Recall::Movement::ProcessorGroupNames::StartPhysics::Update);
	ExecutionOrder.ExecuteBefore.Add(Recall::Physics::ProcessorGroupNames::StartSimulation);
}

void URecallCrowdAgentSetBlackboardProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallCrowdAgentSetBlackboardProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallTransformFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FRecallCrowdAgentFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FRecallMovementFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FRecallPhysicsBodyFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddConstSharedRequirement<FRecallPathFollowingConstSharedFragment>();
	EntityQuery.AddConstSharedRequirement<FRecallCrowdAgentCollisionConstSharedFragment>();
	EntityQuery.AddConstSharedRequirement<FRecallMovementSharedFragment>();
	EntityQuery.AddSubsystemRequirement<URecallCrowdSubsystem>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddSubsystemRequirement<URecallPhysicsSubsystem>(EMassFragmentAccess::ReadOnly);
}

void URecallCrowdAgentSetBlackboardProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_CrowdAgentSetBlackboard_Execute);

	EntityQuery.ParallelForEachEntityChunk(Context, [](FMassExecutionContext& Context)
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

			const FConstRecallPhysicsBodyView Body = PhysicsSystem.GetBody(BodyFragment.BodyHandle);
			if (ensure(Body.IsValid()))
			{
				AgentBlackboard.VelocityCentimetersPerSecond = Recall::Math::Utils::UnitsPerFrameToPerSecond(
					Body.GetLinearVelocity());
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
	ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::All);
	ProcessingPhase = EMassProcessingPhase::EndPhysics;
	ExecutionOrder.ExecuteAfter.Add(Recall::Physics::ProcessorGroupNames::EndSimulation);
	ExecutionOrder.ExecuteBefore.Add(Recall::Physics::ProcessorGroupNames::CopyLocation);
}

void URecallCrowdAgentGetBlackboardProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallCrowdAgentGetBlackboardProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallCrowdAgentFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FRecallPhysicsBodyFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddSubsystemRequirement<URecallCrowdSubsystem>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddSubsystemRequirement<URecallPhysicsSubsystem>(EMassFragmentAccess::ReadWrite);
}

void URecallCrowdAgentGetBlackboardProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_CrowdAgentGetBlackboard_Execute);
	
	EntityQuery.ParallelForEachEntityChunk(Context, [](FMassExecutionContext& Context)
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
			const FRecallPhysicsBodyView Body = PhysicsSystem.GetMutableBody(BodyFragment.BodyHandle);
			if (!ensure(Body.IsValid()))
			{
				continue;
			}

			if (AgentBlackboard.HasPositionChanged())
			{
#if RECALL_DESYNC_LOG
				RECALL_DESYNC_LOG_VEC(Context.GetWorld(), GetAgentBlackboardLocation, AgentBlackboard.Location);
#endif // RECALL_DESYNC_LOG
				Body.SetPosition(AgentBlackboard.Location);
			}

			if (AgentBlackboard.HasVelocityChanged())
			{
#if RECALL_DESYNC_LOG
				RECALL_DESYNC_LOG_VEC(Context.GetWorld(), GetAgentBlackboardVelocity, AgentBlackboard.VelocityCentimetersPerSecond);
#endif // RECALL_DESYNC_LOG
				Body.SetLinearVelocity(Recall::Math::Utils::UnitsPerSecondToPerFrame(
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
	ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::All);
	ProcessingPhase = EMassProcessingPhase::DuringPhysics;
}

void URecallCrowdUpdateProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

bool URecallCrowdUpdateProcessor::ShouldAllowQueryBasedPruning(const bool bRuntimeMode) const
{
	return false;
}

void URecallCrowdUpdateProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	ProcessorRequirements.AddSubsystemRequirement<URecallCrowdSubsystem>(EMassFragmentAccess::ReadWrite);
}

void URecallCrowdUpdateProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_CrowdUpdate_Execute);

	URecallCrowdSubsystem& CrowdSystem = Context.GetMutableSubsystemChecked<URecallCrowdSubsystem>();
	CrowdSystem.TickCrowdManager(Context.GetDeltaTimeSeconds());
}
