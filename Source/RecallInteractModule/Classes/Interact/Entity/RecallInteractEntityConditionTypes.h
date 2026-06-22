// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Interact/RecallInteractConditionTypes.h"

#include "RecallInteractEntityConditionTypes.generated.h"

USTRUCT(DisplayName="Has Children")
struct RECALLINTERACTMODULE_API FRecallInteractHasChildrenCondition : public FRecallInteractCondition
{
	GENERATED_BODY()
	
public:
	virtual bool EvaluateCondition(const FRecallInteractContext& Context, FText& OutFailedText) const override;
	
protected:
	UPROPERTY(EditAnywhere)
	ERecallInteractTarget Target = ERecallInteractTarget::Owner;
	
	UPROPERTY(EditAnywhere)
	bool bInvert = false;
	
	UPROPERTY(EditAnywhere)
	FText FailedText;
};

/**
 * Validates target selection based on controller rotation and interactable positioning.
 * Uses the instigator's controller rotation to determine if they are "looking at" the interactable.
 */
USTRUCT(DisplayName="Control Target Condition")
struct RECALLINTERACTMODULE_API FRecallControlTargetCondition : public FRecallInteractCondition
{
	GENERATED_BODY()
	
public:
	virtual bool EvaluateCondition(const FRecallInteractContext& Context, FText& OutFailedText) const override;
	
protected:
	// Maximum angle from forward direction to interactable (in degrees)
	UPROPERTY(EditAnywhere, Category="Targeting", meta=(ClampMin="1.0", ClampMax="180.0", Units=Degrees))
	float MaxTargetingAngle = 15.0f;
	
	// Whether to use controller rotation forward vector (true) or instigator entity forward vector (false)
	UPROPERTY(EditAnywhere, Category="Targeting")
	bool bUseControlRotation = true;
	
	// Whether to project onto interactable's bounding box for more accurate targeting
	UPROPERTY(EditAnywhere, Category="Targeting")
	bool bUseBoundingBoxProjection = true;
};
