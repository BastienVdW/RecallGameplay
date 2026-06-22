// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "StateTree/RecallStateTreeTaskBase.h"
#include "Mass/EntityHandle.h"
#include "StateTreePropertyRef.h"

#include "RecallParameterTasks.generated.h"

UENUM()
enum class ERecallSetParameterSource : uint8
{
	Data,
	Payload,
};

USTRUCT()
struct RECALLGAMEPLAY_API FRecallSetEntityParameterTaskInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category=Input)
	TStateTreePropertyRef<FMassEntityHandle> Parameter;
	
	UPROPERTY(EditAnywhere, Category=Parameter)
	ERecallSetParameterSource Source = ERecallSetParameterSource::Data;
	
	UPROPERTY(EditAnywhere, Category=Parameter, meta=(EditCondition="Source == ERecallSetParameterSource::Data", EditConditionHides))
	FMassEntityHandle Value;
	
	UPROPERTY(EditAnywhere, Category=Parameter, meta=(EditCondition="Source == ERecallSetParameterSource::Payload", EditConditionHides), DisplayName="Value")
	FInstancedStruct PayloadValue;
};

/**
 * Copy an entity input into a parameter.
 */
USTRUCT(meta=(DisplayName="Set Entity Parameter"))
struct RECALLGAMEPLAY_API FRecallSetEntityParameterTask : public FRecallStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallSetEntityParameterTaskInstanceData;

	FRecallSetEntityParameterTask();
	
protected:
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	/** This task succeed as soon as we set the entity */
	UPROPERTY(EditAnywhere)
	bool bSucceedOnSet = true;
};

USTRUCT()
struct RECALLGAMEPLAY_API FRecallSetVectorParameterTaskInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category=Input)
	TStateTreePropertyRef<FVector> Parameter;
	
	UPROPERTY(EditAnywhere, Category=Parameter)
	ERecallSetParameterSource Source = ERecallSetParameterSource::Data;
	
	UPROPERTY(EditAnywhere, Category=Parameter, meta=(EditCondition="Source == ERecallSetParameterSource::Data", EditConditionHides))
	FVector Value = FVector::ZeroVector;
	
	UPROPERTY(EditAnywhere, Category=Parameter, meta=(EditCondition="Source == ERecallSetParameterSource::Payload", EditConditionHides), DisplayName="Value")
	FInstancedStruct PayloadValue;
};

/**
 * Copy an vector input into a parameter.
 */
USTRUCT(meta=(DisplayName="Set Vector Parameter"))
struct RECALLGAMEPLAY_API FRecallSetVectorParameterTask : public FRecallStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallSetVectorParameterTaskInstanceData;

	FRecallSetVectorParameterTask();

protected:
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	/** This task succeed as soon as we set the entity */
	UPROPERTY(EditAnywhere)
	bool bSucceedOnSet = true;
};
