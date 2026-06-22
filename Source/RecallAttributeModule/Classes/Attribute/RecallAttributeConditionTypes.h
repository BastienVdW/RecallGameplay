// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "RecallAttributeConditionTypes.generated.h"

UENUM()
enum class ERecallAttributeConditionValue : uint8
{
	/// Use the attribute as a percent between 0 and 100 (or more if it can go above the max value)
	Percent,

	/// Use a fix value to compare the attribute.
	Value,
};

UENUM()
enum class ERecallAttributeConditionComparison : uint8
{
	None UMETA(Hidden),
	GreaterThan,
	LessThan,
	GreaterThanOrEqual,
	LessThanOrEqual,
	NotEqual,
	Equal,
	MAX UMETA(Hidden)
};

struct FRecallAttributeSet;

USTRUCT()
struct RECALLATTRIBUTEMODULE_API FRecallAttributeCondition
{
	GENERATED_BODY()

public:
	bool EvaluateCondition(const FRecallAttributeSet& AttributeSet) const;
	
protected:
	/**
	 * Attribute to evaluate.
	 */
	UPROPERTY(EditAnywhere, meta=(GameplayTagFilter="Attribute"))
	FGameplayTag Attribute;

	/**
	 * Will only evaluate children attributes.
	 */
	UPROPERTY(EditAnywhere)
	bool bUseChildrenAttributes = false;

	/**
	 * Comparison operator to test the attribute against the value.
	 */
	UPROPERTY(EditAnywhere)
	ERecallAttributeConditionComparison Comparison = ERecallAttributeConditionComparison::Equal;

	/**
	 * Value type to evaluate the attribute with.
	 */
	UPROPERTY(EditAnywhere)
	ERecallAttributeConditionValue Value = ERecallAttributeConditionValue::Value;

	UPROPERTY(EditAnywhere, meta=(EditCondition="Value == ERecallAttributeConditionValue::Value", EditConditionHides), DisplayName="Value")
	float DefaultValue = 0.0f;

	UPROPERTY(EditAnywhere, meta=(Units=Percent, EditCondition="Value == ERecallAttributeConditionValue::Percent", EditConditionHides, ClampMin="0.0", ClampMax="100.0"), DisplayName="Value")
	float PercentValue = 0.0f;

protected:
	bool EvaluateValue(float AttributeValue) const;
	float GetAttributeValue(const FGameplayTag& InAttribute, const FRecallAttributeSet& AttributeSet) const;
	
private:
	float GetComparisonValue() const;
};
