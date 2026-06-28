// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallPhysicsTasks.h"

#include "RecallSignalSubsystem.h"
#include "StateTree/RecallStateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "Kismet/KismetMathLibrary.h"
#include "Physics/RecallPhysicsObjects.h"
#include "Simulation/Controller/RecallControllerFragments.h"
#include "Simulation/Physics/RecallPhysicsBodyFragment.h"
#include "Simulation/Physics/RecallPhysicsVehicleFragments.h"
#include "Simulation/StateTree/RecallStateTreeSignalTypes.h"
#include "Simulation/Transform/RecallTransformFragments.h"
#include "System/Physics/RecallPhysicsSubsystem.h"

//----------------------------------------------------------------------//
// FRecallVehicleDriverInputTask
//----------------------------------------------------------------------//
bool FRecallVehicleDriverInputTask::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(VehicleFragmentHandle);
	return true;
}

EStateTreeRunStatus FRecallVehicleDriverInputTask::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	return Super::EnterState(Context, Transition);
}

void FRecallVehicleDriverInputTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	return Super::ExitState(Context, Transition);
}

EStateTreeRunStatus FRecallVehicleDriverInputTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	FRecallPhysicsVehicleFragment& VehicleFragment = Context.GetExternalData(VehicleFragmentHandle);
	VehicleFragment.Forward = InstanceData.Forward;
	VehicleFragment.Right = InstanceData.Right;
	VehicleFragment.Brake = InstanceData.Brake;
	VehicleFragment.HandBrake = InstanceData.HandBrake;
	
	return Super::Tick(Context, DeltaTime);
}

//----------------------------------------------------------------------//
// FRecallImpulseTask
//----------------------------------------------------------------------//
FRecallImpulseTask::FRecallImpulseTask()
{
	bShouldStateChangeOnReselect = false;
}

bool FRecallImpulseTask::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(TransformFragmentHandle);
	Linker.LinkExternalData(ControllerFragmentHandle);
	Linker.LinkExternalData(BodyFragmentHandle);
	Linker.LinkExternalData(PhysicsSystemHandle);
	return true;
}

EStateTreeRunStatus FRecallImpulseTask::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.ElapsedTime = 0.0f;
	InstanceData.StartDirection = InstanceData.Direction;
	
	if (bApplyOverTime)
	{
		const FRecallStateTreeExecutionContext& RecallContext = static_cast<FRecallStateTreeExecutionContext&>(Context);
		RecallContext.GetSignalSystem().SignalEntity(Recall::StateTree::Signals::TickRequired,
			RecallContext.GetEntity());
	}
	else
	{
		ApplyImpulse(Context, InstanceData.Force);
		
		if (bSucceedOnImpulse)
		{
			return EStateTreeRunStatus::Succeeded;
		}
	}
	
	return Super::EnterState(Context, Transition);
}

EStateTreeRunStatus FRecallImpulseTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	if (bApplyOverTime && Duration > 0.0f)
	{
		FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
		
		const float Alpha = FMath::Clamp(InstanceData.ElapsedTime / Duration, 0.0f, 1.0f);
		
		if (!FMath::IsNearlyEqual(Alpha, 1.0f))
		{		
			const float Force = UKismetMathLibrary::Ease(0.0f, InstanceData.Force, Alpha,
				EasingFunction, EasingBlendExp, EasingSteps);

			ApplyImpulse(Context, Force);
			
			const FRecallStateTreeExecutionContext& RecallContext = static_cast<FRecallStateTreeExecutionContext&>(Context);
			RecallContext.GetSignalSystem().SignalEntity(Recall::StateTree::Signals::TickRequired,
				RecallContext.GetEntity());
		}
		else if (bSucceedOnImpulse)
		{
			return EStateTreeRunStatus::Succeeded;
		}
		
		InstanceData.ElapsedTime += DeltaTime;
	}
	
	return FRecallStateTreeTaskBase::Tick(Context, DeltaTime);
}

void FRecallImpulseTask::ApplyImpulse(FStateTreeExecutionContext& Context, float Force) const
{
	URecallPhysicsSubsystem& PhysicsSystem = Context.GetExternalData(PhysicsSystemHandle);
	const FRecallPhysicsBodyFragment& BodyFragment = Context.GetExternalData(BodyFragmentHandle);

	const FRecallPhysicsBodyView Body = PhysicsSystem.GetMutableBody(BodyFragment.BodyHandle);
	if (!Body.IsValid())
	{
		return;
	}

	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	FVector Direction = bKeepStartDirection ? InstanceData.StartDirection : InstanceData.Direction;
	
	if (bUseRelativeDirection)
	{
		const FRecallControllerFragment* ControllerFragmentPtr = Context.GetExternalDataPtr(ControllerFragmentHandle);
		if (bUseControlRotation && ControllerFragmentPtr != nullptr)
		{
			Direction = ControllerFragmentPtr->ControlRotation.Quaternion().RotateVector(Direction);
		}
		else
		{
			const FRecallTransformFragment& TransformFragment = Context.GetExternalData(TransformFragmentHandle);
			Direction = TransformFragment.GetTransform().TransformVectorNoScale(Direction);			
		}
	}

	if (bAbsolute)
	{
		if (bUse2DVelocity)
		{
			Body.SetLinearVelocity2DFromImpulse(Direction.GetSafeNormal() * InstanceData.Force, bIgnoreMass);
		}
		else
		{			
			Body.SetLinearVelocityFromImpulse(Direction.GetSafeNormal() * InstanceData.Force, bIgnoreMass);
		}
	}
	else
	{
		Body.AddImpulse(Direction.GetSafeNormal() * InstanceData.Force, bIgnoreMass);
	}
}
