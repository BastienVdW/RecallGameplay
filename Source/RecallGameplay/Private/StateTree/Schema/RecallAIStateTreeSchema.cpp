// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "StateTree/Schema/RecallAIStateTreeSchema.h"

URecallAIStateTreeSchema::URecallAIStateTreeSchema()
	: Super()
{
}

bool URecallAIStateTreeSchema::IsStructAllowed(const UScriptStruct* InScriptStruct) const
{
	// Only allow Recall evals and tasks,and common conditions.
	return Super::IsStructAllowed(InScriptStruct);
}
