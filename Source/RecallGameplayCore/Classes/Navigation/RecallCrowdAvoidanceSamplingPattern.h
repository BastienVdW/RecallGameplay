// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"

#include "RecallCrowdAvoidanceSamplingPattern.generated.h"

USTRUCT(MinimalAPI)
struct FRecallCrowdAvoidanceSamplingPattern
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category=Crowd)
	TArray<float> Angles;

	UPROPERTY(EditAnywhere, Category = Crowd)
	TArray<float> Radii;

	RECALLGAMEPLAYCORE_API void AddSample(float AngleInDegrees, float NormalizedRadius);
	RECALLGAMEPLAYCORE_API void AddSampleWithMirror(float AngleInDegrees, float NormalizedRadius);
};
