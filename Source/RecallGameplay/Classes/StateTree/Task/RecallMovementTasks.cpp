// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallMovementTasks.h"

#include "MassEntityView.h"
#include "RecallSignalSubsystem.h"
#include "Physics/RecallPhysicsObjects.h"
#include "Simulation/Movement/RecallMovementFragments.h"
#include "Simulation/Physics/RecallPhysicsBodyFragment.h"
#include "Simulation/Transform/RecallTransformFragments.h"
#include "StateTree/RecallStateTreeExecutionContext.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "System/Physics/RecallPhysicsSubsystem.h"

//----------------------------------------------------------------------//
// FRecallMoveToward
//----------------------------------------------------------------------//
bool FRecallMoveTowardTask::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(TransformFragmentHandle);
	Linker.LinkExternalData(MovementFragmentHandle);
	return true;
}

EStateTreeRunStatus FRecallMoveTowardTask::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	return Super::EnterState(Context, Transition);
}

void FRecallMoveTowardTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FRecallMovementFragment& MovementFragment = Context.GetExternalData(MovementFragmentHandle);
	MovementFragment.MovementDirection = FVector2f::ZeroVector;

	return Super::ExitState(Context, Transition);
}

FVector2f FRecallMoveTowardTask::GetMoveTowardVector(FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	const FRecallTransformFragment& TransformFragment = Context.GetExternalData(TransformFragmentHandle);

	switch (GoalType)
	{
	case ERecallMoveTowardTarget::Direction:
		return (FVector2f)static_cast<FVector2D>(InstanceData.MoveToward.GetClampedToMaxSize(1.0f));

	case ERecallMoveTowardTarget::Location:
		return (FVector2f)static_cast<FVector2D>((InstanceData.MoveToward - TransformFragment.Position).GetSafeNormal2D());

	default:
		unimplemented();
		return FVector2f::ZeroVector;
	}
}

EStateTreeRunStatus FRecallMoveTowardTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	if (GoalType == ERecallMoveTowardTarget::Location)
	{
		const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
		const FRecallTransformFragment& TransformFragment = Context.GetExternalData(TransformFragmentHandle);

		if (FVector2D::DistSquared((FVector2D)TransformFragment.Position, (FVector2D)InstanceData.MoveToward) < ReachGoalThreshold * ReachGoalThreshold)
		{
			return EStateTreeRunStatus::Succeeded;
		}
	}

	FRecallMovementFragment& MovementFragment = Context.GetExternalData(MovementFragmentHandle);
	MovementFragment.MovementDirection = GetMoveTowardVector(Context);

	return Super::Tick(Context, DeltaTime);
}

//----------------------------------------------------------------------//
// FRecallRotateTask
//----------------------------------------------------------------------//
bool FRecallRotateTask::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(BodyFragmentHandle);
	Linker.LinkExternalData(PhysicsSystemHandle);
	return true;
}

EStateTreeRunStatus FRecallRotateTask::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	return Super::EnterState(Context, Transition);
}

void FRecallRotateTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	return Super::ExitState(Context, Transition);
}

EStateTreeRunStatus FRecallRotateTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	const FRecallPhysicsBodyFragment& BodyFragment = Context.GetExternalData(BodyFragmentHandle);
	URecallPhysicsSubsystem& PhysicsSystem = Context.GetExternalData(PhysicsSystemHandle);

	const TWeakPtr<FRecallPhysicsBody> PhysicsBody = PhysicsSystem.GetMutableBody(BodyFragment.BodyHandle);
	if (!ensure(PhysicsBody.IsValid()))
	{
		return EStateTreeRunStatus::Failed;
	}

	FVector BodyPosition = FVector::ZeroVector;
	FQuat BodyRotation = FQuat::Identity;
	PhysicsBody.Pin()->GetPositionAndRotation(BodyPosition, BodyRotation);

	const FQuat Rotation = GetTargetRotation(Context, BodyPosition, BodyRotation);

	PhysicsBody.Pin()->SetRotation(Rotation);

	return Super::Tick(Context, DeltaTime);
}

FQuat FRecallRotateTask::GetTargetRotation(FStateTreeExecutionContext& Context,
	const FVector& Position, const FQuat& Rotation) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	auto GetEntityLocation = [&Context](const FMassEntityHandle& Entity)
	{
		const FRecallStateTreeExecutionContext& MassContext = static_cast<const FRecallStateTreeExecutionContext&>(Context);
		const FMassEntityManager& EntityManager = MassContext.GetEntityManager();
		if (!EntityManager.IsEntityValid(Entity))
		{
			return FVector::ZeroVector;
		}
		
		const FMassEntityView EntityView(EntityManager, Entity);
		const auto& TransformFragment = EntityView.GetFragmentData<FRecallTransformFragment>();
		return TransformFragment.Position;
	};

	switch (InstanceData.Target)
	{
	case ERecallRotateTaskType::Fixed:
		return Rotation * InstanceData.Rotation.Quaternion();

	case ERecallRotateTaskType::Direction:
		return InstanceData.Direction.ToOrientationQuat();

	case ERecallRotateTaskType::TowardLocation:
		return (InstanceData.Location - Position).ToOrientationQuat();

	case ERecallRotateTaskType::TowardEntity:
		return (GetEntityLocation(InstanceData.Entity) - Position).ToOrientationQuat();
		
	default:
		unimplemented();
		return FQuat::Identity;
	}
}
