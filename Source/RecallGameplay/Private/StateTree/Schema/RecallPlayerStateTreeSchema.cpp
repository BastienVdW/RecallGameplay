// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "StateTree/Schema/RecallPlayerStateTreeSchema.h"

#include "Simulation/Player/Input/RecallPlayerInputFragments.h"
#include "StateTree/Player/RecallPlayerConditionBase.h"
#include "StateTree/Player/RecallPlayerEvaluatorBase.h"
#include "StateTree/RecallStateTreeEvaluatorBase.h"
#include "StateTree/RecallStateTreeTaskBase.h"
#include "StateTree/Player/RecallPlayerTaskBase.h"

URecallPlayerStateTreeSchema::URecallPlayerStateTreeSchema()
{
	LinkContextData(PlayerInputFragment,			FName("Input Data"),		FGuid(TEXT("1F9A3657-4E61-F9EC-8038-BE91D36B562B")));
}

bool URecallPlayerStateTreeSchema::IsStructAllowed(const UScriptStruct* InScriptStruct) const
{
	return Super::IsStructAllowed(InScriptStruct)
		|| IsStructChildOf<FRecallPlayerConditionBase>(InScriptStruct)
		|| IsStructChildOf<FRecallPlayerEvaluatorBase>(InScriptStruct)
		|| IsStructChildOf<FRecallPlayerTaskBase>(InScriptStruct)
		|| IsStructChildOf<FRecallStateTreeEvaluatorBase>(InScriptStruct)
		|| IsStructChildOf<FRecallStateTreeTaskBase>(InScriptStruct);
}
