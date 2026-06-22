// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallGameplaySettings.h"

URecallGameplaySettings::URecallGameplaySettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MaxAgents = 50;
	MaxAgentRadius = 100.0f;
	MaxAvoidedAgents = 6;
	MaxAvoidedWalls = 8;
	NavmeshCheckInterval = 1.0f;
	PathOptimizationInterval = 0.5f;
	SeparationDirClamp = -1.0f;
	PathOffsetRadiusMultiplier = 1.0f;
	bResolveCollisions = false;
	
	FRecallCrowdAvoidanceConfig AvoidanceConfig11;		// 11 samples, ECrowdAvoidanceQuality::Low
	AvoidanceConfig11.VelocityBias = 0.5f;
	AvoidanceConfig11.AdaptiveDivisions = 5;
	AvoidanceConfig11.AdaptiveRings = 2;
	AvoidanceConfig11.AdaptiveDepth = 1;
	AvoidanceConfig.Add(AvoidanceConfig11);
	
	FRecallCrowdAvoidanceConfig AvoidanceConfig22;		// 22 samples, ECrowdAvoidanceQuality::Medium
	AvoidanceConfig22.VelocityBias = 0.5f;
	AvoidanceConfig22.AdaptiveDivisions = 5;
	AvoidanceConfig22.AdaptiveRings = 2;
	AvoidanceConfig22.AdaptiveDepth = 2;
	AvoidanceConfig.Add(AvoidanceConfig22);

	FRecallCrowdAvoidanceConfig AvoidanceConfig45;		// 45 samples, ECrowdAvoidanceQuality::Good
	AvoidanceConfig45.VelocityBias = 0.5f;
	AvoidanceConfig45.AdaptiveDivisions = 7;
	AvoidanceConfig45.AdaptiveRings = 2;
	AvoidanceConfig45.AdaptiveDepth = 3;
	AvoidanceConfig.Add(AvoidanceConfig45);

	FRecallCrowdAvoidanceConfig AvoidanceConfig66;		// 66 samples, ECrowdAvoidanceQuality::High
	AvoidanceConfig66.VelocityBias = 0.5f;
	AvoidanceConfig66.AdaptiveDivisions = 7;
	AvoidanceConfig66.AdaptiveRings = 3;
	AvoidanceConfig66.AdaptiveDepth = 3;
	AvoidanceConfig.Add(AvoidanceConfig66);
}
