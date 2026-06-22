// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallAttributeConditionTypes.h"

#include "RecallAttributeSetTypes.h"

bool FRecallAttributeCondition::EvaluateCondition(const FRecallAttributeSet& AttributeSet) const
{
	TArray<FGameplayTag> Tags;

	if (bUseChildrenAttributes)
	{
		const FGameplayTagContainer TagContainer = UGameplayTagsManager::Get().RequestGameplayTagChildren(Attribute);
		TagContainer.GetGameplayTagArray(Tags);
	}
	else
	{
		Tags.Add(Attribute);
	}

	for (const FGameplayTag& Tag : Tags)
	{
		const float Result = GetAttributeValue(Tag, AttributeSet);
		if (EvaluateValue(Result))
		{
			return true;
		}
	}
	
	return false;
}

float FRecallAttributeCondition::GetAttributeValue(const FGameplayTag& InAttribute, const FRecallAttributeSet& AttributeSet) const
{
	switch (Value)
	{
	case ERecallAttributeConditionValue::Value:
		return AttributeSet.GetValue(InAttribute);

	case ERecallAttributeConditionValue::Percent:
		return AttributeSet.GetValueAsPercent(InAttribute) * 100.0f;

	default:
		unimplemented();
		return 0.0f;
	}
}

float FRecallAttributeCondition::GetComparisonValue() const
{
	switch (Value)
	{
	case ERecallAttributeConditionValue::Value:
		return DefaultValue;
		
	case ERecallAttributeConditionValue::Percent:
		return PercentValue;

	default:
		unimplemented();
		return 0.0f;
	}
}

bool FRecallAttributeCondition::EvaluateValue(float AttributeValue) const
{
	const float ComparisonValue = GetComparisonValue();
	
	switch (Comparison)
	{
	case ERecallAttributeConditionComparison::Equal:
		return FMath::IsNearlyEqual(ComparisonValue, AttributeValue);

	case ERecallAttributeConditionComparison::NotEqual:
		return !FMath::IsNearlyEqual(ComparisonValue, AttributeValue);
		
	case ERecallAttributeConditionComparison::GreaterThan:
		return AttributeValue > ComparisonValue;

	case ERecallAttributeConditionComparison::GreaterThanOrEqual:
		return AttributeValue >= ComparisonValue;

	case ERecallAttributeConditionComparison::LessThan:
		return AttributeValue < ComparisonValue;

	case ERecallAttributeConditionComparison::LessThanOrEqual:
		return AttributeValue <= ComparisonValue;

	case ERecallAttributeConditionComparison::None:
	case ERecallAttributeConditionComparison::MAX:
		checkNoEntry();
		return false;
		
	default:
		unimplemented();
		return false;
	}
}
