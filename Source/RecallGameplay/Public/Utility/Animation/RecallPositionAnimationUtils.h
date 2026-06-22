// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "MassExtendedEntityHandle.h"

struct FMassExtendedExecutionContext;
struct FRecallPositionAnimationSettings;

namespace Recall::Animation::Utils
{
    /**
     * Start a position animation for an entity
     * @param Context The execution context for deferred commands
     * @param Entity The entity to animate
     * @param StartPosition The starting position
     * @param TargetPosition The target position to animate to
     * @param Settings Animation settings (duration, curve, etc.)
     */
    RECALLGAMEPLAY_API void StartPositionAnimation(
        FMassExtendedExecutionContext& Context,
        const FMassExtendedEntityHandle& Entity,
        const FVector& StartPosition,
        const FVector& TargetPosition,
        const FRecallPositionAnimationSettings& Settings
    );
    
    /**
     * Stop a position animation for an entity (removes fragment)
     * @param Context The execution context for deferred commands
     * @param Entity The entity to stop animating
     */
    RECALLGAMEPLAY_API void StopPositionAnimation(
        FMassExtendedExecutionContext& Context,
        const FMassExtendedEntityHandle& Entity
    );
}