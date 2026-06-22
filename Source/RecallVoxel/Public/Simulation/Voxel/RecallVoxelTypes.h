// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"

#include "RecallVoxelTypes.generated.h"

USTRUCT()
struct RECALLVOXEL_API FRecallVoxelGrid
{
	GENERATED_BODY()

public:
	FRecallVoxelGrid() = default;
	FRecallVoxelGrid(int32 X, int32 Y, int32 Z = 1)
	{
		SetSize(X, Y, Z);
	}

	FORCEINLINE void SetSize(int32 X, int32 Y, int32 Z = 1)
	{
		GridSize = FIntVector(X, Y, Z);
		VoxelGrid.SetNumZeroed(Z * Y * X);
	}

	FORCEINLINE void SetVoxel(const TCHAR& Char, int32 X, int32 Y, int32 Z = 1)
	{
		const int32 Index = GetVoxelIndex(X, Y, Z);
		VoxelGrid[Index] = Char;
	}

	FORCEINLINE TCHAR GetVoxel(int32 X, int32 Y, int32 Z = 1) const
	{
		const int32 Index = GetVoxelIndex(X, Y, Z);
		return VoxelGrid[Index];
	}

	FORCEINLINE bool IsEmpty(int32 X, int32 Y, int32 Z = 1) const
	{
		const int32 Index = GetVoxelIndex(X, Y, Z);
		return !VoxelGrid.IsValidIndex(Index) || VoxelGrid[Index] == '0';
	}

	FORCEINLINE const FIntVector& GetSize() const { return GridSize; }

	FORCEINLINE int32 GetVoxelIndex(int32 X, int32 Y, int32 Z = 1) const
	{
		if (!FMath::IsWithinInclusive(X, 1, GridSize.X) ||
			!FMath::IsWithinInclusive(Y, 1, GridSize.Y) ||
			!FMath::IsWithinInclusive(Z, 1, GridSize.Z))
		{
			return INDEX_NONE;
		}

		return (GridSize.X * GridSize.Y) * (Z - 1) +
			GridSize.X * (Y - 1) + (X - 1);
	}

	FORCEINLINE FIntVector GetVoxelPosition(int32 Index) const
	{
		if (!VoxelGrid.IsValidIndex(Index))
		{
			return FIntVector::ZeroValue;
		}

		const int32 Z = (Index / (GridSize.X * GridSize.Y)) + 1;
		const int32 Y = ((Index / GridSize.X) % GridSize.Y) + 1;
		const int32 X = (Index % GridSize.X) + 1;

		return FIntVector(X, Y, Z);
	}

protected:
	// Size of our voxel grid
	UPROPERTY(VisibleAnywhere)
	FIntVector GridSize = FIntVector::ZeroValue;

	// Voxels are represented by a letter (bytes)
	UPROPERTY(VisibleAnywhere)
	TArray<uint8> VoxelGrid;
};
