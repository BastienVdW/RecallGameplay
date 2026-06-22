// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"

/**
 * The blackboard is used to set or get temporary variables.
 * These values should not persist between frames.
 */
struct FRecallCrowdAgentBlackboard
{
    /**
     * Radius of the agent cylinder.
     */
    float CylinderRadius = 0.0f;
    
    /**
     * Half height of the agent cylinder.
     */
    float CylinderHalfHeight = 0.0f;
    
    /**
     * Maximum speed that the agent can reach.
     */
    float MaxSpeedCentimetersPerSecond = 0.0f;
    
    /**
     * Location of the agent.
     */
    FVector Location = FVector::ZeroVector;
    bool bDirtyLocation = false;
    
    /**
     * Velocity of the agent.
     */
    FVector VelocityCentimetersPerSecond = FVector::ZeroVector;
    bool bDirtyVelocity = false;

    FORCEINLINE bool HasChanged() const { return HasPositionChanged() || HasVelocityChanged(); }

    FORCEINLINE bool HasPositionChanged() const { return bDirtyLocation; }
    FORCEINLINE bool HasVelocityChanged() const { return bDirtyVelocity; }
};
