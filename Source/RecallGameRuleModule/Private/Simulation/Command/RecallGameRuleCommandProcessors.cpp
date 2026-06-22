// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallGameRuleCommandProcessors.h"

#include "MassExecutionContext.h"
#include "System/Command/RecallGameRuleCommandSubsystem.h"

//----------------------------------------------------------------------//
// URecallGameRuleCommandCleanupProcessor
//----------------------------------------------------------------------//
URecallGameRuleCommandCleanupProcessor::URecallGameRuleCommandCleanupProcessor()
{
    ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::All);
    ProcessingPhase = EMassProcessingPhase::FrameEnd; // Run at the very end of the frame
}

void URecallGameRuleCommandCleanupProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    // This processor doesn't need any entity queries - it only works with the command subsystem
    ProcessorRequirements.AddSubsystemRequirement<URecallGameRuleCommandSubsystem>(EMassFragmentAccess::ReadWrite);
}

void URecallGameRuleCommandCleanupProcessor::Execute(FMassEntityManager& EntityManager, 
    FMassExecutionContext& Context)
{
    URecallGameRuleCommandSubsystem& CommandSubsystem = Context.GetMutableSubsystemChecked<URecallGameRuleCommandSubsystem>();
    
    // Get all remaining commands and log them as warnings
    TArray<FRecallGameRuleCommandEvent> RemainingCommands = CommandSubsystem.ConsumeAllCommands();
    
    if (RemainingCommands.IsEmpty())
    {
        return;
    }
    
    UE_LOG(LogTemp, Log, 
        TEXT("%hs: Found %d unprocessed commands."), 
        __FUNCTION__, RemainingCommands.Num());
    
    // Log details about each unprocessed command for debugging
    for (int32 i = 0; i < RemainingCommands.Num(); ++i)
    {
        const FRecallGameRuleCommandEvent& Command = RemainingCommands[i];
        UE_LOG(LogTemp, Verbose, 
            TEXT("%hs: Unprocessed Command %d: Type=%s, PayloadType=%s"), 
            __FUNCTION__, i, 
            *Command.CommandType.ToString(),
            Command.Payload.IsValid() ? *Command.Payload.GetScriptStruct()->GetName() : TEXT("Invalid"));
    }
    
    // Commands are automatically discarded when RemainingCommands goes out of scope
    // This prevents them from persisting to the next frame
}