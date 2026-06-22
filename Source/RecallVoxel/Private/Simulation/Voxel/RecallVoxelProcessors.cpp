// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallVoxelProcessors.h"

#include "MassExtendedExecutionContext.h"
#include "RecallSignalSubsystem.h"
#include "Simulation/Physics/RecallPhysicsBodyFragment.h"
#include "Simulation/Voxel/RecallVoxelFragments.h"
#include "System/Physics/RecallPhysicsSubsystem.h"
#include "Utility/Physics/RecallPhysicsUtils.h"
#include "Utility/Voxel/RecallVoxelUtils.h"

//----------------------------------------------------------------------//
// URecallVoxelConstructor
//----------------------------------------------------------------------//
URecallVoxelConstructor::URecallVoxelConstructor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EExtendedProcessorExecutionFlags::All);
	ObservedType = FRecallVoxelShapeFragment::StaticStruct();
	Operation = EMassExtendedObservedOperation::Add;
}

void URecallVoxelConstructor::InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallVoxelConstructor::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallPhysicsBodyFragment>(EMassExtendedFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FRecallVoxelShapeFragment>(EMassExtendedFragmentAccess::ReadWrite);
	EntityQuery.AddConstSharedRequirement<FRecallVoxelShapeConstSharedFragment>();
	EntityQuery.AddSubsystemRequirement<URecallPhysicsSubsystem>(EMassExtendedFragmentAccess::ReadWrite);
}

void URecallVoxelConstructor::Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_Voxel_Constructor);

	EntityQuery.ForEachEntityChunk(Context, [](FMassExtendedExecutionContext& Context)
	{
		URecallPhysicsSubsystem& PhysicsSystem = Context.GetMutableSubsystemChecked<URecallPhysicsSubsystem>();

		const FRecallVoxelShapeConstSharedFragment& ShapeParams = Context.GetConstSharedFragment<FRecallVoxelShapeConstSharedFragment>();

		const TArrayView<FRecallPhysicsBodyFragment> BodyList = Context.GetMutableFragmentView<FRecallPhysicsBodyFragment>();
		const TArrayView<FRecallVoxelShapeFragment> VoxelList = Context.GetMutableFragmentView<FRecallVoxelShapeFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			const FMassExtendedEntityHandle Entity = Context.GetEntity(EntityIndex);

			FRecallPhysicsBodyFragment& BodyFragment = BodyList[EntityIndex];
			FRecallVoxelShapeFragment& VoxelFragment = VoxelList[EntityIndex];

			if (ShapeParams.bConvexHull)
			{
				TArray<int32> DummyTriangles;

				FRecallPhysicsConvexHullShape ConvexHullShape;
				Recall::Voxel::Utils::GenerateMesh(VoxelFragment.VoxelGrid, ConvexHullShape.Vertices, DummyTriangles,
					ShapeParams.VoxelSize, VoxelFragment.PivotVoxelIndex);

				BodyFragment.BodyHandle = PhysicsSystem.CreateShape(Entity, ConvexHullShape, ShapeParams.PhysicsParams);
			}
			else
			{
				FRecallPhysicsMeshShape MeshShape;
				Recall::Voxel::Utils::GenerateMesh(VoxelFragment.VoxelGrid, MeshShape.Vertices, MeshShape.Triangles,
					ShapeParams.VoxelSize, VoxelFragment.PivotVoxelIndex);

				BodyFragment.BodyHandle = PhysicsSystem.CreateShape(Entity, MeshShape, ShapeParams.PhysicsParams);
			}
			
			Recall::Physics::Utils::InitializePhysicsBody(Context, Entity,
				PhysicsSystem, BodyFragment, ShapeParams.PhysicsParams);
		}
	});
}

//----------------------------------------------------------------------//
// URecallVoxelDestructor
//----------------------------------------------------------------------//
URecallVoxelDestructor::URecallVoxelDestructor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EExtendedProcessorExecutionFlags::All);
	ObservedType = FRecallVoxelShapeFragment::StaticStruct();
	Operation = EMassExtendedObservedOperation::Remove;
}

