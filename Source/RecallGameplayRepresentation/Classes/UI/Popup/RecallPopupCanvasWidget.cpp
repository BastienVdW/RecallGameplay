// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallPopupCanvasWidget.h"

#include "Components/CanvasPanel.h"
#include "RecallFrontendUtils.h"
#include "RecallPopupWidget.h"

void URecallPopupCanvasWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	Recall::Frontend::Utils::RegisterGlobalObserver<IRecallUIPopupInterface>(this);
}

void URecallPopupCanvasWidget::NativeDestruct()
{
	Super::NativeDestruct();
	
	Recall::Frontend::Utils::UnregisterAllGlobalObservers(this);
}

void URecallPopupCanvasWidget::ShowPopup_Implementation(
	const FVector& Location, const FText& Popup, float Duration)
{
	if (!CanvasPanel_Popup)
	{
		return;
	}
	
	URecallPopupWidget* PopupWidget = Cast<URecallPopupWidget>(CreateWidgetInstance(
		*this, PopupWidgetClass, NAME_None));
	if (!IsValid(PopupWidget))
	{
		return;
	}

	CanvasPanel_Popup->AddChildToCanvas(PopupWidget);
	
	PopupWidget->ShowPopup(Location, Popup, Duration);
}
