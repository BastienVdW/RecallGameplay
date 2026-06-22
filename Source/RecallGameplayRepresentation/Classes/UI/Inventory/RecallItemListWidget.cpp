// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallItemListWidget.h"

#include "CommonHierarchicalScrollBox.h"
#include "RecallFrontendUtils.h"
#include "UI/GameplayTag/RecallGameplayTagCountWidget.h"
#include "Utility/Representation/RecallGameplayRepresentationFunctionLibrary.h"

void URecallItemListWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	Recall::Frontend::Utils::RegisterGlobalObserver<IRecallRepresentationReactInterface>(this);
}

void URecallItemListWidget::NativeDestruct()
{
	Super::NativeDestruct();
	
	Recall::Frontend::Utils::UnregisterAllGlobalObservers(this);
}

void URecallItemListWidget::SetTrackedEntity_Implementation(const FMassExtendedEntityHandle& Entity)
{
	TrackedEntity = Entity;
}

void URecallItemListWidget::OnRender()
{
	const FGameplayTag InventoryTagParent = FGameplayTag::RequestGameplayTag(TEXT("Inventory"));

	// Use owning player's entity if not set
	if (!TrackedEntity.IsValid())
	{
		const int32 PlayerIndex = GetOwningLocalPlayerIndex();
		const FString PlayerID = URecallGameplayRepresentationFunctionLibrary::GetLocalPlayerID(
			this, PlayerIndex);
		TrackedEntity = URecallGameplayRepresentationFunctionLibrary::GetPlayerEntity(
			this, PlayerID);
	}
	
	const FGameplayTagContainer NewInventoryTags = URecallGameplayRepresentationFunctionLibrary::GetSubGameplayTagsByEntity(
		this, InventoryTagParent, TrackedEntity);
	if (InventoryTags != NewInventoryTags)
	{
		InventoryTags = NewInventoryTags;
		RefreshInventory();
	}
}

void URecallItemListWidget::RefreshInventory()
{
	if (!CommonHierarchicalScrollBox_Inventory)
	{
		return;
	}
	
	CommonHierarchicalScrollBox_Inventory->ClearChildren();

	TArray<FGameplayTag> ItemTags;
	InventoryTags.GetGameplayTagArray(ItemTags);

	for (const FGameplayTag& ItemTag : ItemTags)
	{
		auto* NewWidget = CreateWidget<URecallGameplayTagCountWidget>(
			GetOwningPlayer(), ItemWidgetClass);
		if (IsValid(NewWidget))
		{
			NewWidget->SetEntity(TrackedEntity);
			NewWidget->SetGameplayTag(ItemTag);
			CommonHierarchicalScrollBox_Inventory->AddChild(NewWidget);
		}
	}
}
