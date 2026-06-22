// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "RecallCrowdAgentCollisionSettings.generated.h"

/**
 * Crowd agent collision configuration settings common to all navigation systems
 * Contains settings that are needed by the crowd avoidance processor
 * regardless of whether navigation is NavMesh or Grid based
 */
USTRUCT()
struct RECALLNAVIGATION_API FRecallCrowdAgentCollisionSettings
{
	GENERATED_BODY()

	/** Agent height used for crowd avoidance collision cylinder */
	UPROPERTY(EditAnywhere)
	float AgentHeight = 100.0f;
};
