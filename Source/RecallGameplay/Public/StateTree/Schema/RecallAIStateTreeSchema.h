// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "StateTree/RecallStateTreeSchemaBase.h"

#include "RecallAIStateTreeSchema.generated.h"

/**
 * StateTree for Recall behaviors.
 */
UCLASS(BlueprintType, EditInlineNew, CollapseCategories, meta=(DisplayName="RE AI Behavior", CommonSchema))
class RECALLGAMEPLAY_API URecallAIStateTreeSchema : public URecallStateTreeSchemaBase
{
	GENERATED_BODY()

public:
	URecallAIStateTreeSchema();

protected:
	virtual bool IsStructAllowed(const UScriptStruct* InScriptStruct) const override;
};
