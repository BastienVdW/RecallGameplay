// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallInteractEntityCommandTypes.h"

#include "MassExtendedEntityTypes.h"
#include "MassExtendedEntityView.h"
#include "Chaos/PBDRigidClusteringAlgo.h"
#include "Simulation/Transform/RecallTransformFragments.h"
#include "System/Entity/RecallEntityAsyncSpawnSubsystem.h"

void FRecallInteractSpawnEntityEventCommand::OnExecute(const FRecallInteractContext& Context) const
{
	const FMassExtendedEntityHandle TargetEntity = Context.GetTargetEntity(ParentTarget);
	const FMassExtendedEntityView TargetView(Context.GetEntityManagerChecked(), TargetEntity);

	const FRecallTransformFragment* TargetTransformFragmentPtr = TargetView.GetFragmentDataPtr<FRecallTransformFragment>();
	if (TargetTransformFragmentPtr == nullptr)
	{
		return;
	}
	
	URecallEntityAsyncSpawnSubsystem* EntityAsyncSpawnSystem = UWorld::GetSubsystem<URecallEntityAsyncSpawnSubsystem>(Context.GetWorld());
	checkf(IsValid(EntityAsyncSpawnSystem),
		TEXT("%hs Invalid entity async spawn system"), __FUNCTION__)
	
	FRecallEntityAsyncSpawnParameters Params;
	Params.SpawnCommand = SpawnCommand;
	
	FVector PositionOffset = TargetTransformFragmentPtr->Position;
	FQuat RotationOffset = TargetTransformFragmentPtr->Rotation;

	if (bAttach)
	{
		Params.ParentEntity = TargetEntity;
	}
	
	EntityAsyncSpawnSystem->SpawnEntityAsync(EntityConfigAsset, PositionOffset, RotationOffset, Params);
}
