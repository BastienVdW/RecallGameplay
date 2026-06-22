// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Kismet/KismetMathLibrary.h"
#include "StateTree/RecallStateTreeTaskBase.h"

#include "RecallPhysicsTasks.generated.h"

namespace EEasingFunc
{
	enum Type : int;
}

USTRUCT()
struct RECALLGAMEPLAY_API FRecallVehicleDriverInputTaskInstanceData
{
	GENERATED_BODY()
	
	/**
	 * Value between -1 and 1 for auto transmission and value between 0 and 1 indicating desired driving direction and amount the gas pedal is pressed
	 */
	UPROPERTY(EditAnywhere, Category=Parameter)
	float Forward = 0.0f;

	/**
	 * Value between -1 and 1 indicating desired steering angle (1 = right)
	 */
	UPROPERTY(EditAnywhere, Category=Parameter)
	float Right = 0.0f;

	/**
	 * Value between 0 and 1 indicating how strong the brake pedal is pressed
	 */
	UPROPERTY(EditAnywhere, Category=Parameter)
	float Brake = 0.0f;

	/**
	 * Value between 0 and 1 indicating how strong the hand brake is pulled
	 */
	UPROPERTY(EditAnywhere, Category=Parameter)
	float HandBrake = 0.0f;
};


USTRUCT(meta=(DisplayName="Vehicle Driver Input"))
struct RECALLGAMEPLAY_API FRecallVehicleDriverInputTask : public FRecallStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallVehicleDriverInputTaskInstanceData;

protected:
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

protected:
	TStateTreeExternalDataHandle<struct FRecallPhysicsVehicleFragment> VehicleFragmentHandle;
};

USTRUCT()
struct RECALLGAMEPLAY_API FRecallImpulseTaskInstanceData
{
	GENERATED_BODY()

	/**
	 * Directory of the force to apply to this entity.
	 */
	UPROPERTY(EditAnywhere, Category=Parameter)
	FVector Direction = FVector::ForwardVector;

	/**
	 * Force to apply to this entity.
	 */
	UPROPERTY(EditAnywhere, Category=Parameter, meta=(Units=Newtons))
	float Force = 10.0f;

	UPROPERTY()
	FVector StartDirection = FVector::ZeroVector;
	
	UPROPERTY()
	float ElapsedTime = 0.0f;
};


USTRUCT(meta=(DisplayName="Add Impulse"))
struct RECALLGAMEPLAY_API FRecallImpulseTask : public FRecallStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallImpulseTaskInstanceData;

	FRecallImpulseTask();

protected:
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	
	/**
	 * Impulse will ignore the mass of the entity.
	 */
	UPROPERTY(EditAnywhere, Category=Parameter)
	bool bIgnoreMass = true;

	/**
	 * Task will succeed as soon as the impulse is applied.
	 */
	UPROPERTY(EditAnywhere, Category=Parameter)
	bool bSucceedOnImpulse = true;

	/**
	 * Impulse is absolute and will override any existing velocity.
	 */
	UPROPERTY(EditAnywhere, Category=Parameter)
	bool bAbsolute = true;
	
	/**
	 * Absolute impulse will only set the XY velocity.
	 */
	UPROPERTY(EditAnywhere, Category=Parameter, meta=(EditCondition=bAbsolute))
	bool bUse2DVelocity = true;

	/**
	 * Direction is relative to the entity's transform.
	 */
	UPROPERTY(EditAnywhere, Category=Parameter)
	bool bUseRelativeDirection = false;

	/**
	 * Use the controller rotation as the transform for the relative direction.
	 */
	UPROPERTY(EditAnywhere, Category=Parameter, meta=(EditCondition=bUseRelativeDirection))
	bool bUseControlRotation = true;

	/**
	 * Apply the impulse over multiple ticks.
	 */
	UPROPERTY(EditAnywhere, Category=Parameter)
	bool bApplyOverTime = false;

	UPROPERTY(EditAnywhere, Category=Parameter, meta=(EditCondition=bApplyOverTime, Units=Seconds))
	float Duration = 3.0f;

	/**
	 * Keep moving in the start direction.
	 */
	UPROPERTY(EditAnywhere, Category=Parameter, meta=(EditCondition=bApplyOverTime))
	bool bKeepStartDirection = true;
	
	UPROPERTY(EditAnywhere, Category=Parameter, meta=(EditCondition=bApplyOverTime))
	TEnumAsByte<EEasingFunc::Type> EasingFunction = EEasingFunc::Type::EaseInOut;

	UPROPERTY(EditAnywhere, Category=Parameter, meta=(EditCondition=bApplyOverTime, ClampMin=0.1, ClampMax=3.0))
	float EasingBlendExp = 2.0f;

	UPROPERTY(EditAnywhere, Category=Parameter, meta=(EditCondition=bApplyOverTime, ClampMin=1, ClampMax=10))
	int32 EasingSteps = 2;
	
protected:
	TStateTreeExternalDataHandle<struct FRecallTransformFragment> TransformFragmentHandle;
	TStateTreeExternalDataHandle<struct FRecallControllerFragment, EStateTreeExternalDataRequirement::Optional> ControllerFragmentHandle;
	TStateTreeExternalDataHandle<struct FRecallPhysicsBodyFragment> BodyFragmentHandle;
	TStateTreeExternalDataHandle<class URecallPhysicsSubsystem> PhysicsSystemHandle;

	void ApplyImpulse(FStateTreeExecutionContext& Context, float Force) const;
};
