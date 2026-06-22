// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Interact/RecallInteractConditionTypes.h"
#include "Attribute/RecallAttributeConditionTypes.h"

#include "RecallInteractAttributeConditionTypes.generated.h"

USTRUCT(DisplayName="Attribute")
struct RECALLINTERACTMODULE_API FRecallInteractAttributeCondition : public FRecallInteractCondition
{
	GENERATED_BODY()
	
public:
	virtual bool EvaluateCondition(const FRecallInteractContext& Context, FText& OutFailedText) const override;

protected:
	UPROPERTY(EditAnywhere, meta=(ShowOnlyInnerProperties))
	FRecallAttributeCondition Condition;

	UPROPERTY(EditAnywhere)
	ERecallInteractTarget Target = ERecallInteractTarget::Instigator;
};
