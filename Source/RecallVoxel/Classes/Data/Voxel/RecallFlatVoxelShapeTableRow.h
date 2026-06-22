// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "RecallFlatVoxelShapeTableRow.generated.h"

USTRUCT(BlueprintType)
struct RECALLVOXEL_API FRecallFlatVoxelShapeTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	// Define our voxel shape with characters 0 is empty
	UPROPERTY(EditAnywhere, meta=(MultiLine))
	FString VoxelShape;

	/**
	* Index of the voxel to use as pivot position
	* Empty voxels can be used as pivot
	*/
	UPROPERTY(EditAnywhere)
	int32 PivotVoxelIndex = 0;

};
