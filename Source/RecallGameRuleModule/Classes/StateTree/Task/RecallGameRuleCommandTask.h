// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "StateTree/RecallGameRuleStateTreeTaskBase.h"
#include "System/Command/RecallGameRuleCommands.h"
#include "GameplayTagContainer.h"
#include "InstancedStruct.h"

#include "RecallGameRuleCommandTask.generated.h"

USTRUCT()
struct RECALLGAMERULEMODULE_API FRecallGameRuleCommandTaskInstanceData
{
    GENERATED_BODY()
    
    // Command type for dispatch
    UPROPERTY(EditAnywhere, Category=Parameter, meta=(Categories="Command.GameRule"))
    FGameplayTag CommandType;
    
    // Polymorphic command payload
    UPROPERTY(EditAnywhere, Category=Parameter, meta=(BaseStruct="/Script/RecallGameRuleModule.RecallGameRuleCommandBase", ExcludeBaseStruct))
    FInstancedStruct Payload;
    
    // Cached command (set on enter, used on tick) - UPROPERTY for rollback
    UPROPERTY()
    FRecallGameRuleCommandEvent CachedCommand;
};

/**
 * Task that generates and queues a game rule command event on enter
 * with optional continuous re-queuing on tick
 */
USTRUCT(meta=(DisplayName="Game Rule Command"))
struct RECALLGAMERULEMODULE_API FRecallGameRuleCommandTask : public FRecallGameRuleStateTreeTaskBase
{
    GENERATED_BODY()
    
    using FInstanceDataType = FRecallGameRuleCommandTaskInstanceData;
    
public:
    FRecallGameRuleCommandTask();
    
protected:
    virtual bool Link(FStateTreeLinker& Linker) override;
    virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
    virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
    virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
    virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
    
    // Enable continuous re-queuing on tick
    UPROPERTY(EditAnywhere, Category=Parameter)
    bool bRepeatEveryTick = false;
    
private:
    // Handle to command subsystem
    TStateTreeExternalDataHandle<class URecallGameRuleCommandSubsystem> CommandSystemHandle;
};
