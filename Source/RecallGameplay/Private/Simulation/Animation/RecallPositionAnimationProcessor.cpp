// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallPositionAnimationProcessor.h"

#include "MassExtendedExecutionContext.h"
#include "Simulation/Animation/RecallPositionAnimationFragments.h"
#include "System/Physics/RecallPhysicsSubsystem.h"
#include "Utility/Animation/RecallPositionAnimationUtils.h"
#include "Utility/Physics/RecallPhysicsUtils.h"

// Static helper method for calculating position
static FVector CalculateAnimationPosition(const FRecallPositionAnimationFragment& AnimationFragment)
{
    if (AnimationFragment.GetPhase() == ERecallPositionAnimationPhase::Complete)
    {
        return AnimationFragment.TargetPosition;
    }
    
    // Linear interpolation phase
    FVector NewPosition = FMath::Lerp(AnimationFragment.StartPosition, AnimationFragment.TargetPosition,
        AnimationFragment.GetProgress());
    
    if (AnimationFragment.GetPhase() == ERecallPositionAnimationPhase::Bouncing)
    {        
        // Use the fragment's GetCurveValue method to get bounce curve value
        const float CurveValue = AnimationFragment.GetCurveValue(AnimationFragment.GetBounceProgress());
        
        NewPosition.Z += CurveValue * AnimationFragment.Settings.BounceHeight;
    }

    return NewPosition;
}

URecallPositionAnimationProcessor::URecallPositionAnimationProcessor()
    : EntityQuery(*this)
{
    ExecutionFlags = static_cast<int32>(EExtendedProcessorExecutionFlags::All);
    ProcessingPhase = EMassExtendedProcessingPhase::StartPhysics;
}

void URecallPositionAnimationProcessor::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager)
{
    EntityQuery.AddRequirement<FRecallPositionAnimationFragment>(EMassExtendedFragmentAccess::ReadWrite);
    EntityQuery.AddSubsystemRequirement<URecallPhysicsSubsystem>(EMassExtendedFragmentAccess::ReadWrite);
}

void URecallPositionAnimationProcessor::Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context)
{    
    EntityQuery.ForEachEntityChunk(Context, [&EntityManager](FMassExtendedExecutionContext& Context)
    {
        const float DeltaTime = Context.GetDeltaTimeSeconds();
        URecallPhysicsSubsystem& PhysicsSubsystem = Context.GetMutableSubsystemChecked<URecallPhysicsSubsystem>();
        
        const TArrayView<FRecallPositionAnimationFragment> AnimationFragmentList = 
            Context.GetMutableFragmentView<FRecallPositionAnimationFragment>();
        
        for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); ++EntityIndex)
        {
            const FMassExtendedEntityHandle Entity = Context.GetEntity(EntityIndex);
            FRecallPositionAnimationFragment& AnimationFragment = AnimationFragmentList[EntityIndex];
            
            // Update animation time
            AnimationFragment.ElapsedTime += DeltaTime;
            
            // Calculate new position using static helper method
            const FVector NewPosition = CalculateAnimationPosition(AnimationFragment);
            
            // Update position
            Recall::Physics::Utils::Teleport(EntityManager, Entity, PhysicsSubsystem, NewPosition);
            
            // Mark for cleanup if complete
            if (AnimationFragment.GetPhase() == ERecallPositionAnimationPhase::Complete)
            {
                Recall::Animation::Utils::StopPositionAnimation(Context, Entity);
            }
        }
    });
}
