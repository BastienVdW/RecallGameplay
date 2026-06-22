// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

#include "RecallEquipmentTypes.generated.h"

RECALLINVENTORYMODULE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(EquipmentSlot_Head);
RECALLINVENTORYMODULE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(EquipmentSlot_LHand);
RECALLINVENTORYMODULE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(EquipmentSlot_RHand);
RECALLINVENTORYMODULE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(EquipmentSlot_LFeet);
RECALLINVENTORYMODULE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(EquipmentSlot_RFeet);

USTRUCT()
struct RECALLINVENTORYMODULE_API FRecallEquipmentSlot
{
    GENERATED_BODY()
    
	UPROPERTY(EditAnywhere, meta=(GameplayTagFilter="Inventory"))
	FGameplayTag Item;
	
	FORCEINLINE bool IsEmpty() const { return !Item.IsValid(); }
};

USTRUCT()
struct RECALLINVENTORYMODULE_API FRecallEquipment
{
    GENERATED_BODY()

public:
	FORCEINLINE void SetEquipmentSlot(const FGameplayTag& EquipSlot, const FGameplayTag& InventoryItem)
	{
		if (!ensureAlwaysMsgf(EquipSlot.IsValid(), TEXT("Invalid equip slot")))
		{
			return;
		}
		
		Slots.Add(EquipSlot).Item = InventoryItem;
	}
	
	FORCEINLINE FGameplayTag ClearEquipmentSlot(const FGameplayTag& EquipSlot)
	{
		if (!ensureAlwaysMsgf(EquipSlot.IsValid(), TEXT("Invalid equip slot")) ||
			!ensureAlwaysMsgf(!IsEmptySlot(EquipSlot), TEXT("Slot <%s> is already empty"), *EquipSlot.ToString()))
		{
			return FGameplayTag();
		}

		return Slots.FindAndRemoveChecked(EquipSlot).Item;
	}
	
	FORCEINLINE void SetEquipmentSlot(const FGameplayTag& EquipSlot, const FRecallEquipmentSlot& Slot)
	{
		if (!ensureAlwaysMsgf(EquipSlot.IsValid(), TEXT("Invalid equip slot")))
		{
			return;
		}
		
		Slots.Add(EquipSlot, Slot);
	}
	
	FORCEINLINE bool GetEquipmentSlot(const FGameplayTag& EquipSlot, FGameplayTag& OutItemTag) const
	{
		if (const FRecallEquipmentSlot* SlotPtr = Slots.Find(EquipSlot))
		{
			OutItemTag = SlotPtr->Item;
			return true;
		}
		return false;
	}
	
	FORCEINLINE bool IsEmptySlot(const FGameplayTag& EquipmentSlot) const
	{
		return !EquipmentSlot.IsValid() || Slots.FindRef(EquipmentSlot).IsEmpty();
	}

protected:    
	UPROPERTY(EditAnywhere, meta=(GameplayTagFilter="EquipSlot"))
	TMap<FGameplayTag, FRecallEquipmentSlot> Slots;
};

USTRUCT()
struct RECALLINVENTORYMODULE_API FRecallEquipmentSettings
{
    GENERATED_BODY()

	/**
	 * Auto-equip new items whenever possible.
	 */
	UPROPERTY(EditAnywhere)
	bool bAutoEquip = true;

	/**
	 * List of the equip slots for this entity.
	 */
	UPROPERTY(EditAnywhere, meta=(GameplayTagFilter="EquipSlot"))
	FGameplayTagContainer EquipmentSlots;
};