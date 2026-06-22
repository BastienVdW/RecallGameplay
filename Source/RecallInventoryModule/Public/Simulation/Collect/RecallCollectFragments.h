// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Mass/EntityElementTypes.h"
#include "Collect/RecallCollectTypes.h"

#include "RecallCollectFragments.generated.h"

/*
 * Resources can be collected from this location
 */
USTRUCT()
struct RECALLINVENTORYMODULE_API FRecallCollectPointFragment : public FMassFragment
{
	GENERATED_BODY()
	
public:
	// Owner who will collect this resource
	UPROPERTY(VisibleAnywhere)
	FGameplayTag OwnerInventoryTag;
};

USTRUCT()
struct RECALLINVENTORYMODULE_API FRecallCollectableFragment : public FMassFragment
{
	GENERATED_BODY()
	
public:
	/**
	 * Tag of the item to collect.
	 */
	UPROPERTY(EditAnywhere, meta=(GameplayTagFilter="Inventory"))
	FGameplayTag InventoryItem;

	/**
	 * How man items to collect.
	 */
	UPROPERTY(EditAnywhere)
	int32 ItemCount = 1;
};

USTRUCT()
struct RECALLINVENTORYMODULE_API FRecallCollectableConstSharedFragment : public FMassConstSharedFragment
{
	GENERATED_BODY()

	/**
	 * Settings for the collectable entity.
	 */
	UPROPERTY(VisibleAnywhere)
	FRecallCollectableSettings CollectableSettings;
};
