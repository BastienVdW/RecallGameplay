// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallGameplayEffectListWidget.h"

#include "Components/PanelWidget.h"
#include "RecallGameplayEffectWidget.h"

URecallGameplayEffectListWidget::URecallGameplayEffectListWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void URecallGameplayEffectListWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void URecallGameplayEffectListWidget::SetFromGameplayEffects(
	const TArray<FRecallGameplayEffectRepresentation>& GameplayEffects) const
{
	if (!PanelWidget_List)
	{
		return;
	}

	PanelWidget_List->ClearChildren();
	
	for (const FRecallGameplayEffectRepresentation& GameplayEffect : GameplayEffects)
	{
		URecallGameplayEffectWidget* GameplayEffectWidget = CreateWidget<URecallGameplayEffectWidget>(
			GetOwningPlayer(), GameplayEffectWidgetClass);
		if (!GameplayEffectWidget)
		{
			continue;
		}

		GameplayEffectWidget->SetFromGameplayEffect(GameplayEffect);
		PanelWidget_List->AddChild(GameplayEffectWidget);
	}
}
