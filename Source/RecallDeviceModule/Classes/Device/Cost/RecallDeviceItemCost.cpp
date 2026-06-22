// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallDeviceItemCost.h"

#include "Device/RecallDeviceCostContextTypes.h"
#include "MassEntityView.h"
#include "Simulation/GameplayTag/RecallGameplayTagFragments.h"
#include "System/Inventory/RecallInventorySubsystem.h"
#include "Utility/GameplayTag/RecallGameplayTagUtils.h"

//----------------------------------------------------------------------//
// FRecallDeviceItemCost
//----------------------------------------------------------------------//
bool FRecallDeviceItemCost::Evaluate(const FRecallDeviceCostContext& Context) const
{
	return EvaluateInventoryCost(Context) != bInvert;
}

void FRecallDeviceItemCost::Consume(const FRecallDeviceCostContext& Context) const
{
	for (FRecallGameplayTagCountMap* GameplayTagCountMap : GetGameplayTagCountMap(Context))
	{
		GameplayTagCountMap->RemoveTag(ItemTag, ItemCount);
	}
}

bool FRecallDeviceItemCost::EvaluateInventoryCost(const FRecallDeviceCostContext& Context) const
{
	for (const FRecallGameplayTagCountMap* GameplayTagCountMap : GetGameplayTagCountMap(Context))
	{
		check(GameplayTagCountMap != nullptr);
		if (GameplayTagCountMap->GetTagCount(ItemTag) >= ItemCount)
		{
			return true;
		}
	}

	return false;
}

TArray<FRecallGameplayTagCountMap*> FRecallDeviceItemCost::GetGameplayTagCountMap(
	const FRecallDeviceCostContext& Context) const
{
	TArray<FRecallGameplayTagCountMap*> Results;
	
	const FMassEntityView EntityView = Context.GetEntityView();
	auto* GameplayTagFragmentPtr = EntityView.GetFragmentDataPtr<FRecallGameplayTagFragment>();

	if (GameplayTagFragmentPtr != nullptr)
	{
		if (bUseFactionInventory)
		{
			URecallInventorySubsystem* InventorySystem = UWorld::GetSubsystem<URecallInventorySubsystem>(
				Context.GetWorld());
			checkf(IsValid(InventorySystem),
				TEXT("%s Invalid inventory system"), ANSI_TO_TCHAR(__FUNCTION__));
		
			const FGameplayTagContainer FactionTags = Recall::GameplayTag::Utils::GetFactionTags(
				GameplayTagFragmentPtr->GameplayTagCountMap.GetTags());

			TArray<FGameplayTag> FactionTagsArray;
			FactionTags.GetGameplayTagArray(FactionTagsArray);

			for (const FGameplayTag& FactionTag : FactionTagsArray)
			{
				Results.Add(&InventorySystem->GetMutableInventory(FactionTag));
			}
		}
		else
		{
			Results.Add(&GameplayTagFragmentPtr->GameplayTagCountMap);
		}
	}	

	return Results;
}
