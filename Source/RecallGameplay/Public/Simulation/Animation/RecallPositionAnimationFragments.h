// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Mass/EntityElementTypes.h"

// Forward declarations
class UCurveFloat;

#include "RecallPositionAnimationFragments.generated.h"

UENUM()
enum class ERecallPositionAnimationPhase : uint8
{
    Moving,
    Bouncing,
    Complete
};

/**
 * Settings for position animation
 */
USTRUCT(BlueprintType)
struct RECALLGAMEPLAY_API FRecallPositionAnimationSettings
{
    GENERATED_BODY()

    // Total duration of the animation in seconds
    UPROPERTY(EditAnywhere, Category="Animation", meta=(ClampMin="0.1", ClampMax="10.0"))
    float Duration = 1.5f;
    
    // When bounce phase starts (0-1 progress)
    UPROPERTY(EditAnywhere, Category="Animation", meta=(ClampMin="0.0", ClampMax="1.0"))
    float BounceStartProgress = 0.7f;
    
    // Optional curve for vertical bounce at end
    UPROPERTY(EditAnywhere, Category="Animation")
    UCurveFloat* BounceCurve = nullptr;
    
    // Maximum height of the bounce
    UPROPERTY(EditAnywhere, Category="Animation", meta=(ClampMin="0.0", ClampMax="1000.0"))
    float BounceHeight = 100.0f;
};

/**
 * Fragment for animating entity positions over time with optional end bounce
 * This fragment is temporary and will be removed when animation completes
 */
USTRUCT()
struct RECALLGAMEPLAY_API FRecallPositionAnimationFragment : public FMassFragment
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere)
    FVector StartPosition = FVector::ZeroVector;

    UPROPERTY(VisibleAnywhere)
    FVector TargetPosition = FVector::ZeroVector;

    UPROPERTY(VisibleAnywhere)
    float ElapsedTime = 0.0f;

    UPROPERTY(VisibleAnywhere)
    FRecallPositionAnimationSettings Settings;

    /**
     * Get the current animation progress (0.0 to 1.0)
     * @return Normalized animation progress
     */
    float GetProgress() const;

    /**
     * Get the current animation phase based on progress
     * @return Current phase of the animation
     */
    ERecallPositionAnimationPhase GetPhase() const;

    /**
     * Get the progress within the bounce phase (0.0 to 1.0)
     * @return Normalized progress during bounce phase, 0 if not bouncing
     */
    float GetBounceProgress() const;

    /**
     * Calculate curve value for bounce animation
     * @param BounceProgress Progress within the bounce phase (0.0 to 1.0)
     * @return Curve value to apply to bounce height
     */
    float GetCurveValue(float BounceProgress) const;
};