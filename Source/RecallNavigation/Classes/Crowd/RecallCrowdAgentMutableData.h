// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "RecallCrowdAgentTypes.h"

#include "RecallCrowdAgentMutableData.generated.h"

USTRUCT()
struct FRecallCrowdAgentMutableData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	ERecallCrowdSimulationState SimulationState = ERecallCrowdSimulationState::Enabled;
	
	/** if set, avoidance and steering will be suspended (used for direct move requests) */
	UPROPERTY(VisibleAnywhere)
	uint8 bSuspendCrowdSimulation = false;
};
