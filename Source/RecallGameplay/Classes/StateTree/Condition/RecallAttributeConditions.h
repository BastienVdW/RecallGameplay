// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "StateTree/RecallStateTreeConditionBase.h"
#include "AITypes.h"
#include "MassExtendedEntityHandle.h"
#include "MassExtendedEntityTypes.h"

#include "RecallAttributeConditions.generated.h"

USTRUCT()
struct RECALLGAMEPLAY_API FRecallStateTreeAttributeCompareConditionInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category=Input)
	FMassExtendedEntityHandle EntityHandle;
	
	UPROPERTY(EditAnywhere, Category=Parameter, meta=(GameplayTagFilter="Attribute"))
	FGameplayTag Left;

	UPROPERTY(EditAnywhere, Category=Parameter)
	float Right = 0.0f;
};
STATETREE_POD_INSTANCEDATA(FRecallStateTreeAttributeCompareConditionInstanceData);

USTRUCT(DisplayName="Attribute Compare")
struct RECALLGAMEPLAY_API FRecallStateTreeAttributeCompareCondition : public FRecallStateTreeConditionBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallStateTreeAttributeCompareConditionInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;

	UPROPERTY(EditAnywhere, Category="Parameter")
	bool bInvert = false;

	UPROPERTY(EditAnywhere, Category="Parameter", meta=(InvalidEnumValues = "IsTrue"))
	EGenericAICheck Operator = EGenericAICheck::Equal;
};
