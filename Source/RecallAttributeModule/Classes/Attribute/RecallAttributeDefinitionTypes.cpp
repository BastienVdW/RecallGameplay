// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "Attribute/RecallAttributeDefinitionTypes.h"

#include "RecallAttributeSetTypes.h"

DEFINE_LOG_CATEGORY(LogRecallAttribute);

//----------------------------------------------------------------------//
// FRecallAttributeBoundDefinition
//----------------------------------------------------------------------//
FRecallAttributeBoundDefinition::FRecallAttributeBoundDefinition(float InDefaultValue)
	: DefaultValue(InDefaultValue)
{
}

float FRecallAttributeBoundDefinition::GetValue(const FRecallAttributeSet& AttributeSet) const
{
	if (Attribute.IsValid())
	{
		return AttributeSet.GetValue(Attribute, DefaultValue);
	}
	return DefaultValue;
}

//----------------------------------------------------------------------//
// URecallAttributeSetAsset
//----------------------------------------------------------------------//
#if WITH_EDITOR
void URecallAttributeSetAsset::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	if (PropertyChangedEvent.GetPropertyName() == TEXT("Parent"))
	{
		ValidateParent_Internal();
	}
}

void URecallAttributeSetAsset::ValidateParent_Internal()
{
	TSet<URecallAttributeSetAsset*> Parents;

	URecallAttributeSetAsset* AttributeSet = this;
	while (IsValid(AttributeSet))
	{
		if (Parents.Contains(AttributeSet))
		{
			Parent = nullptr;
			Modify();
			
			UE_LOG(LogRecallAttribute, Error, TEXT("Circular reference detected in attribute set %s"), *GetName());
			return;
		}

		Parents.Add(AttributeSet);
		AttributeSet = AttributeSet->Parent;
	}
}
#endif // WITH_EDITOR

const FRecallAttributeDefinition& URecallAttributeSetAsset::GetAttributeChecked(const FGameplayTag& Attribute) const
{
	TObjectPtr<const URecallAttributeSetAsset> AttributeSet = this;
	while (AttributeSet)
	{
		if (const FRecallAttributeDefinition* AttributeDefinitionPtr = AttributeSet->Attributes.Find(Attribute))
		{
			return *AttributeDefinitionPtr;
		}

		AttributeSet = AttributeSet->Parent;
	}

	checkNoEntry();
	return Attributes.FindChecked(Attribute);
}

TArray<TSubclassOf<URecallAttributeMod>> URecallAttributeSetAsset::GetDefaultModClasses() const
{
	TArray<TSubclassOf<URecallAttributeMod>>  Results;
	
	TObjectPtr<const URecallAttributeSetAsset> AttributeSet = this;
	while (AttributeSet)
	{
		Results.Append(AttributeSet->DefaultModClasses);
		AttributeSet = AttributeSet->Parent;
	}
	
	return Results;
}

TArray<FGameplayTag> URecallAttributeSetAsset::GetAttributeTags() const
{
	TSet<FGameplayTag> Results;

	TObjectPtr<const URecallAttributeSetAsset> AttributeSet = this;
	while (AttributeSet)
	{
		TArray<FGameplayTag> NewAttributes;
		AttributeSet->Attributes.GenerateKeyArray(NewAttributes);

		Results.Append(NewAttributes);

		AttributeSet = AttributeSet->Parent;
	}

	return Results.Array();
}
