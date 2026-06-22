// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "HierarchicalHashGrid2D.h"
#include "MassExtendedEntityHandle.h"

UENUM()
enum class ERecallNavigationObstacleFlags : uint8
{
	None				= 0,
	HasColliderData		= 1 << 0,
};
ENUM_CLASS_FLAGS(ERecallNavigationObstacleFlags)

struct FRecallNavigationObstacleItem
{
	bool operator==(const FRecallNavigationObstacleItem& Other) const
	{
		return Entity == Other.Entity;
	}

	FMassExtendedEntityHandle Entity;
	ERecallNavigationObstacleFlags ItemFlags = ERecallNavigationObstacleFlags::None;
	FVector Location = FVector::ZeroVector;
};

typedef THierarchicalHashGrid2D<2, 4, FRecallNavigationObstacleItem> FRecallNavigationObstacleHashGrid2D;	// 2 levels of hierarchy, 4 ratio between levels