void URecallVoxelDestructor::InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallVoxelDestructor::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallPhysicsBodyFragment>(EMassExtendedFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FRecallVoxelShapeFragment>(EMassExtendedFragmentAccess::ReadWrite);
	EntityQuery.AddSubsystemRequirement<URecallPhysicsSubsystem>(EMassExtendedFragmentAccess::ReadWrite);

}

void URecallVoxelDestructor::Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_Voxel_Destructor);

	EntityQuery.ForEachEntityChunk(Context, [](FMassExtendedExecutionContext& Context)
	{
		URecallPhysicsSubsystem& PhysicsSystem = Context.GetMutableSubsystemChecked<URecallPhysicsSubsystem>();

		const TArrayView<FRecallPhysicsBodyFragment> BodyList = Context.GetMutableFragmentView<FRecallPhysicsBodyFragment>();
		const TArrayView<FRecallVoxelShapeFragment> VoxelList = Context.GetMutableFragmentView<FRecallVoxelShapeFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			FRecallPhysicsBodyFragment& BodyFragment = BodyList[EntityIndex];
			FRecallVoxelShapeFragment& VoxelFragment = VoxelList[EntityIndex];

			PhysicsSystem.ReleaseBody(BodyFragment.BodyHandle);
		}
	});
}

/*
//----------------------------------------------------------------------//
// URecallVoxelSignalProcessor
//----------------------------------------------------------------------//
URecallVoxelSignalProcessor::URecallVoxelSignalProcessor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void URecallVoxelSignalProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);

	// SubscribeToSignal(Recall::Voxel::Signals::DummySignal);
}

void URecallVoxelSignalProcessor::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallVoxelShapeFragment>(EMassExtendedFragmentAccess::ReadOnly);
	EntityQuery.AddConstSharedRequirement<FRecallVoxelConstSharedFragment>();
	EntityQuery.AddSubsystemRequirement<URecallSignalSubsystem>(EMassExtendedFragmentAccess::ReadWrite);
}

void URecallVoxelSignalProcessor::SignalEntities(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context, FRecallSignalNameLookup& EntitySignals)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_Voxel_Signal);

	EntityQuery.ForEachEntityChunk(Context, [&EntitySignals](FMassExtendedExecutionContext& Context)
	{
		const FMassExtendedEntityManager& EntityManager = Context.GetEntityManagerChecked();
		URecallSignalSubsystem& SignalSystem = Context.GetMutableSubsystemChecked<URecallSignalSubsystem>();

		const FRecallVoxelConstSharedFragment& VoxelConstSharedFragment = Context.GetConstSharedFragment<FRecallVoxelConstSharedFragment>();

		const TArrayView<FRecallVoxelShapeFragment> VoxelList = Context.GetMutableFragmentView<FRecallVoxelShapeFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			const FMassExtendedEntityHandle Entity = Context.GetEntity(EntityIndex);

			FRecallVoxelShapeFragment& VoxelFragment = VoxelList[EntityIndex];
		}
	});
}

//----------------------------------------------------------------------//
// URecallVoxelProcessor
//----------------------------------------------------------------------//
URecallVoxelProcessor::URecallVoxelProcessor()
	: EntityQuery(*this)
{
	ExecutionFlags = (int32)EExtendedProcessorExecutionFlags::All;
	ProcessingPhase = EMassExtendedProcessingPhase::FrameEnd;
}

void URecallVoxelProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

bool URecallVoxelProcessor::ShouldAllowQueryBasedPruning(const bool bRuntimeMode) const
{
	return Super::ShouldAllowQueryBasedPruning(bRuntimeMode);
}

void URecallVoxelProcessor::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallVoxelShapeFragment>(EMassExtendedFragmentAccess::ReadOnly);
}

void URecallVoxelProcessor::Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_Voxel_Execute);

	EntityQuery.ForEachEntityChunk(Context, [](FMassExtendedExecutionContext& Context)
	{
		const FMassExtendedEntityManager& EntityManager = Context.GetEntityManagerChecked();

		const TConstArrayView<FRecallVoxelShapeFragment> VoxelList = Context.GetFragmentView<FRecallVoxelShapeFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			const FRecallVoxelShapeFragment& VoxelFragment = VoxelList[EntityIndex];
		}
	});
}
*/
