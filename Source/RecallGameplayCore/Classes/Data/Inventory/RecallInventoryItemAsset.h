// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "StructUtils/InstancedStruct.h"

#include "RecallInventoryItemAsset.generated.h"

UCLASS()
class RECALLGAMEPLAYCORE_API URecallInventoryItemAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	static const FPrimaryAssetType AssetType;
	
public:
	/**
	 * Tag to identify this item.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Item, meta=(GameplayTagFilter="Inventory"))
	FGameplayTag ItemTag;

	/**
	 * Commands that can be executed by this item.
	 */
	UPROPERTY(EditAnywhere, Category=Item, meta=(BaseStruct="/Script/RecallGameplayCore.RecallInventoryItemCommand", ExcludeBaseStruct))
	FInstancedStruct Command;

	/**
	 * Base entity config asset to spawn the item from.
	 * Representation will be generated using MeshDesc, and collectable will be set with this item's tag.
	 */
	UPROPERTY(EditAnywhere, Category=Item)
	TSoftObjectPtr<class UMassEntityConfigAsset> DropBaseEntityConfig;

	/**
	 * Ability collection that can be used by this item.
	 */
	UPROPERTY(EditAnywhere, Category=Item, meta=(AllowedClasses="/Script/RecallAbility.RecallAbilityCollectionAsset"))
	TObjectPtr<UObject> AbilityCollection;
	
	/**
	 * Base price of the item when sold.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Item)
	int32 Price = 10;
	
	/**
	 * List of equip slots to which this item can be equipped.
	 */
	UPROPERTY(EditAnywhere, Category=Equip, meta=(GameplayTagFilter="EquipSlot"))
	FGameplayTagContainer EquipSlotTags;

	/**
	 * Tags applied to the owner when item is equipped.
	 */
	UPROPERTY(EditAnywhere, Category=Equip)
	FGameplayTagContainer EquipTags;

	/**
	 * Device that can be placed while using this item.
	 */
	UPROPERTY(EditAnywhere, Category=Device, meta=(GameplayTagFilter="Device"))
	FGameplayTag DeviceTag;
	
	/**
	 * Name of the icon for this item.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=UI)
	FString IconName;
	
	/**
	 * Representation of the item.
	 */
	UPROPERTY(EditAnywhere, Category=Representation, meta=(BaseStruct="/Script/RecallSimulation.RecallRepresentationDescBase", ExcludeBaseStruct))
	FInstancedStruct MeshDesc;
	
	/**
	 * Scale of the actor representation.
	 */
	UPROPERTY(EditAnywhere, Category=Representation)
	FVector MeshScale = FVector::OneVector;
	
	/**
	 * Display the item when it is being equipped.
	 */
	UPROPERTY(EditAnywhere, Category=Representation)
	bool bShowWhenEquipped = true;

	/**
	 * Hide this item when it is not being selected.
	 */
	UPROPERTY(EditAnywhere, Category=Representation, meta=(EditCondition=bShowWhenEquipped, EditConditionHides))
	bool bOnlyShowWhenSelected = false;

	/**
	 * Parent socket to attach the representation of the item.
	 */
	UPROPERTY(EditAnywhere, Category=Representation, meta=(EditCondition=bShowWhenEquipped, EditConditionHides))
	FName SocketName = NAME_None;

	/**
	 * Allow to override the representation attach socket for each equip slot.
	 * If not set, then it will default to SocketName.
	 */
	UPROPERTY(EditAnywhere, Category=Representation, meta=(EditCondition=bShowWhenEquipped, EditConditionHides, GameplayTagFilter="EquipSlot"))
	TMap<FGameplayTag, FName> OverrideEquipSlotSocket;
	
	/**
	 * Relative transform to the parent of this item representation.
	 */
	UPROPERTY(EditAnywhere, Category=Representation, meta=(EditCondition=bShowWhenEquipped, EditConditionHides))
	FTransform RelativeTransform = FTransform::Identity;
	
protected:	
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
	
};
