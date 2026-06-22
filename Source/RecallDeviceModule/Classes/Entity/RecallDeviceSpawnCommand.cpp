// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallDeviceSpawnCommand.h"

#include "MassExtendedCommandBuffer.h"
#include "MassExtendedEntityManager.h"
#include "MassExtendedEntityView.h"
#include "Simulation/GameplayTag/RecallGameplayTagFragments.h"

void FRecallDeviceSpawnCommand::OnSpawn(FMassExtendedEntityManager& System,
												  const TArray<FMassExtendedEntityHandle>& Entities) const
{
	for (const FMassExtendedEntityHandle& Entity : Entities)
	{
		const FMassExtendedEntityView EntityView(System, Entity);
		
		FRecallGameplayTagFragment* GameplayTagFragmentPtr = EntityView.GetFragmentDataPtr<FRecallGameplayTagFragment>();
		if (GameplayTagFragmentPtr != nullptr)
		{
			GameplayTagFragmentPtr->GameplayTagCountMap.AddTags(SpawnTags);
		}
	}	
}
