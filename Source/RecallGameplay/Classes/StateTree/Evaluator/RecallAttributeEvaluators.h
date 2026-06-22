// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "StateTree/RecallStateTreeEvaluatorBase.h"
#include "StateTreeExecutionTypes.h"

#include "RecallAttributeEvaluators.generated.h"

USTRUCT()
struct RECALLGAMEPLAY_API FRecallAttributeEvaluatorInstanceData
{
	GENERATED_BODY()

	/**
	 * Attribute to evaluate.
	 */
	UPROPERTY(EditAnywhere, Category=Parameter, meta=(GameplayTagFilter="Attribute"))
	FGameplayTag Attribute;

	/**
	 * Return value for our attribute.
	 */
	UPROPERTY(VisibleAnywhere, Category=Output, meta=(CanRefToArray))
	float Value = 0.0f;
};

/**
* Evaluate an attribute.
*/
USTRUCT(meta=(DisplayName="Attribute"))
struct RECALLGAMEPLAY_API FRecallAttributeEvaluator : public FRecallStateTreeEvaluatorBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallAttributeEvaluatorInstanceData;

protected:
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual void TreeStart(FStateTreeExecutionContext& Context) const override;
	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

protected:
	TStateTreeExternalDataHandle<struct FRecallAttributeFragment> AttributeFragmentHandle;
};
