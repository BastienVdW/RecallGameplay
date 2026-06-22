// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "System/Interface/RecallSimulationReactSystemInterface.h"
#include "RecallGameRuleCommands.h"

#include "RecallGameRuleCommandSubsystem.generated.h"

/**
 * World subsystem for queuing game rule commands
 * Thread-safe command queue that processors can consume
 * Implements simulation react interface for save validation
 */
UCLASS()
class RECALLGAMERULEMODULE_API URecallGameRuleCommandSubsystem : public UWorldSubsystem, public IRecallSimulationReactSystemInterface
{
    GENERATED_BODY()

public:
    // Queue a single command event
    void QueueCommand(const FRecallGameRuleCommandEvent& CommandEvent);
    
    // Queue multiple command events
    void QueueCommands(const TArray<FRecallGameRuleCommandEvent>& CommandEvents);
    
    // Get and remove commands of a specific type
    TArray<FRecallGameRuleCommandEvent> ConsumeCommandsByType(const FGameplayTag& CommandType);
    
    // Get and remove all pending commands
    TArray<FRecallGameRuleCommandEvent> ConsumeAllCommands();
    
    // Get command payloads directly - extracts and validates command payloads of a specific type
    template<typename TCommandType>
    TArray<TCommandType> ConsumeCommandPayloads(const FGameplayTag& CommandType)
    {
        const TArray<FRecallGameRuleCommandEvent> Commands = ConsumeCommandsByType(CommandType);
        
        if (Commands.IsEmpty())
        {
            return {};
        }
        
        TArray<TCommandType> ValidCommands;
        ValidCommands.Reserve(Commands.Num());
        
        for (const FRecallGameRuleCommandEvent& CommandEvent : Commands)
        {
            if (const TCommandType* Command = CommandEvent.Payload.GetPtr<TCommandType>())
            {
                ValidCommands.Add(*Command);  // Copy the command value, not the pointer
            }
        }
        
        return ValidCommands;
    }

    // IRecallSimulationReactSystemInterface implementation Begin
    virtual void Reset() override;
    virtual void Save(const FRecallSnapshotContext& Context, FInstancedStruct& OutSnapshot) override;
    // IRecallSimulationReactSystemInterface implementation End

private:
    // Thread-safe command queue
    TArray<FRecallGameRuleCommandEvent> PendingCommands;
    mutable FCriticalSection CommandQueueGuard;
};

template<>
struct TMassExternalSubsystemTraits<URecallGameRuleCommandSubsystem> final
{
	enum
	{
		GameThreadOnly = false
	};
};
