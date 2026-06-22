// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallGameRuleCommandTask.h"

#include "RecallSignalSubsystem.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "Simulation/StateTree/RecallStateTreeSignalTypes.h"
#include "StateTree/RecallStateTreeExecutionContext.h"
#include "System/Command/RecallGameRuleCommandSubsystem.h"


FRecallGameRuleCommandTask::FRecallGameRuleCommandTask()
{
    bShouldStateChangeOnReselect = false;
}

bool FRecallGameRuleCommandTask::Link(FStateTreeLinker& Linker)
{
    Linker.LinkExternalData(CommandSystemHandle);
    return true;
}

EStateTreeRunStatus FRecallGameRuleCommandTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
    FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
    
    // Create command event from instance data
    FRecallGameRuleCommandEvent CommandEvent;
    CommandEvent.CommandType = InstanceData.CommandType;
    CommandEvent.Payload = InstanceData.Payload;
    
    if (!CommandEvent.IsValid())
    {
        return EStateTreeRunStatus::Failed;
    }
    
    // Cache the command for potential repeating
    InstanceData.CachedCommand = CommandEvent;
    
    // Queue the initial command
    URecallGameRuleCommandSubsystem& CommandSystem = Context.GetExternalData(CommandSystemHandle);
    CommandSystem.QueueCommand(CommandEvent);

    // Return Running if we need to repeat, Succeeded if one-time execution
    if (bRepeatEveryTick)
    {
		FRecallStateTreeExecutionContext& RecallContext = static_cast<FRecallStateTreeExecutionContext&>(Context);        
        RecallContext.GetSignalSystem().SignalEntity(
            Recall::StateTree::Signals::TickRequired, RecallContext.GetEntity());
		return Super::EnterState(Context, Transition);
    }
    else
    {
        return EStateTreeRunStatus::Succeeded;
    }
}

EStateTreeRunStatus FRecallGameRuleCommandTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
    const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
    
    // Re-queue the cached command
    if (bRepeatEveryTick)
    {
        URecallGameRuleCommandSubsystem& CommandSystem = Context.GetExternalData(CommandSystemHandle);
        CommandSystem.QueueCommand(InstanceData.CachedCommand);
        
        FRecallStateTreeExecutionContext& RecallContext = static_cast<FRecallStateTreeExecutionContext&>(Context);        
        RecallContext.GetSignalSystem().SignalEntity(
            Recall::StateTree::Signals::TickRequired, RecallContext.GetEntity());
    }
    
    return Super::Tick(Context, DeltaTime);
}

void FRecallGameRuleCommandTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
    FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
    
    // Clear cached command to prevent memory leaks
    InstanceData.CachedCommand = FRecallGameRuleCommandEvent();
}
