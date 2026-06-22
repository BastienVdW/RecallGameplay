// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallGameRuleCommonCommandProcessors.h"

#include "Desync/RecallDesyncLog.h"
#include "GameplayTag/RecallGameplayTagTypes.h"
#include "MassExtendedEntityView.h"
#include "MassExtendedExecutionContext.h"
#include "Simulation/GameplayTag/RecallGameplayTagFragments.h"
#include "Simulation/Physics/RecallPhysicsBodyFragment.h"
#include "Simulation/Transform/RecallTransformFragments.h"
#include "System/Command/RecallGameRuleCommonCommands.h"
#include "System/Command/RecallGameRuleCommandSubsystem.h"
#include "System/Physics/RecallPhysicsSubsystem.h"
#include "System/Random/RecallRandomNumberSubsystem.h"
#include "Utility/Animation/RecallPositionAnimationUtils.h"
#include "Utility/GameplayTag/RecallGameplayTagUtils.h"
#include "Utility/Physics/RecallPhysicsUtils.h"

//----------------------------------------------------------------------//
// URecallGameRuleDestroyEntitiesProcessor
//----------------------------------------------------------------------//
URecallGameRuleDestroyEntitiesProcessor::URecallGameRuleDestroyEntitiesProcessor()
    : EntityQuery(*this)
{
    ExecutionFlags = static_cast<int32>(EExtendedProcessorExecutionFlags::All);
    ProcessingPhase = EMassExtendedProcessingPhase::PostPhysics;
}

void URecallGameRuleDestroyEntitiesProcessor::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager)
{
    EntityQuery.AddRequirement<FRecallGameplayTagFragment>(EMassExtendedFragmentAccess::ReadOnly);
    
    ProcessorRequirements.AddSubsystemRequirement<URecallGameRuleCommandSubsystem>(EMassExtendedFragmentAccess::ReadWrite);
}

void URecallGameRuleDestroyEntitiesProcessor::Execute(FMassExtendedEntityManager& EntityManager, 
    FMassExtendedExecutionContext& Context)
{
    URecallGameRuleCommandSubsystem& CommandSubsystem = Context.GetMutableSubsystemChecked<URecallGameRuleCommandSubsystem>();
    
    // Get all destroy commands
    const TArray<FRecallGameRuleDestroyEntitiesCommand> ValidCommands = 
        CommandSubsystem.ConsumeCommandPayloads<FRecallGameRuleDestroyEntitiesCommand>(
            TAG_Command_GameRule_DestroyEntities);
    
    if (ValidCommands.IsEmpty())
    {
        return;
    }
    
    EntityQuery.ForEachEntityChunk(Context, [&ValidCommands](FMassExtendedExecutionContext& Context)
    {
        const TConstArrayView<FRecallGameplayTagFragment> GameplayTagList = 
            Context.GetFragmentView<FRecallGameplayTagFragment>();
        
        for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); ++EntityIndex)
        {
            const FRecallGameplayTagFragment& GameplayTagFragment = GameplayTagList[EntityIndex];

            // Check all commands for this entity
            for (const FRecallGameRuleDestroyEntitiesCommand& Command : ValidCommands)
            {
                if (Recall::GameplayTag::Utils::EvaluateCondition(
                    Command.TagCondition, GameplayTagFragment.GameplayTagCountMap))
                {
                    const FMassExtendedEntityHandle Entity = Context.GetEntity(EntityIndex);
                    Context.Defer().DestroyEntity(Entity);

#if RECALL_DESYNC_LOG
                    RECALL_DESYNC_LOG_INT(Context.GetWorld(), "GameRuleDestroy_EntityDestroyed", Entity.Index);
#endif
                    break; // Entity already marked for destruction, no need to check other commands
                }
            }
        }
    });
}

//----------------------------------------------------------------------//
// URecallGameRuleApplyTagProcessor
//----------------------------------------------------------------------//
URecallGameRuleApplyTagProcessor::URecallGameRuleApplyTagProcessor()
    : EntityQuery(*this)
{
    ExecutionFlags = static_cast<int32>(EExtendedProcessorExecutionFlags::All);
    ProcessingPhase = EMassExtendedProcessingPhase::PostPhysics;
}

