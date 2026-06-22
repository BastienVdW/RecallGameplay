// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "System/Command/RecallGameRuleCommandSubsystem.h"

void URecallGameRuleCommandSubsystem::QueueCommand(const FRecallGameRuleCommandEvent& CommandEvent)
{
    if (!CommandEvent.IsValid())
    {
        return;
    }
    
    FScopeLock Lock(&CommandQueueGuard);
    PendingCommands.Add(CommandEvent);
}

void URecallGameRuleCommandSubsystem::QueueCommands(const TArray<FRecallGameRuleCommandEvent>& CommandEvents)
{
    for (const FRecallGameRuleCommandEvent& CommandEvent : CommandEvents)
    {
        QueueCommand(CommandEvent);
    }
}

TArray<FRecallGameRuleCommandEvent> URecallGameRuleCommandSubsystem::ConsumeCommandsByType(const FGameplayTag& CommandType)
{
    FScopeLock Lock(&CommandQueueGuard);
    
    TArray<FRecallGameRuleCommandEvent> Result;
    
    for (int32 CommandIndex = PendingCommands.Num() - 1; CommandIndex >= 0; --CommandIndex)
    {
        const FRecallGameRuleCommandEvent& Command = PendingCommands[CommandIndex];        
        if (Command.CommandType.MatchesTag(CommandType))
        {
            Result.Add(Command);
            PendingCommands.RemoveAtSwap(CommandIndex);
        }
    }
    
    return Result;
}

TArray<FRecallGameRuleCommandEvent> URecallGameRuleCommandSubsystem::ConsumeAllCommands()
{
    FScopeLock Lock(&CommandQueueGuard);
    
    TArray<FRecallGameRuleCommandEvent> Result = MoveTemp(PendingCommands);
    PendingCommands.Reset();
    return Result;
}

void URecallGameRuleCommandSubsystem::Reset()
{
    FScopeLock Lock(&CommandQueueGuard);
    
    // This should NEVER happen - if commands remain during reset, it's a critical bug in processor ordering
    checkf(PendingCommands.IsEmpty(), 
        TEXT("%hs: Found %d unprocessed commands during reset! This indicates a critical bug in processor ordering. Commands must be consumed before reset."), 
        __FUNCTION__, PendingCommands.Num());
}

void URecallGameRuleCommandSubsystem::Save(const FRecallSnapshotContext& Context, FInstancedStruct& OutSnapshot)
{
    FScopeLock Lock(&CommandQueueGuard);
    
    // This should NEVER happen - if commands remain during save, it's a critical bug in processor ordering
    checkf(PendingCommands.IsEmpty(), 
        TEXT("%hs: Found %d unprocessed commands during save! This indicates a critical bug in processor ordering. Commands must be consumed before save."), 
        __FUNCTION__, PendingCommands.Num());
}
