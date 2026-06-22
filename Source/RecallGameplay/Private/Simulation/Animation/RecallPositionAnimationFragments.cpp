// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "Simulation/Animation/RecallPositionAnimationFragments.h"

#include "Curves/CurveFloat.h"

float FRecallPositionAnimationFragment::GetProgress() const
{
    if (Settings.Duration > 0.0f)
    {
        return FMath::Clamp(ElapsedTime / Settings.Duration, 0.0f, 1.0f);
    }
    return 1.0f; // If duration is 0 or negative, consider animation complete
}

ERecallPositionAnimationPhase FRecallPositionAnimationFragment::GetPhase() const
{
    const float Progress = GetProgress();
    if (Progress < Settings.BounceStartProgress)
    {
        return ERecallPositionAnimationPhase::Moving;
    }
    else if (Progress < 1.0f)
    {
        return ERecallPositionAnimationPhase::Bouncing;
    }
    else
    {
        return ERecallPositionAnimationPhase::Complete;
    }
}

float FRecallPositionAnimationFragment::GetBounceProgress() const
{
    const float Progress = GetProgress();
    if (Progress >= Settings.BounceStartProgress && Progress < 1.0f)
    {
        // Calculate normalized progress within the bounce phase (0 to 1)
        return (Progress - Settings.BounceStartProgress) / (1.0f - Settings.BounceStartProgress);
    }
    return 0.0f;
}

float FRecallPositionAnimationFragment::GetCurveValue(float BounceProgress) const
{
    if (Settings.BounceCurve)
    {
        return Settings.BounceCurve->GetFloatValue(BounceProgress);
    }
    else
    {
        // Default bounce curve using sine wave for smooth up and down motion
        // Goes from 0 to 1 and back to 0 over the bounce duration
        return FMath::Sin(BounceProgress * PI);
    }
}