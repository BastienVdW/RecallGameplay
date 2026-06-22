// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallGridNavigationTasks.h"

#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "System/Grid/RecallGridSelectionSubsystem.h"

//----------------------------------------------------------------------//
// FRecallGridCellOffsetTask
//----------------------------------------------------------------------//
bool FRecallGridCellOffsetTask::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(GridSelectionSystemHandle);
	return true;
}

EStateTreeRunStatus FRecallGridCellOffsetTask::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	const URecallGridSelectionSubsystem& GridSelectionSystem = Context.GetExternalData(GridSelectionSystemHandle);
	
	// Validate input grid cell index
	if (InstanceData.GridCellIndex == INDEX_NONE)
	{
		return EStateTreeRunStatus::Failed;
	}

	// Use the grid subsystem to calculate the offset cell index
	const int32 OutputCellIndex = GridSelectionSystem.GetGridIndexFromCoordinatesWithOffset(
		InstanceData.GridCellIndex, 
		InstanceData.OffsetX, 
		InstanceData.OffsetY
	);
	
	// Check if the calculated output cell index is valid
	// Returns failure if the offset would place the cell outside the grid boundaries
	if (OutputCellIndex == INDEX_NONE)
	{
		return EStateTreeRunStatus::Failed;
	}
	
	InstanceData.OutputGridCellIndex = OutputCellIndex;

	if (bSucceedOnOffset)
	{
		return EStateTreeRunStatus::Succeeded;
	}
	
	return Super::EnterState(Context, Transition);
}
