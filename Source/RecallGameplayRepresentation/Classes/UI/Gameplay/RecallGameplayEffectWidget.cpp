// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallGameplayEffectWidget.h"

#include "CommonLazyImage.h"
#include "CommonRichTextBlock.h"
#include "Representation/Gameplay/RecallGameplayEffectRepresentation.h"

URecallGameplayEffectWidget::URecallGameplayEffectWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void URecallGameplayEffectWidget::SetFromGameplayEffect(
	const FRecallGameplayEffectRepresentation& GameplayEffect)
{
	if (CommonRichTextBlock_Name)
	{
		CommonRichTextBlock_Name->SetText(GameplayEffect.EffectName);
	}

	if (CommonRichTextBlock_Description)
	{
		CommonRichTextBlock_Description->SetText(GameplayEffect.EffectDescription);
	}

	if (CommonLazyImage_Icon)
	{
		CommonLazyImage_Icon->SetBrushFromSoftTexture(GameplayEffect.IconTexture);
	}

	if (CommonRichTextBlock_StackCount)
	{
		const TMap<FString, FStringFormatArg> Args =
		{
			{ TEXT("StackCount"), GameplayEffect.StackCount }
		};	
				
		CommonRichTextBlock_StackCount->SetText(FText::FromString(FString::Format(*StackCountTextFormat, Args)));
	}
}
