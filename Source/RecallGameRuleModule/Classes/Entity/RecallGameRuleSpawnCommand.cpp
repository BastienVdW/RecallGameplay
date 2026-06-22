// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "Entity/RecallGameRuleSpawnCommand.h"

#include "MassEntityManager.h"
#include "MassEntityView.h"
#include "Simulation/Game/RecallGameRuleFragments.h"

void FRecallGameRuleSpawnCommand::OnSpawn(FMassEntityManager& System, const TArray<FMassEntityHandle>& Entities) const
{
	// Apply game rule data to spawned entities
	for (const FMassEntityHandle& Entity : Entities)
	{
		System.AddTagToEntity(Entity, FRecallGameRuleTag::StaticStruct());
	}
}