// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"

struct FRecallVoxelGrid;
struct FRecallFlatVoxelShapeTableRow;

namespace Recall::Voxel::Utils
{

/** Parse our voxel shape into a grid */
RECALLVOXEL_API extern FRecallVoxelGrid ParseVoxelShape(const FRecallFlatVoxelShapeTableRow& Shape);

/** Because the voxel shape's X and Y axis are flipped, the index has to be flipped as well before it can be used by the grid */
RECALLVOXEL_API extern int32 FlipVoxelGridIndex2D(const FRecallVoxelGrid& Grid, int32 Index);

/** Generate a mesh from our voxel grid */
RECALLVOXEL_API extern void GenerateMesh(const FRecallVoxelGrid& VoxelGrid, TArray<FVector3f>& OutVertices, TArray<int32>& OutTriangles, float VoxelSize, int32 PivotVoxelIndex = INDEX_NONE);

} // namespace Recall::Voxel::Utils
