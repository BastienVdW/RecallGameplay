// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "MassEntityTraitBase.h"
#include "GameplayTagContainer.h"
#include "Collect/RecallCollectTypes.h"

#include "RecallCollectTraits.generated.h"

/**
* Entity that can be collected as an inventory item
*/
UCLASS(meta=(DisplayName="RE Collectable"))
class RECALLINVENTORYMODULE_API URecallCollectableTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

public:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;

protected:
	/**
	 * Tag of the item to collect.
	 */
	UPROPERTY(EditAnywhere, meta=(GameplayTagFilter="Inventory"), DisplayName="Item")
	FGameplayTag InventoryItem;

	/**
	 * How man items to collect.
	 */
	UPROPERTY(EditAnywhere, meta=(ClampMin=1, ClampMax=9999))
	int32 ItemCount = 1;
	
	/**
	 * Settings for the collectable entity.
	 */
	UPROPERTY(EditAnywhere, meta=(ShowOnlyInnerProperties))
	FRecallCollectableSettings CollectableSettings;
	
private:
	UFUNCTION()
	TArray<FName> GetSensorNames() const;
};

/**
* Entity where collectable can be collected
*/
UCLASS(meta=(DisplayName="RE Collect Point"))
class RECALLINVENTORYMODULE_API URecallCollectPointTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

public:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;

protected:
	UPROPERTY(EditAnywhere, DisplayName="Inventory Owner Tag")
	FGameplayTag OwnerTag;
};
