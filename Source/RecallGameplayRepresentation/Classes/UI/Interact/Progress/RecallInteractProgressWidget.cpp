// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallInteractProgressWidget.h"

#include "Blueprint/SlateBlueprintLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"

void URecallInteractProgressWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	CanvasPanelSlot = Cast<UCanvasPanelSlot>(Slot);
	if (CanvasPanelSlot)
	{
		CanvasPanelSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		CanvasPanelSlot->SetPosition(FVector2D(0.f, 0.f));
		CanvasPanelSlot->SetAnchors(FAnchors());
	}

}

void URecallInteractProgressWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void URecallInteractProgressWidget::SetInteraction(const FRecallInteractProgress& Interaction)
{
	if (ProgressBar_Interact)
	{
		ProgressBar_Interact->SetPercent(Interaction.Progress);
	}

	FVector2D ScreenPosition = FVector2D::ZeroVector;
	UGameplayStatics::ProjectWorldToScreen(GetOwningPlayer(), Interaction.Location, ScreenPosition, true);

	FVector2D ViewportPosition = FVector2D::ZeroVector;
	USlateBlueprintLibrary::ScreenToViewport(this, ScreenPosition, ViewportPosition);

	if (CanvasPanelSlot != nullptr)
	{
		CanvasPanelSlot->SetPosition(ViewportPosition);
	}
}
