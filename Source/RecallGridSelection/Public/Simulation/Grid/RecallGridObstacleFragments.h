// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "MassExtendedEntityTypes.h"

#include "RecallGridObstacleFragments.generated.h"

USTRUCT()
struct RECALLGRIDSELECTION_API FRecallGridObstacleFragment : public FMassExtendedFragment
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	int32 GridCellIndex = INDEX_NONE;
};
