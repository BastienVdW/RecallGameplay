// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallPopupWidget.h"

#include "Animation/WidgetAnimation.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "CommonRichTextBlock.h"
#include "Kismet/GameplayStatics.h"

void URecallPopupWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void URecallPopupWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void URecallPopupWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateViewportPosition();
}

void URecallPopupWidget::ShowPopup(const FVector& Location, const FText& Popup, float Duration)
{
	PopupLocation = Location;
	UpdateViewportPosition();
	
	if (CommonRichTextBlock_Text)
	{
		CommonRichTextBlock_Text->SetText(Popup);
	}

	if (Animation_PopupFadeIn)
	{
		PlayAnimationForward(Animation_PopupFadeIn, 1.0f, false);		
	}

	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(PopupTimerHandle, this, &ThisClass::OnPopupTimeOut,
			Duration, false);
	}
}

void URecallPopupWidget::OnPopupTimeOut()
{
	if (Animation_PopupFadeOut)
	{
		PlayAnimationForward(Animation_PopupFadeOut, 1.0f, false);

		if (const UWorld* World = GetWorld())
		{
			const float AnimDuration = Animation_PopupFadeOut->GetEndTime() - Animation_PopupFadeOut->GetStartTime();
			FTimerHandle DummyTimer;
			World->GetTimerManager().SetTimer(DummyTimer, this, &ThisClass::RemoveFromParent,
				AnimDuration, false);
		}
	}
	else
	{
		RemoveFromParent();
	}
}

void URecallPopupWidget::UpdateViewportPosition() const
{	
	FVector2D ScreenPosition = FVector2D::ZeroVector;
	UGameplayStatics::ProjectWorldToScreen(GetOwningPlayer(), PopupLocation, ScreenPosition, true);

	FVector2D ViewportPosition = FVector2D::ZeroVector;
	USlateBlueprintLibrary::ScreenToViewport(GetOwningPlayer(), ScreenPosition, ViewportPosition);

	if (UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(Slot))
	{
		CanvasPanelSlot->SetPosition(ViewportPosition);
	}
}
