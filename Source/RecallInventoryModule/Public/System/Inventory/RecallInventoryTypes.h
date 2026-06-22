// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "GameplayTag/RecallGameplayTagTypes.h"

#include "RecallInventoryTypes.generated.h"

RECALLINVENTORYMODULE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Inventory_Item1);
RECALLINVENTORYMODULE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Inventory_Item2);
RECALLINVENTORYMODULE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Inventory_Item3);

/**
 * Data to keep track of items in the inventory.
 */
USTRUCT()
struct FRecallInventoryItem
{
	GENERATED_BODY()

public:
	FORCEINLINE int32 GetCount() const { return Count; }
	FORCEINLINE void Add(int32 Value) { Count = FMath::Max(Count, Count + Value); }
	FORCEINLINE void Sub(int32 Value) { Count = FMath::Clamp(Count - Value, 0, Count); }
	FORCEINLINE bool IsEmpty() const { return Count == 0; }
	
protected:
	UPROPERTY(VisibleAnywhere)
	int32 Count = 0;
};

USTRUCT()
struct RECALLINVENTORYMODULE_API FRecallInventoryRegistry
{
	GENERATED_BODY()

	// Multiple inventories can be stored
	UPROPERTY(VisibleAnywhere)
	TMap<FGameplayTag, FRecallGameplayTagCountMap> Inventories;

	FORCEINLINE void Reset()
	{
		Inventories.Reset();
	}
};
