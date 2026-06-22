// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallInteractProgressTrackWidget.h"

#include "RecallFrontendUtils.h"
#include "RecallInteractProgressWidget.h"
#include "Components/CanvasPanel.h"

void URecallInteractProgressTrackWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (CanvasPanel_Track)
	{
		ProgressWidgets.SetNum(MaxProgressWidgetCount);

		for (int32 WidgetIndex = 0; WidgetIndex < MaxProgressWidgetCount; ++WidgetIndex)
		{
			if (ProgressWidgetClass)
			{
				URecallInteractProgressWidget* NewWidget = CreateWidget<URecallInteractProgressWidget>(GetOwningPlayer(), ProgressWidgetClass);
				if (IsValid(NewWidget))
				{
					CanvasPanel_Track->AddChild(NewWidget);
					NewWidget->SetVisibility(ESlateVisibility::Visible);
					ProgressWidgets[WidgetIndex] = NewWidget;
				}
			}
		}
	}

	Recall::Frontend::Utils::RegisterGlobalObserver<IRecallInteractProgressReactInterface>(this);
}

void URecallInteractProgressTrackWidget::NativeDestruct()
{
	Super::NativeDestruct();

	Recall::Frontend::Utils::UnregisterAllGlobalObservers(this);
}

void URecallInteractProgressTrackWidget::SetInteractions(const TArray<FRecallInteractProgress>& Interactions)
{
	// TODO: Better pooling
	for (int32 WidgetIndex = 0; WidgetIndex < MaxProgressWidgetCount; ++WidgetIndex)
	{
		const TObjectPtr<URecallInteractProgressWidget>& ProgressWidget = ProgressWidgets[WidgetIndex];
		if (!ProgressWidget)
		{
			continue;
		}
		
		if (Interactions.IsValidIndex(WidgetIndex))
		{
			const FRecallInteractProgress& Interaction = Interactions[WidgetIndex];
			ProgressWidget->SetInteraction(Interaction);
			ProgressWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else
		{
			ProgressWidget->SetVisibility(ESlateVisibility::Collapsed);			
		}
	}
}
