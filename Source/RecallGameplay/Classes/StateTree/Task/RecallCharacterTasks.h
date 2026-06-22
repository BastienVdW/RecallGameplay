// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "StateTree/RecallStateTreeTaskBase.h"

#include "RecallCharacterTasks.generated.h"

USTRUCT()
struct RECALLGAMEPLAY_API FRecallCharacterLocomotionTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category=Parameter)
	FVector2f MovementAxis = FVector2f::ZeroVector;
	
	UPROPERTY(EditAnywhere, Category=Parameter)
	FVector2f LookAxis = FVector2f::ZeroVector;
};

USTRUCT(meta=(DisplayName="Character Locomotion"))
struct RECALLGAMEPLAY_API FRecallCharacterLocomotionTask : public FRecallStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallCharacterLocomotionTaskInstanceData;

protected:
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

	UPROPERTY(EditAnywhere, meta=(Units=DegreesPerSecond))
	float LookSpeed = 200.0f;
	
protected:
	TStateTreeExternalDataHandle<struct FRecallControllerFragment> ControllerFragmentHandle;
	TStateTreeExternalDataHandle<struct FRecallMovementFragment> MovementFragmentHandle;

	void GetMovementControl(FStateTreeExecutionContext& Context,
		FVector2f& OutMovementDirection, FRotator& OutControlRotation) const;
};

USTRUCT()
struct RECALLGAMEPLAY_API FRecallCharacterJumpTaskInstanceData
{
	GENERATED_BODY()
};

USTRUCT(meta=(DisplayName="Character Jump"))
struct RECALLGAMEPLAY_API FRecallCharacterJumpTask : public FRecallStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallCharacterJumpTaskInstanceData;

protected:
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	
protected:
	TStateTreeExternalDataHandle<struct FRecallMovementFragment> MovementFragmentHandle;
};

USTRUCT()
struct RECALLGAMEPLAY_API FRecallCharacterVirtualStanceTaskInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category=Parameter)
	bool bStand = true;
};

USTRUCT(meta=(DisplayName="Character Virtual Stance"))
struct RECALLGAMEPLAY_API FRecallCharacterVirtualStanceTask : public FRecallStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallCharacterVirtualStanceTaskInstanceData;

protected:
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	
protected:
	TStateTreeExternalDataHandle<struct FRecallPhysicsBodyFragment> BodyFragmentHandle;
	TStateTreeExternalDataHandle<class URecallPhysicsSubsystem> PhysicsSystemHandle;
};
