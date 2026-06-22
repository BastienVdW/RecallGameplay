// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"

#include "RecallCrowdAvoidanceConfig.generated.h"

USTRUCT()
struct RECALLGAMEPLAYCORE_API FRecallCrowdAvoidanceConfig
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category=Crowd)
	float VelocityBias;

	UPROPERTY(EditAnywhere, Category=Crowd)
	float DesiredVelocityWeight;
	
	UPROPERTY(EditAnywhere, Category=Crowd)
	float CurrentVelocityWeight;
	
	UPROPERTY(EditAnywhere, Category=Crowd)
	float SideBiasWeight;
	
	UPROPERTY(EditAnywhere, Category=Crowd)
	float ImpactTimeWeight;

	UPROPERTY(EditAnywhere, Category=Crowd)
	float ImpactTimeRange;

	// index in SamplingPatterns array or 0xff for adaptive sampling
	UPROPERTY(EditAnywhere, Category=Crowd)
	uint8 CustomPatternIdx;

	// adaptive sampling: number of divisions per ring
	UPROPERTY(EditAnywhere, Category=Crowd)
	uint8 AdaptiveDivisions;

	// adaptive sampling: number of rings
	UPROPERTY(EditAnywhere, Category=Crowd)
	uint8 AdaptiveRings;
	
	// adaptive sampling: number of iterations at best velocity
	UPROPERTY(EditAnywhere, Category=Crowd)
	uint8 AdaptiveDepth;

	FRecallCrowdAvoidanceConfig() :
		VelocityBias(0.4f), DesiredVelocityWeight(2.0f), CurrentVelocityWeight(0.75f),
		SideBiasWeight(0.75f), ImpactTimeWeight(2.5f), ImpactTimeRange(2.5f),
		CustomPatternIdx(0xff), AdaptiveDivisions(7), AdaptiveRings(2), AdaptiveDepth(5)
	{}
};
