// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "System/Obstacle/RecallNavigationObstacleTypes.h"

constexpr int32 MaxExpectedAgentsPerCell = 6;
constexpr int32 MinTouchingCellCount = 4;
constexpr int32 MaxObstacleResults = MaxExpectedAgentsPerCell * MinTouchingCellCount;

namespace Recall::Navigation::Obstacle::Utils
{

RECALLNAVIGATION_API extern void FindCloseObstacles(const FVector& Center, const FVector::FReal SearchRadius,
	const FRecallNavigationObstacleHashGrid2D& AvoidanceObstacleGrid,
	TArray<FRecallNavigationObstacleItem, TFixedAllocator<MaxObstacleResults>>& OutCloseEntities,
	const int32 MaxResults = MaxObstacleResults);

} // namespace Recall::Navigation::Obstacle::Utils
