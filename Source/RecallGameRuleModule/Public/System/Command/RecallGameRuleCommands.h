// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
#include "GameplayTagContainer.h"

#include "RecallGameRuleCommands.generated.h"

/**
 * Base struct for all game rule commands
 * Empty struct used for type safety with FInstancedStruct
 */
USTRUCT()
struct RECALLGAMERULEMODULE_API FRecallGameRuleCommandBase
{
    GENERATED_BODY()
    
    virtual ~FRecallGameRuleCommandBase() = default;
};

/**
 * Command event that contains a gameplay tag and polymorphic payload
 */
USTRUCT(BlueprintType)
struct RECALLGAMERULEMODULE_API FRecallGameRuleCommandEvent
{
    GENERATED_BODY()
    
    // Command type for dispatch
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Command", meta=(Categories="Command.GameRule"))
    FGameplayTag CommandType;
    
    // Polymorphic command payload
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Command", meta=(BaseStruct="/Script/RecallGameRuleModule.RecallGameRuleCommandBase", ExcludeBaseStruct))
    FInstancedStruct Payload;
    
    // Convenience factory method
    template<typename T>
    static FRecallGameRuleCommandEvent Make(const FGameplayTag& InCommandType, const T& InPayload)
    {
        static_assert(TIsDerivedFrom<T, FRecallGameRuleCommandBase>::Value, 
            "Payload must derive from FRecallGameRuleCommandBase");
        
        FRecallGameRuleCommandEvent Event;
        Event.CommandType = InCommandType;
        Event.Payload.InitializeAs<T>(InPayload);
        return Event;
    }

    FORCEINLINE bool IsValid() const
    {
        return CommandType.IsValid() && Payload.IsValid();
    }
};
