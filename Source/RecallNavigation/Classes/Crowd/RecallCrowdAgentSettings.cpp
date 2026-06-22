// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallCrowdAgentSettings.h"

FRecallCrowdAgentSettings::FRecallCrowdAgentSettings()
{
	bEnableAnticipateTurns = false;
	bEnableObstacleAvoidance = true;
	bEnableSeparation = false;
	bEnableOptimizeVisibility = true;
	bEnableOptimizeTopology = true;
	bEnablePathOffset = false;
	bEnableSlowdownAtGoal = true;
	
	SeparationWeight = 2.0f;
	CollisionQueryRange = 400.0f;		// approx: radius * 12.0f
	PathOptimizationRange = 1000.0f;	// approx: radius * 30.0f
	AvoidanceQuality = ERecallCrowdAvoidanceQuality::Low;
	AvoidanceRangeMultiplier = 1.0f;
}
