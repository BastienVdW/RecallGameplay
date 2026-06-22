// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "RecallGameRuleCommands.h"
#include "NativeGameplayTags.h"
#include "Data/GameplayTag/RecallGameplayTagConditionTypes.h"
#include "Mass/EntityHandle.h"
#include "Simulation/Animation/RecallPositionAnimationFragments.h"

#include "RecallGameRuleCommonCommands.generated.h"

// Gameplay tags for common commands
RECALLGAMERULEMODULE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Command_GameRule_DestroyEntities);
RECALLGAMERULEMODULE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Command_GameRule_ApplyTag);
RECALLGAMERULEMODULE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Command_GameRule_SwapPositions);
RECALLGAMERULEMODULE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Command_GameRule_MoveToPosition);

/**
 * Command for destroying entities based on tag conditions
 */
USTRUCT(BlueprintType, DisplayName="Destroy Entities")
struct RECALLGAMERULEMODULE_API FRecallGameRuleDestroyEntitiesCommand : public FRecallGameRuleCommandBase
{
    GENERATED_BODY()
    
    // Tag condition for filtering entities
    UPROPERTY(EditAnywhere, Category="Command", meta=(ShowOnlyInnerProperties))
    FRecallGameplayTagCondition TagCondition;
};

/**
 * Command for applying gameplay tags to entities
 */
USTRUCT(BlueprintType, DisplayName="Apply Tag")
struct RECALLGAMERULEMODULE_API FRecallGameRuleApplyTagCommand : public FRecallGameRuleCommandBase
{
    GENERATED_BODY()
    
    // Tag condition for filtering entities
    UPROPERTY(EditAnywhere, Category="Command", meta=(ShowOnlyInnerProperties))
    FRecallGameplayTagCondition TagCondition;
    
    // Tags to add to matching entities
    UPROPERTY(EditAnywhere, Category="Command")
    FGameplayTagContainer TagsToAdd;
    
    // Tags to remove from matching entities
    UPROPERTY(EditAnywhere, Category="Command")
    FGameplayTagContainer TagsToRemove;
};

/**
 * Command for swapping positions of entities randomly
 */
USTRUCT(BlueprintType, DisplayName="Swap Positions")
struct RECALLGAMERULEMODULE_API FRecallGameRuleSwapPositionsCommand : public FRecallGameRuleCommandBase
{
    GENERATED_BODY()
    
    // Tag condition for filtering entities to swap
    UPROPERTY(EditAnywhere, Category="Command", meta=(ShowOnlyInnerProperties))
    FRecallGameplayTagCondition TagCondition;
    
    // Whether to animate the swap instead of instant teleport
    UPROPERTY(EditAnywhere, Category="Animation")
    bool bAnimateSwap = false;
    
    // Animation settings (only used if bAnimateSwap is true)
    UPROPERTY(EditAnywhere, Category="Animation", meta=(EditCondition="bAnimateSwap", ShowOnlyInnerProperties))
    FRecallPositionAnimationSettings AnimationSettings;
};

/**
 * Command for moving entities to a specific position (e.g., Mouse Monopoly bonus)
 */
USTRUCT(BlueprintType, DisplayName="Move To Position")
struct RECALLGAMERULEMODULE_API FRecallGameRuleMoveToPositionCommand : public FRecallGameRuleCommandBase
{
    GENERATED_BODY()
    
    // Tag condition for filtering entities to move
    UPROPERTY(EditAnywhere, Category="Command", meta=(ShowOnlyInnerProperties))
    FRecallGameplayTagCondition TagCondition;
    
    // Target entity whose position will be used as destination
    UPROPERTY(EditAnywhere, Category="Command")
    FMassEntityHandle TargetEntity;
    
    // Whether to animate the movement instead of instant teleport
    UPROPERTY(EditAnywhere, Category="Animation")
    bool bAnimateMovement = false;
    
    // Animation settings (only used if bAnimateMovement is true)
    UPROPERTY(EditAnywhere, Category="Animation", meta=(EditCondition="bAnimateMovement", ShowOnlyInnerProperties))
    FRecallPositionAnimationSettings AnimationSettings;
};
