// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "Simulation/Inventory/RecallInventoryTraits.h"

#include "MassEntityTemplateRegistry.h"
#include "Simulation/Inventory/RecallInventoryFragments.h"
#include "Simulation/Inventory/RecallDropItemFragments.h"

//----------------------------------------------------------------------//
// URecallEquipmentTrait
//----------------------------------------------------------------------//
void URecallEquipmentTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	FMassEntityManager& EntityManager = UE::Mass::Utils::GetEntityManagerChecked(World);
	
	FRecallEquipmentFragment& EquipmentFragment = BuildContext.AddFragment_GetRef<FRecallEquipmentFragment>();
	EquipmentFragment.SelectedEquipSlot = DefaultSelectedEquipSlot;
	
	for (const TPair<FGameplayTag, FRecallEquipmentSlot>& EquipmentSlot : StartingEquipment)
	{
		EquipmentFragment.Equipment.SetEquipmentSlot(EquipmentSlot.Key, EquipmentSlot.Value);
	}
	
	FRecallEquipmentConstSharedFragment SharedFragment;
	SharedFragment.Settings = Settings;

	BuildContext.AddConstSharedFragment(EntityManager.GetOrCreateConstSharedFragment(SharedFragment));
}
//----------------------------------------------------------------------//
// URecallDropItemTrait
//----------------------------------------------------------------------//
void URecallDropItemTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	FMassEntityManager& EntityManager = UE::Mass::Utils::GetEntityManagerChecked(World);
	
	BuildContext.AddFragment<FRecallDropItemFragment>();

	FRecallDropItemConstSharedFragment SharedFragment;
	SharedFragment.DropItem = DropItem;

	BuildContext.AddConstSharedFragment(EntityManager.GetOrCreateConstSharedFragment(SharedFragment));
}
