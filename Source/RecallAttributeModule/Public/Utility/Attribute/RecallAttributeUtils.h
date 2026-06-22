// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

struct FRecallAttributeSet;
struct FRecallAttributeCondition;
struct FRecallAttributeRepresentation;

namespace Recall::Attribute::Utils
{

RECALLATTRIBUTEMODULE_API bool EvaluateCondition(
	const FRecallAttributeCondition& Condition, const FRecallAttributeSet& AttributeSet);

RECALLATTRIBUTEMODULE_API TMap<FGameplayTag, FRecallAttributeRepresentation> GetAttributeRepresentationMap(
	const FRecallAttributeSet& AttributeSet);
	
} // Recall::Attribute::Utils
