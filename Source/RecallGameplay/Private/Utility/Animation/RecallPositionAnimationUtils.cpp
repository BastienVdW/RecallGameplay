// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "Utility/Animation/RecallPositionAnimationUtils.h"

#include "MassExtendedEntityView.h"
#include "MassExtendedExecutionContext.h"
#include "Physics/RecallPhysicsObjects.h"
#include "Simulation/Animation/RecallPositionAnimationFragments.h"
#include "Simulation/Physics/RecallPhysicsBodyFragment.h"
#include "System/Physics/RecallPhysicsSubsystem.h"

namespace Recall::Animation::Utils
{
    void StartPositionAnimation(
        FMassExtendedExecutionContext& Context,
        const FMassExtendedEntityHandle& Entity,
        const FVector& StartPosition,
        const FVector& TargetPosition,
        const FRecallPositionAnimationSettings& Settings)
    {
        const FMassExtendedEntityManager& EntityManager = Context.GetEntityManagerChecked();
        if (!EntityManager.IsEntityValid(Entity))
        {
            return;
        }
        
        const FMassExtendedEntityView EntityView(EntityManager, Entity);
        const FRecallPositionAnimationFragment* AnimationFragmentPtr = EntityView.GetFragmentDataPtr<FRecallPositionAnimationFragment>();
        if (AnimationFragmentPtr != nullptr)
        {
            return;
        }
        
        // Deactivate physics body to prevent collisions during animation
        if (const FRecallPhysicsBodyFragment* PhysicsBodyFragment = EntityView.GetFragmentDataPtr<FRecallPhysicsBodyFragment>())
        {
            URecallPhysicsSubsystem* PhysicsSystem = UWorld::GetSubsystem<URecallPhysicsSubsystem>(EntityManager.GetWorld());
            check(PhysicsSystem);
            
            const TWeakPtr<FRecallPhysicsBody> Body = PhysicsSystem->GetMutableBody(PhysicsBodyFragment->BodyHandle);
            if (Body.IsValid())
            {
                Body.Pin()->Desactivate();
            }
        }
        
        // Add fragment via deferred command
        Context.Defer().PushCommand<FMassExtendedDeferredAddCommand>(
            [Entity, StartPosition, TargetPosition, Settings](FMassExtendedEntityManager& EntityManager)
            {
                // Set fragment values after adding (will be done in deferred execution)
                // Note: This is a simplified approach - in a production system you might want to 
                // use a custom command that adds and initializes the fragment in one operation
                EntityManager.AddFragmentToEntity(Entity, FRecallPositionAnimationFragment::StaticStruct(),
                    [StartPosition, TargetPosition, Settings](void* FragmentMemory, const UScriptStruct& FragmentType)
                {
                    FRecallPositionAnimationFragment* AnimationFragment = static_cast<FRecallPositionAnimationFragment*>(FragmentMemory);
                    AnimationFragment->StartPosition = StartPosition;
                    AnimationFragment->TargetPosition = TargetPosition;
                    AnimationFragment->Settings = Settings;
                    AnimationFragment->ElapsedTime = 0.0f;
                });
            }
        );
    }
    
    void StopPositionAnimation(
        FMassExtendedExecutionContext& Context,
        const FMassExtendedEntityHandle& Entity)
    {
        const FMassExtendedEntityManager& EntityManager = Context.GetEntityManagerChecked();
        if (!EntityManager.IsEntityValid(Entity))
        {
            return;
        }
        
        // Reactivate physics body after animation completes
        const FMassExtendedEntityView EntityView(EntityManager, Entity);
        if (const FRecallPhysicsBodyFragment* PhysicsBodyFragment = EntityView.GetFragmentDataPtr<FRecallPhysicsBodyFragment>())
        {
            URecallPhysicsSubsystem* PhysicsSystem = UWorld::GetSubsystem<URecallPhysicsSubsystem>(EntityManager.GetWorld());
            check(PhysicsSystem);
            
            const TWeakPtr<FRecallPhysicsBody> Body = PhysicsSystem->GetMutableBody(PhysicsBodyFragment->BodyHandle);
            if (Body.IsValid())
            {
                Body.Pin()->Activate();
            }
        }

        // Use deferred command to remove fragment and reactivate physics
        Context.Defer().RemoveFragment<FRecallPositionAnimationFragment>(Entity);
    }
}