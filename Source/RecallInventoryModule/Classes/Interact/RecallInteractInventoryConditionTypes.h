// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Interact/RecallInteractConditionTypes.h"
#include "GameplayTagContainer.h"

#include "RecallInteractInventoryConditionTypes.generated.h"

USTRUCT(DisplayName="Required Item")
struct RECALLINVENTORYMODULE_API FRecallInteractInventoryCondition : public FRecallInteractCondition
{
	GENERATED_BODY()
	
public:
	FRecallInteractInventoryCondition();
	
	virtual bool EvaluateCondition(const FRecallInteractContext& Context, FText& OutFailedText) const override;

protected:
	/**
	 * Tag of the item required to interact with this interactable.
	 */
	UPROPERTY(EditAnywhere, meta=(GameplayTagFilter="Inventory"))
	FGameplayTag ItemTag;

	/**
	 * How many items are required.
	 */
	UPROPERTY(EditAnywhere)
	int32 ItemCount = 1;

	/**
	 * Text returned to the interact widget when interact condition fails.
	 * The item tag will be passed as the first argument.
	 */
	UPROPERTY(EditAnywhere)
	FText FailedText;
};
