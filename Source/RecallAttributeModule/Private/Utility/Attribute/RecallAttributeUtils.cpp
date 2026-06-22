// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "Utility/Attribute/RecallAttributeUtils.h"

#include "Attribute/RecallAttributeConditionTypes.h"
#include "Attribute/RecallAttributeSetTypes.h"
#include "Representation/Attribute/RecallAttributeRepresentationTypes.h"

namespace Recall::Attribute::Utils
{

bool EvaluateCondition(const FRecallAttributeCondition& Condition, const FRecallAttributeSet& AttributeSet)
{
	return Condition.EvaluateCondition(AttributeSet);
}

TMap<FGameplayTag, FRecallAttributeRepresentation> GetAttributeRepresentationMap(
	const FRecallAttributeSet& AttributeSet)
{
	TArray<FGameplayTag> Tags;
	AttributeSet.GetAttributeTagArray(Tags);

	TMap<FGameplayTag, FRecallAttributeRepresentation> AttributeMap;
	AttributeMap.Reserve(Tags.Num());

	for (const FGameplayTag& Tag : Tags)
	{
		FRecallAttributeRepresentation& AttributeRepresentation = AttributeMap.Add(Tag);
		AttributeRepresentation.CurrentValue = AttributeSet.GetValue(Tag);
		AttributeRepresentation.MinValue = AttributeSet.GetMinValue(Tag);
		AttributeRepresentation.MaxValue = AttributeSet.GetMaxValue(Tag);
	}

	return AttributeMap;
}
} // Recall::Attribute::Utils
