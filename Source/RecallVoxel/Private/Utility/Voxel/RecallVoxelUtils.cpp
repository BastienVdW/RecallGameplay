// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "Utility/Voxel/RecallVoxelUtils.h"

#include "Data/Voxel/RecallFlatVoxelShapeTableRow.h"
#include "Simulation/Voxel/RecallVoxelTypes.h"

namespace Recall::Voxel::Utils
{

FRecallVoxelGrid ParseVoxelShape(const FRecallFlatVoxelShapeTableRow& Shape)
{
	TArray<FString> Lines;

	// X is forward so it is equals to the amount of lines
	const int32 GridSizeX = Shape.VoxelShape.ParseIntoArrayLines(Lines);

	// Y is right so it is equals to the max amount of char per line
	int32 GridSizeY = 0;
	for (int32 LineIndex = 0; LineIndex < Lines.Num(); LineIndex++)
	{
		GridSizeY = FMath::Max(GridSizeY, Lines[LineIndex].Len());
	}

	FRecallVoxelGrid VoxelGrid(GridSizeX, GridSizeY);

	for (int32 LineIndex = 0; LineIndex < Lines.Num(); LineIndex++)
	{
		const FString& Line = Lines[LineIndex];
		const TArray<TCHAR, FString::AllocatorType>& CharArray = Line.GetCharArray();

		const int32 X = Lines.Num() - LineIndex; // Flip

		for (int32 CharIndex = 0; CharIndex < Line.Len(); CharIndex++)
		{
			const int32 Y = CharIndex + 1;
			VoxelGrid.SetVoxel(CharArray[CharIndex], X, Y);
		}
	}

	return VoxelGrid;
}

int32 FlipVoxelGridIndex2D(const FRecallVoxelGrid& Grid, int32 Index)
{
	const FIntVector GridSize = Grid.GetSize();

	// Flip X and Y to get the correct pivot voxel index
	const int32 UnflippedPivotPosX = (Index % GridSize.Y) + 1;
	const int32 UnflippedPivotPosY = GridSize.X - (Index / GridSize.Y);

	const FRecallVoxelGrid UnflippedGrid(GridSize.Y, GridSize.X);
	return Grid.GetVoxelIndex(UnflippedPivotPosY, UnflippedPivotPosX);
}

static void AddFace(const FRecallVoxelGrid& VoxelGrid, const FVector3f& Foward, const FVector3f& Up, const FVector3f& Right, const FIntVector& Pos, 
	TArray<FVector3f>& OutVertices, TArray<int32>& OutTriangles, float VoxelSize)
{
	const FIntVector Neighbor = Pos + FIntVector(Foward.X, Foward.Y, Foward.Z);
	if (!VoxelGrid.IsEmpty(Neighbor.X, Neighbor.Y, Neighbor.Z))
	{
		return;
	}

	const float HalfSize = VoxelSize * 0.5f;
	const FVector3f Center((Pos.X - 0.5f) * VoxelSize, (Pos.Y - 0.5f) * VoxelSize, (Pos.Z - 0.5f) * VoxelSize);

	OutVertices.Reserve(OutVertices.Num() + 4);

	OutVertices.Add(Center + Foward * HalfSize - Right * HalfSize + Up * HalfSize); // Top left (-4)
	OutVertices.Add(Center + Foward * HalfSize + Right * HalfSize + Up * HalfSize); // Top right (-3)
	OutVertices.Add(Center + Foward * HalfSize + Right * HalfSize - Up * HalfSize); // Bottom right (-2)
	OutVertices.Add(Center + Foward * HalfSize - Right * HalfSize - Up * HalfSize); // Bottom left (-1)

	OutTriangles.Add(OutVertices.Num() - 4); // TL
	OutTriangles.Add(OutVertices.Num() - 3); // TR
	OutTriangles.Add(OutVertices.Num() - 1); // BL

	OutTriangles.Add(OutVertices.Num() - 3); // TR
	OutTriangles.Add(OutVertices.Num() - 2); // BR
	OutTriangles.Add(OutVertices.Num() - 1); // BL
}

static void AddBox(const FRecallVoxelGrid& VoxelGrid, const FIntVector& Pos, TArray<FVector3f>& OutVertices, TArray<int32>& OutTriangles, float VoxelSize)
{
	static const FVector3f Right(0.f, 1.f, 0.f);
	static const FVector3f Up(0.f, 0.f, 1.f);
	static const FVector3f Forward(1.f, 0.f, 0.f);

	AddFace(VoxelGrid, -Right, Up, Forward, Pos, OutVertices, OutTriangles, VoxelSize); // Left
	AddFace(VoxelGrid, Right, Up, -Forward, Pos, OutVertices, OutTriangles, VoxelSize); // Right
	AddFace(VoxelGrid, Up, Forward, -Right, Pos, OutVertices, OutTriangles, VoxelSize); // Up
	AddFace(VoxelGrid, -Up, Forward, Right, Pos, OutVertices, OutTriangles, VoxelSize); // Down
	AddFace(VoxelGrid, Forward, Up, Right, Pos, OutVertices, OutTriangles, VoxelSize); // Front
	AddFace(VoxelGrid, -Forward, Up, -Right, Pos, OutVertices, OutTriangles, VoxelSize); // Back
}

static FVector3f GetVoxelGridOffset(const FRecallVoxelGrid& VoxelGrid, float VoxelSize, int32 PivotVoxelIndex)
{
	if (PivotVoxelIndex == INDEX_NONE)
	{
		return (FVector3f)VoxelGrid.GetSize() * (VoxelSize * 0.5f);
	}
	else
	{
		const FVector3f HalfVoxel = (FVector3f)(.5f * VoxelSize);
		const FIntVector PivotPosition = VoxelGrid.GetVoxelPosition(PivotVoxelIndex) - FIntVector(1, 1, 1);
		return ((FVector3f)PivotPosition * VoxelSize) + HalfVoxel;
	}
}

void GenerateMesh(const FRecallVoxelGrid& VoxelGrid, TArray<FVector3f>& OutVertices, TArray<int32>& OutTriangles, float VoxelSize, int32 PivotVoxelIndex)
{
	for (int32 Z = 1; Z <= VoxelGrid.GetSize().Z; Z++)
	{
		for (int32 Y = 1; Y <= VoxelGrid.GetSize().Y; Y++)
		{
			for (int32 X = 1; X <= VoxelGrid.GetSize().X; X++)
			{
				if (VoxelGrid.IsEmpty(X, Y, Z))
				{
					continue;
				}
				
				const FIntVector Pos(X, Y, Z);
				AddBox(VoxelGrid, Pos, OutVertices, OutTriangles, VoxelSize);
			}
		}
	}

	// TODO: Share vertices

	// Offset everything
	const FVector3f Offset = GetVoxelGridOffset(VoxelGrid, VoxelSize, PivotVoxelIndex);

	for (FVector3f& Vertex : OutVertices)
	{
		Vertex -= Offset;
	}
}

} // namespace Recall::Voxel::Utils
