// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "MassExtendedExternalSubsystemTraits.h"
#include "RecallNavigationObstacleTypes.h"

#include "RecallNavigationObstacleSubsystem.generated.h"

/**
 * Keep track of navigation obstacles,
 * the grid will be generated at the start of each frame.
 */
UCLASS()
class RECALLNAVIGATION_API URecallNavigationObstacleSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	const FRecallNavigationObstacleHashGrid2D& GetObstacleGrid() const { return AvoidanceObstacleGrid; }
	FRecallNavigationObstacleHashGrid2D& GetObstacleGridMutable() { return AvoidanceObstacleGrid; }

private:
	FRecallNavigationObstacleHashGrid2D AvoidanceObstacleGrid;
	
};

template<>
struct TMassExtendedExternalSubsystemTraits<URecallNavigationObstacleSubsystem> final
{
	enum
	{
		GameThreadOnly = false
	};
};
