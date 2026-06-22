// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallInteractInventoryConditionTypes.h"

#include "MassEntityView.h"
#include "Data/Inventory/RecallInventoryItemAsset.h"
#include "Simulation/GameplayTag/RecallGameplayTagFragments.h"
#include "System/Inventory/RecallItemSubsystem.h"

FRecallInteractInventoryCondition::FRecallInteractInventoryCondition()
	: Super()
{
	FailedText = FText::FromString(TEXT("<Icon.{0}></>"));
}

bool FRecallInteractInventoryCondition::EvaluateCondition(const FRecallInteractContext& Context, FText& OutFailedText) const
{
	const FMassEntityView InstigatorView(Context.GetEntityManagerChecked(), Context.InstigatorEntity);
	const FRecallGameplayTagFragment* InstigatorGameplayTagFragmentPtr = InstigatorView.GetFragmentDataPtr<FRecallGameplayTagFragment>();

	if (InstigatorGameplayTagFragmentPtr == nullptr ||
		InstigatorGameplayTagFragmentPtr->GameplayTagCountMap.GetTagCount(ItemTag) < ItemCount)
	{
		const URecallItemSubsystem* ItemSystem = UGameInstance::GetSubsystem<URecallItemSubsystem>(Context.GetGameInstance());
		const TObjectPtr<const URecallInventoryItemAsset> ItemAsset = IsValid(ItemSystem) ?
			ItemSystem->GetItemAsset(ItemTag) : nullptr;
		if (ItemAsset)
		{
			OutFailedText = FText::Format(FailedText, FText::FromString(ItemAsset->IconName));
		}
		return false;
	}
	
	return true;
}
