// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallGridObstacleProcessors.h"

#include "MassExtendedExecutionContext.h"
#include "Simulation/Grid/RecallGridObstacleFragments.h"
#include "System/Grid/RecallGridSelectionSubsystem.h"

//----------------------------------------------------------------------//
// URecallGridObstacleDeinitializer
//----------------------------------------------------------------------//
URecallGridObstacleDeinitializer::URecallGridObstacleDeinitializer()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EExtendedProcessorExecutionFlags::All);
	ObservedType = FRecallGridObstacleFragment::StaticStruct();
	Operation = EMassExtendedObservedOperation::Remove;
}

void URecallGridObstacleDeinitializer::InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallGridObstacleDeinitializer::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallGridObstacleFragment>(EMassExtendedFragmentAccess::ReadOnly);
	EntityQuery.AddSubsystemRequirement<URecallGridSelectionSubsystem>(EMassExtendedFragmentAccess::ReadWrite);
}

void URecallGridObstacleDeinitializer::Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(Context, [](FMassExtendedExecutionContext& Context)
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
