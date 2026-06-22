// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "StateTree/RecallStateTreeTaskBase.h"
#include "MassExtendedEntityHandle.h"
#include "StateTreePropertyRef.h"

#include "RecallEntityTasks.generated.h"

USTRUCT()
struct RECALLGAMEPLAY_API FRecallEntityLocationTaskInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category=Parameter)
	FMassExtendedEntityHandle Entity;

	UPROPERTY(EditAnywhere, Category=Input)
	TStateTreePropertyRef<FVector> Location;
};

/**
 * Access the location of an entity.
 */
USTRUCT(meta=(DisplayName="Entity Location"))
struct RECALLGAMEPLAY_API FRecallEntityLocationTask : public FRecallStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallEntityLocationTaskInstanceData;

protected:
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	
	/** This task succeeds as soon as we get the entity location */
	UPROPERTY(EditAnywhere)
	bool bSucceedOnGet = false;

private:
	bool UpdateEntityLocation(FStateTreeExecutionContext& Context) const;
};

USTRUCT()
struct RECALLGAMEPLAY_API FRecallFilterEntityTaskInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category=Parameter)
	TArray<FMassExtendedEntityHandle> Entities;
	
	UPROPERTY(EditAnywhere, Category=Input, meta=(CanRefToArray))
	TStateTreePropertyRef<FMassExtendedEntityHandle> Result;
};

/**
 * Filter entities by condition.
 */
USTRUCT(meta=(DisplayName="Filter Entities"))
struct RECALLGAMEPLAY_API FRecallFilterEntityTask : public FRecallStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallFilterEntityTaskInstanceData;

protected:
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	/** This task succeeds as soon as we filter the entities */
	UPROPERTY(EditAnywhere, Category=Parameter)
	bool bSucceedOnFilter = false;
	
	UPROPERTY(EditAnywhere, Category=Parameter, meta=(BaseStruct="/Script/RecallGameplay.RecallStateTreeEntityFilterBase", ExcludeBaseStruct))
	TArray<FInstancedStruct> Conditions;
};

USTRUCT()
struct RECALLGAMEPLAY_API FRecallDestroyEntityTaskInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category=Input)
	TArray<FMassExtendedEntityHandle> Entities;
};

/**
 * Destroy entities;
 */
USTRUCT(meta=(DisplayName="Destroy Entities"))
struct RECALLGAMEPLAY_API FRecallDestroyEntityTask : public FRecallStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallDestroyEntityTaskInstanceData;

protected:
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	UPROPERTY(EditAnywhere, Category=Parameter)
	bool bSucceedOnDestroy = false;
};

USTRUCT()
struct RECALLGAMEPLAY_API FRecallSpawnEntityTaskInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category=Parameter)
	TObjectPtr<class UMassExtendedEntityConfigAsset> EntityConfig;

	UPROPERTY(EditAnywhere, Category=Parameter)
	FVector Position = FVector::ZeroVector;
};

USTRUCT(meta=(DisplayName="Spawn Entity"))
struct RECALLGAMEPLAY_API FRecallSpawnEntityTask : public FRecallStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallSpawnEntityTaskInstanceData;

protected:
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

	UPROPERTY(EditAnywhere)
	bool bSucceedOnSpawn = true;

private:
	TStateTreeExternalDataHandle<class URecallEntitySubsystem> EntitySystemHandle;
};
