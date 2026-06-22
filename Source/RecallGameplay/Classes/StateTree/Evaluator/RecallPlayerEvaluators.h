// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "StateTree/Player/RecallPlayerEvaluatorBase.h"
#include "StateTree/RecallStateTreeEvaluatorBase.h"
#include "Mass/EntityElementTypes.h"
#include "StateTreePropertyRef.h"

#include "RecallPlayerEvaluators.generated.h"

USTRUCT()
struct RECALLGAMEPLAY_API FRecallPlayerLocationEvaluatorInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, Category=Output)
	bool bFoundPlayer = false;

	UPROPERTY(VisibleAnywhere, Category=Output)
	FVector PlayerLocation = FVector::ZeroVector;
	
	UPROPERTY(VisibleAnywhere, Category=Output)
	FMassEntityHandle PlayerEntity;
};

/**
 * Find player entity and location. 
 */
USTRUCT(meta=(DisplayName="Find Player"))
struct RECALLGAMEPLAY_API FRecallPlayerLocationEvaluator : public FRecallStateTreeEvaluatorBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallPlayerLocationEvaluatorInstanceData;

protected:
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual void TreeStart(FStateTreeExecutionContext& Context) const override;
	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

protected:
	TStateTreeExternalDataHandle<class URecallEntitySubsystem> EntitySystemHandle;
};

USTRUCT()
struct RECALLGAMEPLAY_API FRecallPlayerAxisEvaluatorInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category=Parameter, meta=(RowType="/Script/RecallCore.RecallInputActionTableRow"))
	FDataTableRowHandle InputAxis;
	
	UPROPERTY(VisibleAnywhere, Category=Output)
	float Value = 0.0f;
};

/**
 * Quick way to access player input
 */
USTRUCT(meta=(DisplayName="Player Input"))
struct RECALLGAMEPLAY_API FRecallPlayerAxisEvaluator : public FRecallPlayerEvaluatorBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallPlayerAxisEvaluatorInstanceData;

protected:
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual void TreeStart(FStateTreeExecutionContext& Context) const override;
	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
		
protected:
	TStateTreeExternalDataHandle<struct FRecallPlayerInputFragment> InputFragmentHandle;
};
