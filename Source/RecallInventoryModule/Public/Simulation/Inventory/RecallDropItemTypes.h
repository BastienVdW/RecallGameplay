// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "System/Entity/RecallEntityAsyncSpawnTypes.h"
#include "GameplayTagContainer.h"

#include "RecallDropItemTypes.generated.h"

class URecallInventoryItemAsset;

USTRUCT()
struct RECALLINVENTORYMODULE_API FRecallDropItemEntitySpawnCommand : public FRecallEntityAsyncSpawnCommand
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<const URecallInventoryItemAsset> ItemAsset;
	
	UPROPERTY(VisibleAnywhere)
	float Force = 0.0f;
	
	// FRecallEntityAsyncSpawnCommand implementation Begin
public:	
	virtual void OnSpawn(FMassExtendedEntityManager& System, const TArray<FMassExtendedEntityHandle>& Entities) const override;
	// FRecallEntityAsyncSpawnCommand implementation End
};

/**
 * Definition of a unique item drop.
 */
USTRUCT()
struct RECALLINVENTORYMODULE_API FRecallDropUniqueItemDefinition
{
	GENERATED_BODY()

	/**
	 * Tag of the item to drop.
	 */
	UPROPERTY(EditAnywhere, meta=(GameplayTagFilter="Inventory"))
	FGameplayTag ItemTag;

	/**
	 * Weight of this item drop ratio.
	 */
	UPROPERTY(EditAnywhere, meta=(ClampMin="0.0"))
	float Weight = 1.0f;

	/**
	 * Range of how many items can be dropped at once when it drops.
	 */
	UPROPERTY(EditAnywhere)
	FInt32Range CountRange = FInt32Range(1, 1);
};

USTRUCT()
struct RECALLINVENTORYMODULE_API FRecallDropItemDefinition
{
	GENERATED_BODY()

	/**
	 * List of items that can be dropped with their drop rate.
	 */
	UPROPERTY(EditAnywhere, meta=(TitleProperty="{ItemTag}"))
	TArray<FRecallDropUniqueItemDefinition> Items;

	/**
	 * Range of how many times an item should be dropped.
	 */
	UPROPERTY(EditAnywhere)
	FInt32Range CountRange = FInt32Range(1, 1);
	
	/**
	 * Force applied to the dropped items, multiplied by a random vector.
	 */
	UPROPERTY(EditAnywhere, meta=(Units=Newtons))
	float SpawnForce = 100.0f;
};