void URecallGameRuleApplyTagProcessor::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager)
{
    EntityQuery.AddRequirement<FRecallGameplayTagFragment>(EMassExtendedFragmentAccess::ReadWrite);
    
    ProcessorRequirements.AddSubsystemRequirement<URecallGameRuleCommandSubsystem>(EMassExtendedFragmentAccess::ReadWrite);
}

void URecallGameRuleApplyTagProcessor::Execute(FMassExtendedEntityManager& EntityManager, 
    FMassExtendedExecutionContext& Context)
{
    URecallGameRuleCommandSubsystem& CommandSubsystem = Context.GetMutableSubsystemChecked<URecallGameRuleCommandSubsystem>();
    
    // Get all apply tag commands
    const TArray<FRecallGameRuleApplyTagCommand> ValidCommands = 
        CommandSubsystem.ConsumeCommandPayloads<FRecallGameRuleApplyTagCommand>(
            TAG_Command_GameRule_ApplyTag);
    
    if (ValidCommands.IsEmpty())
    {
        return;
    }
    
    // Apply tag changes - iterate through entities once
    EntityQuery.ForEachEntityChunk(Context, [&ValidCommands](FMassExtendedExecutionContext& Context)
    {
        const TArrayView<FRecallGameplayTagFragment> GameplayTagList = 
            Context.GetMutableFragmentView<FRecallGameplayTagFragment>();
        
        for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); ++EntityIndex)
        {
            FRecallGameplayTagFragment& GameplayTagFragment = GameplayTagList[EntityIndex];

            // Check all commands for this entity
            for (const FRecallGameRuleApplyTagCommand& Command : ValidCommands)
            {
                if (Recall::GameplayTag::Utils::EvaluateCondition(
                    Command.TagCondition, GameplayTagFragment.GameplayTagCountMap))
                {
                    // Apply tag changes
                    GameplayTagFragment.GameplayTagCountMap.AddTags(Command.TagsToAdd);
                    GameplayTagFragment.GameplayTagCountMap.RemoveTags(Command.TagsToRemove);

#if RECALL_DESYNC_LOG
                    const FMassExtendedEntityHandle Entity = Context.GetEntity(EntityIndex);
                    RECALL_DESYNC_LOG_INT(Context.GetWorld(), "GameRuleApplyTag_TagsModified", Entity.Index);
#endif
                    
                    // Note: We don't break here because multiple commands might apply to the same entity
                    // and we want to apply all matching tag changes
                }
            }
        }
    });
}

//----------------------------------------------------------------------//
// URecallGameRuleSwapPositionsProcessor
//----------------------------------------------------------------------//
URecallGameRuleSwapPositionsProcessor::URecallGameRuleSwapPositionsProcessor()
    : EntityQuery(*this)
{
    ExecutionFlags = static_cast<int32>(EExtendedProcessorExecutionFlags::All);
    ProcessingPhase = EMassExtendedProcessingPhase::PostPhysics;
}

void URecallGameRuleSwapPositionsProcessor::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager)
{
    EntityQuery.AddRequirement<FRecallGameplayTagFragment>(EMassExtendedFragmentAccess::ReadOnly);
    EntityQuery.AddRequirement<FRecallTransformFragment>(EMassExtendedFragmentAccess::ReadWrite);
    EntityQuery.AddRequirement<FRecallPhysicsBodyFragment>(EMassExtendedFragmentAccess::ReadOnly, EMassExtendedFragmentPresence::Optional);
    
    ProcessorRequirements.AddSubsystemRequirement<URecallPhysicsSubsystem>(EMassExtendedFragmentAccess::ReadWrite);
    ProcessorRequirements.AddSubsystemRequirement<URecallRandomNumberSubsystem>(EMassExtendedFragmentAccess::ReadWrite);
    ProcessorRequirements.AddSubsystemRequirement<URecallGameRuleCommandSubsystem>(EMassExtendedFragmentAccess::ReadWrite);
}

