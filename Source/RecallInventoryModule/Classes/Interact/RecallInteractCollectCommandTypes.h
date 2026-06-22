// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Interact/RecallInteractCommandTypes.h"
#include "Simulation/Inventory/RecallDropItemTypes.h"

#include "RecallInteractCollectCommandTypes.generated.h"

/**
 * Collect this entity when the interaction is executed.
 * The entity must have the "RE Collectable" trait.
 */
USTRUCT(DisplayName="Collect")
struct RECALLINVENTORYMODULE_API FRecallCollectCommand : public FRecallInteractCommand
{
	GENERATED_BODY()

public:
	virtual void OnExecute(const FRecallInteractContext& Context) const override;
};

/**
 * Drop an item on interact.
 */
USTRUCT(DisplayName="Drop Item")
struct RECALLINVENTORYMODULE_API FRecallDropItemCommand : public FRecallInteractCommand
{
	GENERATED_BODY()

public:
	virtual void OnExecute(const FRecallInteractContext& Context) const override;

protected:
	/**
	 * Use "RE Drop item" to decide which item to drop.
	 */
	UPROPERTY(EditAnywhere)
	bool bUseDropItemTrait = true;
	
	/**
	 * Custom drop item settings.
	 */
	UPROPERTY(EditAnywhere, meta=(GameplayTagFilter="Inventory", EditCondition="!bUseDropItemTrait"))
	FRecallDropItemDefinition DropItem;

	/**
	 * Whether the interactable entity should be destroyed.
	 */
	UPROPERTY(EditAnywhere)
	bool bDestroyInteractableEntity = true;
	
	/**
	 * Offset of the spawned item relative to the interactable entity.
	 */
	UPROPERTY(EditAnywhere)
	FVector SpawnOffset = FVector(0.0f, 0.0f, 50.0f);
};
