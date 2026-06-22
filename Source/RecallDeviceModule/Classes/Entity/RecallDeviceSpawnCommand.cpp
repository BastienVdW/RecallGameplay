// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallDeviceSpawnCommand.h"

#include "MassCommandBuffer.h"
#include "MassEntityManager.h"
#include "MassEntityView.h"
#include "Simulation/GameplayTag/RecallGameplayTagFragments.h"

void FRecallDeviceSpawnCommand::OnSpawn(FMassEntityManager& System,
												  const TArray<FMassEntityHandle>& Entities) const
{
	for (const FMassEntityHandle& Entity : Entities)
	{
		const FMassEntityView EntityView(System, Entity);
		
		FRecallGameplayTagFragment* GameplayTagFragmentPtr = EntityView.GetFragmentDataPtr<FRecallGameplayTagFragment>();
		if (GameplayTagFragmentPtr != nullptr)
		{
			GameplayTagFragmentPtr->GameplayTagCountMap.AddTags(SpawnTags);
		}
	}	
}
