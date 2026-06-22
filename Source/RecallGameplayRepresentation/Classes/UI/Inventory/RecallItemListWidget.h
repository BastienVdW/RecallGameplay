// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "ExtendedCommonUserWidget.h"
#include "Observer/Representation/RecallRepresentationReactInterface.h"
#include "Representation/Tracker/RecallTrackerEntityInterface.h"
#include "GameplayTagContainer.h"
#include "Mass/EntityHandle.h"

#include "RecallItemListWidget.generated.h"

/**
 * Widget to show the inventory of the owning player.
 * Can also be used to track an entity and show its inventory.
 */
UCLASS(Abstract)
class RECALLGAMEPLAYREPRESENTATION_API URecallItemListWidget :
	public UExtendedCommonUserWidget,
	public IRecallTrackerEntityInterface,
	public IRecallRepresentationReactInterface
{
	GENERATED_BODY()

	// IRecallRepresentationReactInterface implementation Begin
public:
	virtual void OnRender() override;
	// IRecallRepresentationReactInterface implementation End
	
	// IRecallTrackerEntityInterface implementation Begin
public:
	virtual void SetTrackedEntity_Implementation(const FMassEntityHandle& Entity) override;
	// IRecallTrackerEntityInterface implementation End

	// UUserWidget implementation Begin
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	// UUserWidget implementation End

protected:
	/**
	 * Item widget.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class URecallGameplayTagCountWidget> ItemWidgetClass;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<class UCommonHierarchicalScrollBox> CommonHierarchicalScrollBox_Inventory;
	
	UPROPERTY(Transient, BlueprintReadOnly, meta=(GameplayTagFilter="Inventory"))
	FGameplayTagContainer InventoryTags;

	UPROPERTY(Transient, BlueprintReadOnly, meta=(ExposeOnSpawn))
	FMassEntityHandle TrackedEntity;
	
	void RefreshInventory();
};
