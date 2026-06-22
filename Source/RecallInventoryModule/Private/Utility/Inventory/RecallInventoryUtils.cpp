// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "Utility/Inventory/RecallInventoryUtils.h"

#include "Data/Inventory/RecallInventoryItemAsset.h"
#include "GameplayTagContainer.h"
#include "MassEntityView.h"
#include "RecallSignalSubsystem.h"
#include "Interact/RecallInteractCollectCommandTypes.h"
#include "Simulation/GameplayTag/RecallGameplayTagFragments.h"
#include "Simulation/Inventory/RecallInventoryFragments.h"
#include "Simulation/Inventory/RecallInventorySignalTypes.h"
#include "System/Actor/RecallActorSubsystem.h"
#include "System/Entity/RecallEntityAsyncSpawnSubsystem.h"
#include "System/Inventory/RecallItemSubsystem.h"
#include "System/Random/RecallRandomNumberSubsystem.h"

namespace Recall::Inventory::Utils
{

void UpdateEquipmentRepresentation(const FRecallEquipmentContext& Context)
{
	TArray<FGameplayTag> EquipSlots;
	Context.EquipmentConstSharedFragment.Settings.EquipmentSlots.GetGameplayTagArray(EquipSlots);

	for (const FGameplayTag& EquipSlotTag : EquipSlots)
	{
		if (Context.EquipmentFragment.Equipment.IsEmptySlot(EquipSlotTag))
		{
			FRecallEquipSlotRepresentation EquipSlotRepresentation;
			if (Context.EquipmentFragment.EquipSlotRepresentationMap.RemoveAndCopyValue(EquipSlotTag, EquipSlotRepresentation))
			{
				Context.ActorSystem.ReleaseActor(EquipSlotRepresentation.ActorHandle);
			}
		}
		else if (!Context.EquipmentFragment.EquipSlotRepresentationMap.Contains(EquipSlotTag))
		{
			FGameplayTag ItemTag;
			if (Context.EquipmentFragment.Equipment.GetEquipmentSlot(EquipSlotTag, ItemTag))
			{
				const TObjectPtr<const URecallInventoryItemAsset> ItemAsset = Context.ItemSystem.GetItemAsset(ItemTag);
				if (ensureAlwaysMsgf(ItemAsset, TEXT("Failed to find item asset: %s"), *ItemTag.ToString()))
				{
					if (!ItemAsset->bShowWhenEquipped)
					{
						continue;
					}
				
					if (ItemAsset->MeshDesc.IsValid())
					{
						const FRecallEquipSlotRepresentation EquipSlotRepresentation{
							Context.ActorSystem.CreateActor(ItemAsset->MeshDesc)
						};
					
						Context.EquipmentFragment.EquipSlotRepresentationMap.Add(EquipSlotTag, EquipSlotRepresentation);
					}
				}
			}
		}
	}
}

void AutoEquipItem(const FRecallEquipmentContext& Context)
{
	if (Context.GameplayTagFragmentPtr == nullptr)
	{
		return;
	}
	
	TArray<FGameplayTag> ItemTags;
	Context.GameplayTagFragmentPtr->GameplayTagCountMap.GetTags().GetGameplayTagArray(ItemTags);

	for (const FGameplayTag& ItemTag : ItemTags)
	{
		// Attempt to find an existing item for this tag
		const TObjectPtr<const URecallInventoryItemAsset> ItemAsset = Context.ItemSystem.GetItemAsset(ItemTag);
		if (!ItemAsset)
		{
			continue;
		}

		TArray<FGameplayTag> EquipSlotTags;
		ItemAsset->EquipSlotTags.GetGameplayTagArray(EquipSlotTags);

		for (const FGameplayTag& EquipSlotTag : EquipSlotTags)
		{
			// Must have this equip slot
			if (!Context.EquipmentConstSharedFragment.Settings.EquipmentSlots.HasTag(EquipSlotTag))
			{
				continue;
			}

			// Slot must be empty
			if (!Context.EquipmentFragment.Equipment.IsEmptySlot(EquipSlotTag))
			{
				continue;
			}

			Context.EquipmentFragment.Equipment.SetEquipmentSlot(EquipSlotTag, ItemTag);
			Context.GameplayTagFragmentPtr->GameplayTagCountMap.RemoveTag(ItemTag);

			Context.SignalSystem.SignalEntity(Recall::Inventory::Signals::Callback::OnEquip, Context.Entity);
			break;
		}		
	}			
}
	
void UpdateEquipmentEffects(const FRecallEquipmentContext& Context)
{
	auto AddEffect = [&](const TObjectPtr<const URecallInventoryItemAsset>& ItemAsset)
	{
		if (Context.GameplayTagFragmentPtr != nullptr)
		{
			Context.GameplayTagFragmentPtr->GameplayTagCountMap.AddTags(ItemAsset->EquipTags);
		}
	};
	
	auto RemoveEffect = [&](const TObjectPtr<const URecallInventoryItemAsset>& ItemAsset)
	{
		if (Context.GameplayTagFragmentPtr != nullptr)
		{
			Context.GameplayTagFragmentPtr->GameplayTagCountMap.RemoveTags(ItemAsset->EquipTags);
		}
	};
	
	TArray<FGameplayTag> EquipSlots;
	Context.EquipmentConstSharedFragment.Settings.EquipmentSlots.GetGameplayTagArray(EquipSlots);

	TArray<TObjectPtr<const URecallInventoryItemAsset>> EquippedItems;
	
	for (const FGameplayTag& EquipSlotTag : EquipSlots)
	{
		FGameplayTag ItemTag;
		if (Context.EquipmentFragment.Equipment.GetEquipmentSlot(EquipSlotTag, ItemTag))
		{
			const TObjectPtr<const URecallInventoryItemAsset> ItemAsset = Context.ItemSystem.GetItemAsset(ItemTag);
			if (!ItemAsset)
			{
				continue;
			}
			
			EquippedItems.Add(ItemAsset);
		}
	}

	for (int32 ItemIndex = EquippedItems.Num() - 1; ItemIndex >= 0; --ItemIndex)
	{
		const TObjectPtr<const URecallInventoryItemAsset>& ItemAsset = EquippedItems[ItemIndex];
		if (Context.EquipmentFragment.EquippedItems.Contains(ItemAsset))
		{
			Context.EquipmentFragment.EquippedItems.RemoveSingleSwap(ItemAsset, EAllowShrinking::No); // Already applied
		}
		else
		{
			AddEffect(ItemAsset);
		}
	}

	for (const TObjectPtr<const URecallInventoryItemAsset>& ItemAsset : Context.EquipmentFragment.EquippedItems)
	{
		RemoveEffect(ItemAsset);
	}
	
	Context.EquipmentFragment.EquippedItems = EquippedItems;
}

void CreateDropItem(FMassEntityManager& System,
	const FRecallDropItemDefinition& Definition, const FVector& Position)
{
	if (Definition.Items.Num() == 0)
	{
		return;
	}
	
	URecallRandomNumberSubsystem* RandomNumberSystem = UWorld::GetSubsystem<URecallRandomNumberSubsystem>(System.GetWorld());
	if (!ensureAlwaysMsgf(IsValid(RandomNumberSystem), TEXT("%hs Invalid random number system"), __FUNCTION__))
	{
		return;
	}

	const FRandomStream& RandomStream = RandomNumberSystem->GetRandomStream();
	int32 SpawnCount = Definition.CountRange.Size<int32>() > 1 ?
		RandomStream.RandRange(Definition.CountRange.GetLowerBoundValue(), Definition.CountRange.GetUpperBoundValue()) : 1;

	TArray<float> Weights;
	Algo::Transform(Definition.Items, Weights, [](const FRecallDropUniqueItemDefinition& Item) { return Item.Weight; });

	while (SpawnCount-- > 0)
	{
		const int32 Index = RandomNumberSystem->RandWeigthedIndex(Weights);
		const FRecallDropUniqueItemDefinition& Item = Definition.Items[Index];

		const int32 ItemCount = Item.CountRange.Size<int32>() > 1 ?
			RandomStream.RandRange(Item.CountRange.GetLowerBoundValue(), Item.CountRange.GetUpperBoundValue()) : 1;
		
		CreateDropItem(System, Item.ItemTag, ItemCount, Position, Definition.SpawnForce);
	}
}

void CreateDropItem(FMassEntityManager& System, const FGameplayTag& ItemTag, int32 ItemCount,
	const FVector& Location, float Force /*= 0.0f*/)
{
	const URecallItemSubsystem& ItemSystem = URecallItemSubsystem::GetRef(System.GetWorld());	
	const TObjectPtr<const URecallInventoryItemAsset> ItemAsset = ItemSystem.GetItemAsset(ItemTag);
	if (!ItemAsset)
	{
		UE_LOG(LogRecallInteractCommand, Warning, TEXT("%hs Invalid item tag: %s"),
			__FUNCTION__, *ItemTag.ToString())
		return;
	}
	
	URecallEntityAsyncSpawnSubsystem* EntityAsyncSpawnSubsystem = UWorld::GetSubsystem<URecallEntityAsyncSpawnSubsystem>(System.GetWorld());
	if (!IsValid(EntityAsyncSpawnSubsystem))
	{
		UE_LOG(LogRecallInteractCommand, Warning,
			TEXT("%hs Invalid EntityAsyncSpawnSubsystem"), __FUNCTION__)
		return;
	}

	FRecallDropItemEntitySpawnCommand SpawnCommand;
	SpawnCommand.ItemAsset = ItemAsset;
	SpawnCommand.Force = Force;

	FRecallEntityAsyncSpawnParameters DropItemParams;
	DropItemParams.EntityCount = ItemCount;
	DropItemParams.SpawnCommand = FInstancedStruct::Make(SpawnCommand);
	
	EntityAsyncSpawnSubsystem->SpawnEntityAsync(ItemAsset->DropBaseEntityConfig, Location, FQuat::Identity, DropItemParams);
}

} // namespace Recall::Inventory::Utils
