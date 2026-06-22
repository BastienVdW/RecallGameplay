// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallInteractCollectCommandTypes.h"

#include "MassCommandBuffer.h"
#include "MassEntityView.h"
#include "MassExecutionContext.h"
#include "RecallSignalSubsystem.h"
#include "Simulation/Collect/RecallCollectSignalTypes.h"
#include "Simulation/Inventory/RecallDropItemFragments.h"
#include "Simulation/Transform/RecallTransformFragments.h"
#include "Utility/Inventory/RecallInventoryUtils.h"

//----------------------------------------------------------------------//
// FRecallCollectCommand:
//----------------------------------------------------------------------//
void FRecallCollectCommand::OnExecute(const FRecallInteractContext& Context) const
{
	if (Context.SignalSystemPtr != nullptr)
	{
		Context.SignalSystemPtr->SignalEntity(Recall::Collect::Signals::Collect, Context.InteractableEntity);
	}
}

//----------------------------------------------------------------------//
// FRecallDropItemCommand
//----------------------------------------------------------------------//
void FRecallDropItemCommand::OnExecute(const FRecallInteractContext& Context) const
{
	const FMassEntityView EntityView(Context.GetEntityManagerChecked(), Context.InteractableEntity);
	const FRecallTransformFragment& TransformFragment = EntityView.GetFragmentData<FRecallTransformFragment>();
	const FVector SpawnPosition = TransformFragment.Position + SpawnOffset;

	if (bDestroyInteractableEntity)
	{
		Context.ExecutionContext.Defer().DestroyEntity(Context.InteractableEntity);
	}

	FRecallDropItemDefinition Definition = DropItem;

	const FRecallDropItemConstSharedFragment* DropItemConstSharedFragmentPtr = EntityView.GetConstSharedFragmentDataPtr<FRecallDropItemConstSharedFragment>();
	if (bUseDropItemTrait && ensureAlwaysMsgf(DropItemConstSharedFragmentPtr != nullptr,
		TEXT("%hs Invalid trait"), __FUNCTION__))
	{
		Definition = DropItemConstSharedFragmentPtr->DropItem;
	}
	
	Recall::Inventory::Utils::CreateDropItem(Context.GetEntityManagerChecked(), Definition, SpawnPosition);
}
