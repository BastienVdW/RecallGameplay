// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "Simulation/Voxel/RecallVoxelTraits.h"

#include "Data/Voxel/RecallFlatVoxelShapeTableRow.h"
#include "MassEntityTemplateRegistry.h"
#include "Simulation/Voxel/RecallVoxelFragments.h"
#include "Simulation/Physics/RecallPhysicsBodyFragment.h"
#include "Simulation/Transform/RecallTransformFragments.h"
#include "Utility/Trait/RecallTraitUtils.h"
#include "Utility/Voxel/RecallVoxelUtils.h"

void URecallFlatVoxelShapeTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	Super::BuildTemplate(BuildContext, World);

	FRecallVoxelShapeFragment& VoxelShapeFragment = BuildContext.AddFragment_GetRef<FRecallVoxelShapeFragment>();

	static const FString ContextString(TEXT("URecallFlatVoxelShapeTrait::BuildTemplate"));
	
	const FRecallFlatVoxelShapeTableRow* VoxelShapePtr = VoxelShape.GetRow<FRecallFlatVoxelShapeTableRow>(ContextString);
	if (ensureAlwaysMsgf(VoxelShapePtr != nullptr, TEXT("Failed to find voxel shape in data table")))
	{
		VoxelShapeFragment.VoxelGrid = Recall::Voxel::Utils::ParseVoxelShape(*VoxelShapePtr);
		VoxelShapeFragment.PivotVoxelIndex = Recall::Voxel::Utils::FlipVoxelGridIndex2D(VoxelShapeFragment.VoxelGrid, VoxelShapePtr->PivotVoxelIndex);
	}

	FMassEntityManager& EntityManager = UE::Mass::Utils::GetEntityManagerChecked(World);

	FRecallVoxelShapeConstSharedFragment ConstSharedFragment;
	ConstSharedFragment.PhysicsParams = Params;
	ConstSharedFragment.bConvexHull = bConvexHull;
	ConstSharedFragment.VoxelSize = VoxelSize;

	BuildContext.AddConstSharedFragment(EntityManager.GetOrCreateConstSharedFragment(ConstSharedFragment));
}