void URecallGameRuleSwapPositionsProcessor::Execute(FMassExtendedEntityManager& EntityManager, 
    FMassExtendedExecutionContext& Context)
{
    URecallGameRuleCommandSubsystem& CommandSubsystem = Context.GetMutableSubsystemChecked<URecallGameRuleCommandSubsystem>();
    
    // Get all swap positions commands
    const TArray<FRecallGameRuleSwapPositionsCommand> ValidCommands = 
        CommandSubsystem.ConsumeCommandPayloads<FRecallGameRuleSwapPositionsCommand>(
            TAG_Command_GameRule_SwapPositions);
    
    if (ValidCommands.IsEmpty())
    {
        return;
    }
    
    const FRecallGameRuleSwapPositionsCommand& Command = ValidCommands[0];
    
    // Collect entities first
    TArray<FMassExtendedEntityHandle> MatchingEntities;
    TArray<FVector> Positions;
    
    // Collect entities and their positions that match each command's tag condition
    EntityQuery.ForEachEntityChunk(Context,
        [&Command, &MatchingEntities, &Positions](FMassExtendedExecutionContext& Context)
    {
        const TConstArrayView<FRecallGameplayTagFragment> GameplayTagList = 
            Context.GetFragmentView<FRecallGameplayTagFragment>();
        const TConstArrayView<FRecallTransformFragment> TransformList = 
            Context.GetFragmentView<FRecallTransformFragment>();
        
        for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); ++EntityIndex)
        {
            const FRecallGameplayTagFragment& GameplayTagFragment = GameplayTagList[EntityIndex];
            
            if (Recall::GameplayTag::Utils::EvaluateCondition(
                Command.TagCondition, GameplayTagFragment.GameplayTagCountMap))
            {
                MatchingEntities.Add(Context.GetEntity(EntityIndex));
                Positions.Add(TransformList[EntityIndex].GetTransform().GetLocation());
            }
        }
    });
    
    // Only swap if we have at least 2 entities
    if (MatchingEntities.Num() < 2)
    {
        return;
    }
    
    URecallRandomNumberSubsystem& RandomSystem = Context.GetMutableSubsystemChecked<URecallRandomNumberSubsystem>();
    
    // Generate derangement directly on positions array to ensure no entity stays at original position
    TArray<FVector> ShuffledPositions = Positions;
    RandomSystem.GenerateDerangement(ShuffledPositions);
    
    URecallPhysicsSubsystem& PhysicsSubsystem = Context.GetMutableSubsystemChecked<URecallPhysicsSubsystem>();
    
    // Apply shuffled positions to entities
    for (int32 EntityIndex = 0; EntityIndex < MatchingEntities.Num(); ++EntityIndex)
    {
        const FMassExtendedEntityHandle& Entity = MatchingEntities[EntityIndex];
        const FVector& NewPosition = ShuffledPositions[EntityIndex];
        const FVector& CurrentPosition = Positions[EntityIndex];
        
        // Check if we should animate
#if RECALL_DESYNC_LOG
        RECALL_DESYNC_LOG_VEC(Context.GetWorld(), "GameRuleSwapPositions_NewPosition", NewPosition);
        RECALL_DESYNC_LOG_INT(Context.GetWorld(), "GameRuleSwapPositions_Entity", Entity.Index);
#endif
        
        if (Command.bAnimateSwap)
        {
            // Start position animation using utility
            Recall::Animation::Utils::StartPositionAnimation(
                Context,
                Entity,
                CurrentPosition,
                NewPosition,
                Command.AnimationSettings
            );
        }
        else
        {
            // Use instant teleport
            Recall::Physics::Utils::Teleport(EntityManager, Entity, PhysicsSubsystem, NewPosition);
        }
    }
}

//----------------------------------------------------------------------//
// URecallGameRuleMoveToPositionProcessor
//----------------------------------------------------------------------//
URecallGameRuleMoveToPositionProcessor::URecallGameRuleMoveToPositionProcessor()
    : EntityQuery(*this)
{
    ExecutionFlags = static_cast<int32>(EExtendedProcessorExecutionFlags::All);
    ProcessingPhase = EMassExtendedProcessingPhase::StartPhysics;
}

