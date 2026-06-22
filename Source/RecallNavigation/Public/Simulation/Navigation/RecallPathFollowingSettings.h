// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "RecallPathFollowingSettings.generated.h"

/**
 * Path following configuration settings common to all navigation systems
 * Contains settings that are needed by the path follower processor (URecallPathFollowerProcessor)
 * regardless of whether paths come from NavMesh or Grid navigation
 */
USTRUCT()
struct RECALLNAVIGATION_API FRecallPathFollowingSettings
{
	GENERATED_BODY()

	/** Distance threshold to consider a waypoint reached and advance to next segment */
	UPROPERTY(EditAnywhere, meta=(Units=Centimeters))
	float DestinationThreshold = 10.0f;

	/** Agent collision radius - used for obstacle fragment and navigation queries */
	UPROPERTY(EditAnywhere, meta=(Units=Centimeters))
	float Radius = 50.0f;

	/** Agent height - used for NavMesh queries and crowd avoidance collision */
	UPROPERTY(EditAnywhere, meta=(Units=Centimeters))
	float AgentHeight = 100.0f;
};
