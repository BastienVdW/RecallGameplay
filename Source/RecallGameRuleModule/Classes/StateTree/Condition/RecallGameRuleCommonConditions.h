// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "StateTree/RecallGameRuleStateTreeConditionBase.h"

#include "RecallGameRuleCommonConditions.generated.h"

USTRUCT()
struct RECALLGAMERULEMODULE_API FRecallGameRuleInProgressConditionInstanceData
{
	GENERATED_BODY()
};
STATETREE_POD_INSTANCEDATA(FRecallGameRuleInProgressConditionInstanceData);

USTRUCT(DisplayName="Match In Progress")
struct RECALLGAMERULEMODULE_API FRecallGameRuleInProgressCondition : public FRecallGameRuleStateTreeConditionBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallGameRuleInProgressConditionInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
	
	UPROPERTY(EditAnywhere, Category=Parameter)
	bool bInvert = false;

private:
	TStateTreeExternalDataHandle<class URecallGameRuleSubsystem> GameRuleSystemHandle;
};
