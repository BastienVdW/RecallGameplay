// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallAttributeWidget.h"

#include "CommonRichTextBlock.h"
#include "Components/ProgressBar.h"

#define LOCTEXT_NAMESPACE "URecallAttributeWidget"

URecallAttributeWidget::URecallAttributeWidget()
	: Super()
{
	AttributeText = NSLOCTEXT("URecallAttributeWidget", "AttributeValue", "{CurrentValue} / {MaxValue}");
}

void URecallAttributeWidget::NativeConstruct()
{
	Super::NativeConstruct();	
}

void URecallAttributeWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

bool URecallAttributeWidget::DoesSupportAttribute(const FGameplayTag& InAttribute) const
{
	const FGameplayTagQuery Query = FGameplayTagQuery::MakeQuery_MatchAnyTags(Attributes);	
	return Query.Matches(InAttribute.GetSingleTagContainer());
}

bool URecallAttributeWidget::IsAttributeVisible(const FRecallAttributeRepresentation& InRepresentation) const
{
	if (EnumHasAnyFlags(AttributeVisibility, ERecallAttributeVisibility::HideEmpty) && InRepresentation.IsEmpty())
	{
		return false;
	}
	if (EnumHasAnyFlags(AttributeVisibility, ERecallAttributeVisibility::HideFull) && InRepresentation.IsFull())
	{
		return false;
	}
	return true;
}

void URecallAttributeWidget::SetAttribute(const FGameplayTag& InAttribute,
                                            const FRecallAttributeRepresentation& InRepresentation)
{
	SetAttributeText(InAttribute, InRepresentation.CurrentValue,
		InRepresentation.MinValue, InRepresentation.MaxValue, InRepresentation.GetPercent());
}

void URecallAttributeWidget::SetAttributeText(FGameplayTag InAttribute, float CurrentValue,
	float MinValue, float MaxValue, float Percent)
{
	if (ProgressBar_Attribute)
	{
		ProgressBar_Attribute->SetPercent(Percent);
	}
	
	if (CommonRichTextBlock_Attribute)
	{
		FFormatNamedArguments Args;
		Args.Add("AttributeName", FText::FromString(InAttribute.ToString()));
		Args.Add("CurrentValue", CurrentValue);
		Args.Add("MinValue", MinValue);
		Args.Add("MaxValue", MaxValue);
		Args.Add("Percent", FMath::CeilToInt(Percent * 100.0f));

		const FText ResultText = FText::Format(AttributeText, Args);
		
		CommonRichTextBlock_Attribute->SetText(ResultText);
	}
}

#undef LOCTEXT_NAMESPACE
