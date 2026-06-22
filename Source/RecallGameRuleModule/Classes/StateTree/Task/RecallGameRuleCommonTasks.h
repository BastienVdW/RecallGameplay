// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "StateTree/RecallGameRuleStateTreeTaskBase.h"

#include "RecallGameRuleCommonTasks.generated.h"

USTRUCT()
struct RECALLGAMERULEMODULE_API FRecallGameRuleEndMatchTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category=Parameter)
	FString Reason;
};

/**
 * Task to end the match.
 */
USTRUCT(meta=(DisplayName="End Match"))
struct RECALLGAMERULEMODULE_API FRecallGameRuleEndMatchTask : public FRecallGameRuleStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallGameRuleEndMatchTaskInstanceData;

public:
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

private:
	TStateTreeExternalDataHandle<class URecallRepresentationEventSubsystem> RepresentationEventSystemHandle;
};
