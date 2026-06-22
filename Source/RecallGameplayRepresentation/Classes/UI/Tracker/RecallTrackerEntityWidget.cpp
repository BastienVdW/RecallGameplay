// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallTrackerEntityWidget.h"

#include "Components/CanvasPanel.h"
#include "RecallFrontendUtils.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/GameplayStatics.h"
#include "Representation/Tracker/RecallTrackerEntityInterface.h"
#include "Representation/Tracker/RecallTrackerRepresentationTypes.h"

void URecallTrackerEntityWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Recall::Frontend::Utils::RegisterGlobalObserver<IRecallTrackerReactInterface>(this);
}

void URecallTrackerEntityWidget::NativeDestruct()
{
	Super::NativeDestruct();

	Recall::Frontend::Utils::UnregisterAllGlobalObservers(this);
}

void URecallTrackerEntityWidget::SetTrackedEntities(
	const TArray<FRecallTrackedEntityRepresentation>& TrackedEntityRepresentations)
{
	CacheNewTrackedEntities.Reserve(TrackedEntityRepresentations.Num());
	Algo::Transform(TrackedEntityRepresentations, CacheNewTrackedEntities,
		[](const FRecallTrackedEntityRepresentation& TrackedEntityRepresentation)
	{
		return TrackedEntityRepresentation.Entity;
	});

	ActiveTrackerWidgets.GetKeys(CacheActiveTrackedEntities);

	CacheReleaseEntities.Reset();
	for (int32 EntityIndex = CacheActiveTrackedEntities.Num() - 1; EntityIndex >= 0; --EntityIndex)
	{
		const FMassExtendedEntityHandle& ActiveTrackedEntity = CacheActiveTrackedEntities[EntityIndex];
		if (CacheNewTrackedEntities.Contains(ActiveTrackedEntity))
		{
			CacheNewTrackedEntities.Remove(ActiveTrackedEntity);
		}
		else
		{
			CacheReleaseEntities.Add(ActiveTrackedEntity);
		}
	}

	ReleaseTrackers(CacheReleaseEntities);

	CacheNewTrackedEntityRepresentations.Reset(CacheNewTrackedEntities.Num());
	for (int32 EntityIndex = 0; EntityIndex < TrackedEntityRepresentations.Num(); ++EntityIndex)
	{
		const FRecallTrackedEntityRepresentation& TrackedEntityRepresentation = TrackedEntityRepresentations[EntityIndex];
		if (CacheNewTrackedEntities.Contains(TrackedEntityRepresentation.Entity))
		{
			CacheNewTrackedEntityRepresentations.Add(TrackedEntityRepresentation);
		}
	}
	
	CreateTrackers(CacheNewTrackedEntityRepresentations);
	
	UpdateTrackersPosition(TrackedEntityRepresentations);
}

void URecallTrackerEntityWidget::CreateTrackers(
	const TArray<FRecallTrackedEntityRepresentation>& NewTrackedEntities)
{
	if (!CanvasPanel_Track)
	{
		return;
	}

	for (const FRecallTrackedEntityRepresentation& NewTrackedEntity : NewTrackedEntities)
	{
		UUserWidget* NewWidget = nullptr;
		
		FRecallTrackerEntityWidgetPool& WidgetPool = PoolTrackerWidgets.FindOrAdd(NewTrackedEntity.TrackerWidgetClass);
		if (WidgetPool.Widgets.Num())
		{
			NewWidget = WidgetPool.Widgets.Pop(EAllowShrinking::No);
		}
		else
		{
			NewWidget = CreateWidget(GetOwningPlayer(), NewTrackedEntity.TrackerWidgetClass);
		}

		if (IsValid(NewWidget))
		{
			IRecallTrackerEntityInterface::Execute_SetTrackedEntity(NewWidget, NewTrackedEntity.Entity);
			
			if (UCanvasPanelSlot* CanvasPanelSlot =  CanvasPanel_Track->AddChildToCanvas(NewWidget))
			{
				CanvasPanelSlot->SetAlignment(FVector2D(0.5f, 0.5f));
				CanvasPanelSlot->SetPosition(FVector2D(0.f, 0.f));
				CanvasPanelSlot->SetAutoSize(true);
				CanvasPanelSlot->SetAnchors(FAnchors());
			}
			
			NewWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
		
		ActiveTrackerWidgets.Add(NewTrackedEntity.Entity, NewWidget);
	}
}

void URecallTrackerEntityWidget::ReleaseTrackers(
	const TArray<FMassExtendedEntityHandle>& TrackedEntities)
{
	if (!CanvasPanel_Track)
	{
		return;
	}
	
	for (const FMassExtendedEntityHandle& TrackedEntity : TrackedEntities)
	{
		const TObjectPtr<UUserWidget> TrackerWidget = ActiveTrackerWidgets.FindAndRemoveChecked(TrackedEntity);
		CanvasPanel_Track->RemoveChild(TrackerWidget);
		PoolTrackerWidgets.FindOrAdd(TrackerWidget->GetClass()).Widgets.Add(TrackerWidget);
	}
}

void URecallTrackerEntityWidget::UpdateTrackersPosition(
	const TArray<FRecallTrackedEntityRepresentation>& TrackedEntities)
{
	for (const FRecallTrackedEntityRepresentation& TrackedEntity : TrackedEntities)
	{
		const TObjectPtr<UUserWidget>& Widget = ActiveTrackerWidgets.FindChecked(TrackedEntity.Entity);
		if (!Widget)
		{
			continue;
		}

		FVector2D ScreenPosition = FVector2D::ZeroVector;
		UGameplayStatics::ProjectWorldToScreen(GetOwningPlayer(), TrackedEntity.Location, ScreenPosition, true);

		const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(GetOwningPlayer());
		if (!FMath::IsWithinInclusive(ScreenPosition.X, 0.0, ViewportSize.X) ||
			!FMath::IsWithinInclusive(ScreenPosition.Y, 0.0, ViewportSize.Y))
		{
			Widget->SetVisibility(ESlateVisibility::Collapsed);
			continue;
		}

		FVector2D ViewportPosition = FVector2D::ZeroVector;
		USlateBlueprintLibrary::ScreenToViewport(GetOwningPlayer(), ScreenPosition, ViewportPosition);

		if (UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(Widget->Slot))
		{
			CanvasPanelSlot->SetPosition(ViewportPosition);
		}
		
		Widget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}
