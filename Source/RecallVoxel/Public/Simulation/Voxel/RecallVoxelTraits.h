// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "MassEntityTraitBase.h"
#include "Simulation/Physics/RecallPhysicsColliderTraits.h"

#include "RecallVoxelTraits.generated.h"

/**
* Define a voxel shape with his own custom collider
*/
UCLASS(meta=(DisplayName="RE Flat Voxel Shape"))
class RECALLVOXEL_API URecallFlatVoxelShapeTrait : public URecallPhysicsBodyTrait
{
	GENERATED_BODY()

public:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;

protected:
	UPROPERTY(EditAnywhere, meta=(RowType="/Script/RecallVoxel.RecallFlatVoxelShapeTableRow"))
	FDataTableRowHandle VoxelShape;

	UPROPERTY(EditAnywhere, meta=(Units="Centimeters"))
	float VoxelSize = 100.0f;

	UPROPERTY(EditAnywhere)
	bool bConvexHull = false;

};
