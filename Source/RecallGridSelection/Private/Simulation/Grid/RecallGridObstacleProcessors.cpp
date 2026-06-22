// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallGridObstacleProcessors.h"

#include "MassExecutionContext.h"
#include "Simulation/Grid/RecallGridObstacleFragments.h"
#include "System/Grid/RecallGridSelectionSubsystem.h"

//----------------------------------------------------------------------//
// URecallGridObstacleDeinitializer
//----------------------------------------------------------------------//
URecallGridObstacleDeinitializer::URecallGridObstacleDeinitializer()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::All);
	ObservedTypes.Add(FRecallGridObstacleFragment::StaticStruct());
	ObservedOperations = EMassObservedOperationFlags::Remove;
}

void URecallGridObstacleDeinitializer::InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallGridObstacleDeinitializer::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallGridObstacleFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddSubsystemRequirement<URecallGridSelectionSubsystem>(EMassFragmentAccess::ReadWrite);
}

void URecallGridObstacleDeinitializer::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(Context, [](FMassExecutionContext& Context)
	{
		auto& GridSelectionSubsystem = Context.GetMutableSubsystemChecked<URecallGridSelectionSubsystem>();
		
		const TConstArrayView<FRecallGridObstacleFragment> ObstacleList = Context.GetFragmentView<FRecallGridObstacleFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			const FRecallGridObstacleFragment& ObstacleFragment = ObstacleList[EntityIndex];			
			GridSelectionSubsystem.UnregisterCell(ObstacleFragment.GridCellIndex);
		}
	});
}
