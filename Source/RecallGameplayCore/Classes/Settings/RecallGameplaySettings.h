// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Engine/DeveloperSettings.h"
#include "Navigation/RecallCrowdAvoidanceConfig.h"
#include "Navigation/RecallCrowdAvoidanceSamplingPattern.h"

#include "RecallGameplaySettings.generated.h"

/**
 * Settings for the RecallGameplay modules.
 */
UCLASS(config=Game, defaultconfig, meta=(DisplayName="Recall Gameplay"))
class RECALLGAMEPLAYCORE_API URecallGameplaySettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	URecallGameplaySettings(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// How long an env query will be ticked
	UPROPERTY(config, EditAnywhere, Category="Navigation|EnvQuery", meta=(ClampMin=1))
	int32 EnvQueryDuration = 5;

	UPROPERTY(config, EditAnywhere, Category="Navigation|EnvQuery", meta=(ClampMin=1))
	int32 ConcurrentEnvQueryLimit = 20;

	UPROPERTY(config, EditAnywhere, Category="Navigation|EnvQuery", meta=(ClampMin=1))
	int32 EnvQueryDebugCacheDuration = 60;
	
	// How long a path will be ticked
	UPROPERTY(config, EditAnywhere, Category="Navigation|Path", meta=(ClampMin=1))
	int32 PathWaitDuration = 5;
	
	UPROPERTY(config, EditAnywhere, Category="Navigation|Path", meta=(ClampMin=1))
	int32 ConcurrentPathLimit = 20;
	
	/** obstacle avoidance params */
	UPROPERTY(config, EditAnywhere, Category="Navigation|Crowd")
	TArray<FRecallCrowdAvoidanceConfig> AvoidanceConfig;

	/** obstacle avoidance params */
	UPROPERTY(config, EditAnywhere, Category="Navigation|Crowd")
	TArray<FRecallCrowdAvoidanceSamplingPattern> SamplingPatterns;

	/** max number of agents supported by crowd */
	UPROPERTY(config, EditAnywhere, Category="Navigation|Crowd")
	int32 MaxAgents;

	/** max radius of agent that can be added to crowd */
	UPROPERTY(config, EditAnywhere, Category="Navigation|Crowd")
	float MaxAgentRadius;

	/** max number of neighbor agents for velocity avoidance */
	UPROPERTY(config, EditAnywhere, Category="Navigation|Crowd")
	int32 MaxAvoidedAgents;

	/** max number of wall segments for velocity avoidance */
	UPROPERTY(config, EditAnywhere, Category="Navigation|Crowd")
	int32 MaxAvoidedWalls;

	/** how often should agents check their position after moving off navmesh? */
	UPROPERTY(config, EditAnywhere, Category="Navigation|Crowd")
	float NavmeshCheckInterval;

	/** how often should agents try to optimize their paths? */
	UPROPERTY(config, EditAnywhere, Category="Navigation|Crowd")
	float PathOptimizationInterval;

	/** clamp separation force to left/right when neighbor is behind (dot between forward and dirToNei, -1 = disabled) */
	UPROPERTY(config, EditAnywhere, Category="Navigation|Crowd")
	float SeparationDirClamp;

	/** agent radius multiplier for offsetting path around corners */
	UPROPERTY(config, EditAnywhere, Category="Navigation|Crowd")
	float PathOffsetRadiusMultiplier;
	
	/** should crowd simulation resolve collisions between agents? if not, this will be handled by their movement components */
	UPROPERTY(config, EditAnywhere, Category="Navigation|Crowd")
	uint32 bResolveCollisions : 1;


};
