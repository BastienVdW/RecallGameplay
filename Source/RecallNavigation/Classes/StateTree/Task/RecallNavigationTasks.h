// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "StateTree/RecallStateTreeTaskBase.h"
#include "StateTreePropertyRef.h"
#include "AI/Navigation/NavAgentInterface.h"
#include "System/EQS/RecallEnvQueryTypes.h"

#include "RecallNavigationTasks.generated.h"

USTRUCT()
struct RECALLNAVIGATION_API FRecallMoveAtTaskInstanceData
{
	GENERATED_BODY()
	
	// Location to move at
	UPROPERTY(EditAnywhere, Category=Parameter)
	FVector Location = FVector::ZeroVector;

	UPROPERTY()
	float RefreshPathTimer = 0.0f;
};

/**
* Request the navigation agent to move at a location by using the navigation system
* Requires the navigation agent and the movement trait to be attached to the agent
*/
USTRUCT(meta=(DisplayName="Navigation Agent Move At"))
struct RECALLNAVIGATION_API FRecallMoveAtTask : public FRecallStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallMoveAtTaskInstanceData;

protected:
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	
	UPROPERTY(EditAnywhere)
	bool bExitOnReachDestination = true;
	UPROPERTY(EditAnywhere)
	bool bStopOnExit = true;

	UPROPERTY(EditAnywhere, AdvancedDisplay)
	bool bRefreshPath = false;
	
	UPROPERTY(EditAnywhere, meta=(EditCondition="bRefreshPath", Units="Seconds", ClampMin="0.0"), AdvancedDisplay, DisplayName="Duration")
	float RefreshPathDuration = 1.0f;
	
	/** Adds random range to the Duration. */
	UPROPERTY(EditAnywhere, meta=(EditCondition="bRefreshPath", Units="Seconds", ClampMin="0.0"), AdvancedDisplay, DisplayName="Random Deviation")
	float RefreshPathRandomDeviation = 0.1f;

protected:
	TStateTreeExternalDataHandle<struct FRecallPathFollowerFragment> PathFollowerFragmentHandle;

	void StartRefreshPathDelay(FStateTreeExecutionContext& Context) const;
};

USTRUCT()
struct RECALLNAVIGATION_API FRecallEnvQueryDebugNodeData
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	FVector Location = FVector::ZeroVector;

	UPROPERTY(Transient)
	float Value{ 0.0f };
};

UENUM()
enum class ERecallEnvQueryTarget : uint8
{
	Location,
	Entity,
};

USTRUCT()
struct RECALLNAVIGATION_API FRecallEnvQueryTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category=Parameter)
	ERecallEnvQueryTarget Target = ERecallEnvQueryTarget::Location;
	
	UPROPERTY(EditAnywhere, Category=Parameter, meta=(EditCondition="Target==ERecallEnvQueryTarget::Location", EditConditionHides), DisplayName="Location")
	FVector TargetLocation = FVector::ZeroVector;
	
	UPROPERTY(EditAnywhere, Category=Parameter, meta=(EditCondition="Target==ERecallEnvQueryTarget::Entity", EditConditionHides))
	FMassExtendedEntityHandle Entity;

	/**
	 * Make the query result relative to the target entity.
	 */
	UPROPERTY(EditAnywhere, Category=Parameter, meta=(EditCondition="Target==ERecallEnvQueryTarget::Entity", EditConditionHides))
	bool bRelative = false;

	UPROPERTY(EditAnywhere, Category=Input)
	TStateTreePropertyRef<FVector> QueryResult;

	UPROPERTY()
	FRecallEnvQueryHandle EnvQueryHandle;

	UPROPERTY()
	FTransform RelativeTransform = FTransform::Identity;
	
	FORCEINLINE bool ShouldUseRelativeLocation() const
	{
		return Target == ERecallEnvQueryTarget::Entity && bRelative;
	}
};

/**
* Task to run an env query and return the result target location
* This is done asynchronously over multiple frames and will succeed or fail when it complete
*/
USTRUCT(meta=(DisplayName="Run Environment Query"))
struct RECALLNAVIGATION_API FRecallEnvQueryTask : public FRecallStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallEnvQueryTaskInstanceData;

protected:
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UEnvQuery> EnvQuery;

	UPROPERTY(EditAnywhere, meta=(ClampMin=0.0, ClampMax=1.0))
	float MinimumScore = 0.95f;
	
	UPROPERTY(EditAnywhere, meta=(ClampMin=0.0))
	float ObstacleSearchRadius = 500.0f;

protected:
	TStateTreeExternalDataHandle<struct FRecallTransformFragment> TransformFragmentHandle;
	TStateTreeExternalDataHandle<class URecallEnvQuerySubsystem> EnvQuerySystemHandle;
	TStateTreeExternalDataHandle<class URecallNavigationObstacleSubsystem> NavigationObstacleSystemHandle;

	struct FRecallEnvQueryRequest GenerateEnvQueryRequest(FStateTreeExecutionContext& Context) const;
};
