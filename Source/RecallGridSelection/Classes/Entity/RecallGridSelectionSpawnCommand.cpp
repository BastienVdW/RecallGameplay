// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallGridSelectionSpawnCommand.h"

#include "MassEntityManager.h"
#include "MassEntityView.h"
#include "Simulation/Grid/RecallGridCursorFragments.h"

void FRecallGridSelectionSpawnCommand::OnSpawn(FMassEntityManager& System,
                                              const TArray<FMassEntityHandle>& Entities) const
{
	if (!System.IsEntityValid(SelectionOwnerEntity) || !ensure(Entities.Num() == 1))
	{
		System.BatchDestroyEntities(Entities);
		return;
	}

	const FMassEntityView CursorOwnerView(System, SelectionOwnerEntity);
	auto& CursorOwnerFragment = CursorOwnerView.GetFragmentData<FRecallGridCursorOwnerFragment>();
	CursorOwnerFragment.GridSelectionEntity = Entities[0];
}
