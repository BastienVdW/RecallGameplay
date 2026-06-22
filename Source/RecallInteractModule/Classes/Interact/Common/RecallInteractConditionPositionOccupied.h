// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "Interact/RecallInteractConditionTypes.h"

#include "RecallInteractConditionPositionOccupied.generated.h"

/**
 * Condition to validate if a specific interaction position is available or occupied.
 * Used to prevent multiple players from interacting at the same position simultaneously.
 */
USTRUCT(meta=(DisplayName="Position Occupied"))
struct RECALLINTERACTMODULE_API FRecallInteractConditionPositionOccupied : public FRecallInteractCondition
{
	GENERATED_BODY()

public:
	/**
	 * Position index to check for occupancy.
	 * - INDEX_NONE: Check interactor's current ClosestPositionIndex
	 * - 0+: Check specific position index
	 */
	UPROPERTY(EditAnywhere, Category="Condition", meta=(
		Tooltip="Position index to check. Use -1 to check the interactor's current closest position."))
	int32 PositionIndexToCheck = INDEX_NONE;

	/**
	 * If true, condition passes when occupied (inverted logic).
	 * - false (default): Condition PASSES when position is AVAILABLE (blocks occupied positions)
	 * - true: Condition PASSES when position is OCCUPIED (requires position to be occupied)
	 */
	UPROPERTY(EditAnywhere, Category="Condition", meta=(
		Tooltip="If true, requires position to be occupied instead of available."))
	bool bInvertCondition = false;

	virtual bool EvaluateCondition(const FRecallInteractContext& Context, FText& OutFailedText) const override;
};
