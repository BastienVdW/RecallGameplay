// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "MassExtendedEntityTypes.h"
#include "System/Actor/RecallActorTypes.h"
#include "RecallEquipmentTypes.h"

#include "RecallInventoryFragments.generated.h"

class URecallInventoryItemAsset;

USTRUCT()
struct RECALLINVENTORYMODULE_API FRecallEquipSlotRepresentation
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	FRecallActorHandle ActorHandle;
};

/**
 * Fragment that contains the equipment carried by an entity.
 */
USTRUCT()
struct RECALLINVENTORYMODULE_API FRecallEquipmentFragment : public FMassExtendedFragment
{
	GENERATED_BODY()

	/**
	 * The equip slot currently being selected.
	 */
	UPROPERTY(VisibleAnywhere, meta=(GameplayTagFilter="EquipSlot"))
	FGameplayTag SelectedEquipSlot;

	/**
	 * Manage entity equipment.
	 */
	UPROPERTY(VisibleAnywhere)
	FRecallEquipment Equipment;

	/**
	 * Keep track of the representation of each equipped item.
	 */
	UPROPERTY(VisibleAnywhere, meta=(GameplayTagFilter="EquipSlot"))
	TMap<FGameplayTag, FRecallEquipSlotRepresentation> EquipSlotRepresentationMap;

	/**
	 * Used by the equipment processor to cache which items are currently equipped.
	 */
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<const URecallInventoryItemAsset>> EquippedItems;
};

USTRUCT()
struct RECALLINVENTORYMODULE_API FRecallEquipmentConstSharedFragment : public FMassExtendedConstSharedFragment
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	FRecallEquipmentSettings Settings;
};
