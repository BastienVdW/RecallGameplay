// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "StateTreeConditionBase.h"

#include "RecallPlayerConditionBase.generated.h"

/**
 * Base struct for all Mass StateTree Evaluators.
 */
USTRUCT(meta=(DisplayName="Player Condition Base"))
struct RECALLGAMEPLAY_API FRecallPlayerConditionBase : public FStateTreeConditionBase
{
	GENERATED_BODY()
};
