// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "StateTree/RecallGameRuleStateTreeEvaluatorBase.h"
#include "StateTreeExecutionTypes.h"
#include "Data/GameplayTag/RecallGameplayTagConditionTypes.h"

#include "RecallGameRuleCommonEvaluators.generated.h"

USTRUCT()
struct RECALLGAMERULEMODULE_API FRecallGameRuleMatchStateEvaluatorInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, Category=Output)
	FName MatchState = NAME_None;
};

USTRUCT(meta=(DisplayName="Match State"))
struct RECALLGAMERULEMODULE_API FRecallGameRuleMatchStateEvaluator : public FRecallGameRuleStateTreeEvaluatorBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallGameRuleMatchStateEvaluatorInstanceData;

protected:
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual void TreeStart(FStateTreeExecutionContext& Context) const override;
	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	
protected:
	TStateTreeExternalDataHandle<class URecallGameRuleSubsystem> GameRuleSystemHandle;
};
USTRUCT()
struct RECALLGAMERULEMODULE_API FRecallGameRuleFilterControllerEvaluatorInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category=Parameter)
	bool bFilterPlayerOnly = true;
	
	UPROPERTY(EditAnywhere, Category=Parameter)
	FRecallGameplayTagCondition GameplayTagFilter;
	
	UPROPERTY(VisibleAnywhere, Category=Output)
	int32 FilteredControllerCount = 0;

	UPROPERTY(VisibleAnywhere, Category=Output)
	int32 TotalControllerCount = 0;

	UPROPERTY(VisibleAnywhere, Category=Output)
	int32 TotalPlayerCount = 0;
};

/**
 * Evaluator to filter controller entities.
 */
USTRUCT(meta=(DisplayName="Filter Controller"))
struct RECALLGAMERULEMODULE_API FRecallGameRuleFilterControllerEvaluator : public FRecallGameRuleStateTreeEvaluatorBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallGameRuleFilterControllerEvaluatorInstanceData;

protected:
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual void TreeStart(FStateTreeExecutionContext& Context) const override;
	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	
protected:
	TStateTreeExternalDataHandle<class URecallEntitySubsystem> EntitySystemHandle;
};
