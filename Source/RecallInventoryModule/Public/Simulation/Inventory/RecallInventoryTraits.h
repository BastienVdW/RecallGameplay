// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "MassEntityTraitBase.h"
#include "RecallDropItemTypes.h"
#include "RecallEquipmentTypes.h"

#include "RecallInventoryTraits.generated.h"

/**
* Trait for entities that can equip items
*/
UCLASS(meta=(DisplayName="RE Equipment"))
class RECALLINVENTORYMODULE_API URecallEquipmentTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

public:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;

protected:
	/**
	 * The equip slot being selected by default.
	 */
	UPROPERTY(EditAnywhere, meta=(GameplayTagFilter="EquipSlot"))
	FGameplayTag DefaultSelectedEquipSlot;

	/**
	 * List of equipped items at the start of the game.
	 */
	UPROPERTY(EditAnywhere, meta=(GameplayTagFilter="EquipSlot"))
	TMap<FGameplayTag, FRecallEquipmentSlot> StartingEquipment;

	/**
	 * Settings for the equipment that this entity can equip.
	 */
	UPROPERTY(EditAnywhere)
	FRecallEquipmentSettings Settings;
};

/**
* Trait for entities that can drop items.
*/
UCLASS(meta=(DisplayName="RE Drop Item"))
class RECALLINVENTORYMODULE_API URecallDropItemTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

public:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;

protected:
	UPROPERTY(EditAnywhere, meta=(ShowOnlyInnerProperties))
	FRecallDropItemDefinition DropItem;
};
