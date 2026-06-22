// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Mass/EntityElementTypes.h"
#include "MassEntityTypes.h"
#include "Physics/RecallPhysicsTypes.h"
#include "RecallVoxelTypes.h"

#include "RecallVoxelFragments.generated.h"

USTRUCT()
struct RECALLVOXEL_API FRecallVoxelShapeFragment : public FMassFragment
{
	GENERATED_BODY()

	// Index of the voxel used as pivot for this shape
	UPROPERTY(VisibleAnywhere)
	int32 PivotVoxelIndex = 0;

	// Grid of voxels that defines our shape
	UPROPERTY(VisibleAnywhere)
	FRecallVoxelGrid VoxelGrid;
};

template <>
struct TMassFragmentTraits<FRecallVoxelShapeFragment> final
{ enum { AuthorAcceptsItsNotTriviallyCopyable = true }; };

USTRUCT()
struct RECALLVOXEL_API FRecallVoxelShapeConstSharedFragment : public FMassConstSharedFragment
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	float VoxelSize = 100.0f;

	UPROPERTY(VisibleAnywhere)
	FRecallPhysicsBodyParameters PhysicsParams;

	UPROPERTY(VisibleAnywhere)
	bool bConvexHull = false;
};
