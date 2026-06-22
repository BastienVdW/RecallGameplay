// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Mass/EntityElementTypes.h"

#include "RecallGameRuleFragments.generated.h"

/**
 * Tag to identify the game rule entity.
 */
USTRUCT()
struct RECALLGAMERULEMODULE_API FRecallGameRuleTag : public FMassTag
{
	GENERATED_BODY()
};
