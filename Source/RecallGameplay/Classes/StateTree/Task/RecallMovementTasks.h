// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "MassExtendedEntityHandle.h"
#include "StateTree/RecallStateTreeTaskBase.h"
#include "StateTreePropertyRef.h"
#include "AI/Navigation/NavAgentInterface.h"

#include "RecallMovementTasks.generated.h"

UENUM()
enum class ERecallMoveTowardTarget : uint8
{
	Direction,
	Location,
};

USTRUCT()
struct RECALLGAMEPLAY_API FRecallMoveTowardTaskInstanceData
{
	GENERATED_BODY()
	
	// Location or direction to move toward
	UPROPERTY(EditAnywhere, Category=Parameter)
	FVector MoveToward = FVector::ZeroVector;
};

/**
* Move toward a location by using the movement trait
*/
USTRUCT(meta=(DisplayName="Agent Move Torward"))
struct RECALLGAMEPLAY_API FRecallMoveTowardTask : public FRecallStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallMoveTowardTaskInstanceData;

protected:
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	
	UPROPERTY(EditAnywhere)
	ERecallMoveTowardTarget GoalType = ERecallMoveTowardTarget::Location;

	UPROPERTY(EditAnywhere, meta=(EditCondition="GoalType == ERecallMoveTowardTarget::Location", EditConditionHides))
	float ReachGoalThreshold = 50.0f;

protected:
	TStateTreeExternalDataHandle<struct FRecallTransformFragment> TransformFragmentHandle;
	TStateTreeExternalDataHandle<struct FRecallMovementFragment> MovementFragmentHandle;

	FVector2f GetMoveTowardVector(FStateTreeExecutionContext& Context) const;
};

UENUM()
enum class ERecallRotateTaskType : uint8
{
	Fixed,
	Direction,
	TowardEntity,
	TowardLocation,
};

USTRUCT()
struct RECALLGAMEPLAY_API FRecallRotateTaskInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category=Parameter)
	ERecallRotateTaskType Target = ERecallRotateTaskType::Fixed;
	
	UPROPERTY(EditAnywhere, Category=Parameter, meta=(EditCondition="Target == ERecallRotateTaskType::Fixed", EditConditionHides))
	FRotator Rotation = FRotator::ZeroRotator;
	
	UPROPERTY(EditAnywhere, Category=Parameter, meta=(EditCondition="Target == ERecallRotateTaskType::Direction", EditConditionHides))
	FVector Direction = FVector::ZeroVector;
	
	UPROPERTY(EditAnywhere, Category=Parameter, meta=(EditCondition="Target == ERecallRotateTaskType::TowardLocation", EditConditionHides))
	FVector Location = FVector::ZeroVector;
	
	UPROPERTY(EditAnywhere, Category=Parameter, meta=(EditCondition="Target == ERecallRotateTaskType::TowardEntity", EditConditionHides))
	FMassExtendedEntityHandle Entity;
};

/**
* Rotate around physics body
*/
USTRUCT(meta=(DisplayName="Rotate"))
struct RECALLGAMEPLAY_API FRecallRotateTask : public FRecallStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallRotateTaskInstanceData;

protected:
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	
protected:
	TStateTreeExternalDataHandle<struct FRecallPhysicsBodyFragment> BodyFragmentHandle;
	TStateTreeExternalDataHandle<class URecallPhysicsSubsystem> PhysicsSystemHandle;

private:
	FQuat GetTargetRotation(FStateTreeExecutionContext& Context, const FVector& Position, const FQuat& Rotation) const;
};
