// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallCrowdAvoidanceSamplingPattern.h"

void FRecallCrowdAvoidanceSamplingPattern::AddSample(float AngleInDegrees, float NormalizedRadius)
{
	Angles.Add(FMath::DegreesToRadians(AngleInDegrees));
	Radii.Add(NormalizedRadius);
}

void FRecallCrowdAvoidanceSamplingPattern::AddSampleWithMirror(float AngleInDegrees, float NormalizedRadius)
{
	AddSample(AngleInDegrees, NormalizedRadius);
	AddSample(-AngleInDegrees, NormalizedRadius);
}