void URecallGameRuleMoveToPositionProcessor::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager)
{
    EntityQuery.AddRequirement<FRecallGameplayTagFragment>(EMassExtendedFragmentAccess::ReadOnly);
    EntityQuery.AddRequirement<FRecallTransformFragment>(EMassExtendedFragmentAccess::ReadWrite);
    EntityQuery.AddRequirement<FRecallPhysicsBodyFragment>(EMassExtendedFragmentAccess::ReadOnly, EMassExtendedFragmentPresence::Optional);
    EntityQuery.AddSubsystemRequirement<URecallPhysicsSubsystem>(EMassExtendedFragmentAccess::ReadWrite);
    
    ProcessorRequirements.AddSubsystemRequirement<URecallGameRuleCommandSubsystem>(EMassExtendedFragmentAccess::ReadWrite);
}

static bool GetTargetEntityPosition(
    const FMassExtendedEntityManager& EntityManager,
    const FMassExtendedEntityHandle& TargetEntity,
    FVector& OutPosition)
{
    if (!EntityManager.IsEntityValid(TargetEntity))
    {
        return false;
    }
    
    const FMassExtendedEntityView TargetEntityView(EntityManager, TargetEntity);
    const FRecallTransformFragment* TransformFragmentPtr = TargetEntityView.GetFragmentDataPtr<FRecallTransformFragment>();    
    if (!TransformFragmentPtr)
    {
        return false;
    }
    
    OutPosition = TransformFragmentPtr->Position;
    return true;
}

void URecallGameRuleMoveToPositionProcessor::Execute(FMassExtendedEntityManager& EntityManager, 
    FMassExtendedExecutionContext& Context)
{
    URecallGameRuleCommandSubsystem& CommandSubsystem = Context.GetMutableSubsystemChecked<URecallGameRuleCommandSubsystem>();
    
    // Get all move to position commands
    const TArray<FRecallGameRuleMoveToPositionCommand> ValidCommands = 
        CommandSubsystem.ConsumeCommandPayloads<FRecallGameRuleMoveToPositionCommand>(
            TAG_Command_GameRule_MoveToPosition);
    
    if (ValidCommands.IsEmpty())
    {
        return;
    }
    
    // Collect entities that match the tag condition and move them to target position
    EntityQuery.ForEachEntityChunk(Context,
        [&ValidCommands, &EntityManager](FMassExtendedExecutionContext& Context)
    {
        URecallPhysicsSubsystem& PhysicsSubsystem = Context.GetMutableSubsystemChecked<URecallPhysicsSubsystem>();
            
        const TConstArrayView<FRecallGameplayTagFragment> GameplayTagList = 
            Context.GetFragmentView<FRecallGameplayTagFragment>();
        const TConstArrayView<FRecallTransformFragment> TransformList = 
            Context.GetFragmentView<FRecallTransformFragment>();
        
        for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); ++EntityIndex)
        {
            const FRecallGameplayTagFragment& GameplayTagFragment = GameplayTagList[EntityIndex];
            
            const FRecallGameRuleMoveToPositionCommand& Command = ValidCommands[0];
            if (!Recall::GameplayTag::Utils::EvaluateCondition(
                Command.TagCondition, GameplayTagFragment.GameplayTagCountMap))
            {
                continue;
            }
            
            const FMassExtendedEntityHandle Entity = Context.GetEntity(EntityIndex);
            const FRecallTransformFragment& TransformFragment = TransformList[EntityIndex];
            
            // Get target position from the target entity (e.g., the escape/base entity)
            FVector TargetPosition = FVector::ZeroVector;
            if (!GetTargetEntityPosition(EntityManager, Command.TargetEntity, TargetPosition))
            {
                continue; // Skip if we can't get target position
            }
            
#if RECALL_DESYNC_LOG
            RECALL_DESYNC_LOG_VEC(Context.GetWorld(), "GameRuleMoveToPosition_TargetPosition", TargetPosition);
            RECALL_DESYNC_LOG_INT(Context.GetWorld(), "GameRuleMoveToPosition_Entity", Entity.Index);
#endif
            
            // Check if we should animate
            if (Command.bAnimateMovement)
            {
                // Start position animation using utility
                Recall::Animation::Utils::StartPositionAnimation(
                    Context,
                    Entity,
                    TransformFragment.Position,
                    TargetPosition,
                    Command.AnimationSettings
                );
            }
            else
            {
                // Use instant teleport
                Recall::Physics::Utils::Teleport(EntityManager, Entity, PhysicsSubsystem, TargetPosition);
            }
        }
    });
}
