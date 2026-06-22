// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "MassExtendedEntityTypes.h"

#include "RecallNavigationObstacleFragments.generated.h"

USTRUCT()
struct RECALLNAVIGATION_API FRecallNavigationObstacleFragment : public FMassExtendedFragment
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	float Radius = 50.0f;
};
