// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "RecallCrowdAgentTypes.h"

#include "RecallCrowdAgentSettings.generated.h"

USTRUCT()
struct FRecallCrowdAgentSettings
{
	GENERATED_BODY()

	FRecallCrowdAgentSettings();

	UPROPERTY(EditAnywhere, meta=(ClampMin=0.0f))
	float SeparationWeight;
	
	UPROPERTY(EditAnywhere, meta=(Units=Centimeters, ClampMin=0.0f))
	float CollisionQueryRange;
	
	UPROPERTY(EditAnywhere, meta=(Units=Centimeters, ClampMin=0.0f))
	float PathOptimizationRange;
	
	UPROPERTY(EditAnywhere)
	ERecallCrowdAvoidanceQuality AvoidanceQuality;
	
	/** multiplier for avoidance samples during detection, doesn't affect actual velocity */
	UPROPERTY(EditAnywhere, meta=(ClampMin=0.0f))
	float AvoidanceRangeMultiplier = 1.0f;
	
	UPROPERTY(EditAnywhere)
	uint8 bEnableAnticipateTurns : 1;
	
	UPROPERTY(EditAnywhere)
	uint8 bEnableObstacleAvoidance : 1;
	
	UPROPERTY(EditAnywhere)
	uint8 bEnableSeparation : 1;
	
	UPROPERTY(EditAnywhere)
	uint8 bEnableOptimizeVisibility : 1;
	
	UPROPERTY(EditAnywhere)
	uint8 bEnableOptimizeTopology : 1;
	
	UPROPERTY(EditAnywhere)
	uint8 bEnablePathOffset : 1;
	
	UPROPERTY(EditAnywhere)
	uint8 bEnableSlowdownAtGoal : 1;
};
