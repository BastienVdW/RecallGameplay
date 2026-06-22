// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "StateTreeTaskBase.h" 

#include "RecallGameRuleStateTreeTaskBase.generated.h"

USTRUCT(meta=(DisplayName="Game Rule Task Base"))
struct RECALLGAMERULEMODULE_API FRecallGameRuleStateTreeTaskBase : public FStateTreeTaskBase
{
	GENERATED_BODY()
};
