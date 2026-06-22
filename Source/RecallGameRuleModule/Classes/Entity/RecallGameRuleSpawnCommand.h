// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "System/Entity/RecallEntityAsyncSpawnTypes.h"

#include "RecallGameRuleSpawnCommand.generated.h"

/**
 * Spawn command for game rule entities.
 * Passes game rule asset data to spawned entities.
 */
USTRUCT()
struct RECALLGAMERULEMODULE_API FRecallGameRuleSpawnCommand : public FRecallEntityAsyncSpawnCommand
{
	GENERATED_BODY()

public:
	virtual void OnSpawn(FMassEntityManager& System, const TArray<FMassEntityHandle>& Entities) const override;
};
