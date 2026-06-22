// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "System/Inventory/RecallInventorySubsystem.h"

void URecallInventorySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void URecallInventorySubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void URecallInventorySubsystem::Reset()
{
	InventoryRegistry.Reset();
}

void URecallInventorySubsystem::Save(const FRecallSnapshotContext& Context, FInstancedStruct& OutSnapshot)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_Inventory_Save);

	OutSnapshot.InitializeAs<FRecallInventoryRegistry>(InventoryRegistry);
}

void URecallInventorySubsystem::Restore(const FRecallSnapshotContext& Context, const FInstancedStruct& InSnapshot)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_Inventory_Restore);

	const FRecallInventoryRegistry* DataPtr = InSnapshot.GetPtr<FRecallInventoryRegistry>();
	if (DataPtr != nullptr)
	{
		InventoryRegistry = *DataPtr;
	}
}

FRecallGameplayTagCountMap& URecallInventorySubsystem::GetMutableInventory(const FGameplayTag& OwnerTag)
{
	FRecallGameplayTagCountMap* InventoryPtr = InventoryRegistry.Inventories.Find(OwnerTag);
	if (InventoryPtr != nullptr)
	{
		return *InventoryPtr;
	}
	return InventoryRegistry.Inventories.Add(OwnerTag);
}

const FRecallGameplayTagCountMap& URecallInventorySubsystem::GetInventory(const FGameplayTag& OwnerTag) const
{
	const FRecallGameplayTagCountMap* InventoryPtr = InventoryRegistry.Inventories.Find(OwnerTag);
	if (InventoryPtr != nullptr)
	{
		return *InventoryPtr;
	}
	static const FRecallGameplayTagCountMap DummyInventory;
	return DummyInventory;
}
