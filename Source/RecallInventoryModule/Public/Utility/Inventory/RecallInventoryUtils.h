// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"

struct FGameplayTag;
struct FMassExtendedEntityManager;
struct FRecallDropItemDefinition;
class URecallInventoryItemAsset;

struct FRecallEquipmentContext
{
	const class URecallItemSubsystem& ItemSystem;
	class URecallActorSubsystem& ActorSystem;
	class URecallSignalSubsystem& SignalSystem;
	
	const struct FMassExtendedEntityHandle& Entity;
	const struct FRecallEquipmentConstSharedFragment& EquipmentConstSharedFragment;
	struct FRecallEquipmentFragment& EquipmentFragment;
	struct FRecallGameplayTagFragment* const GameplayTagFragmentPtr = nullptr;
};

namespace Recall::Inventory::Utils
{

RECALLINVENTORYMODULE_API extern void AutoEquipItem(const FRecallEquipmentContext& Context);
RECALLINVENTORYMODULE_API extern void UpdateEquipmentRepresentation(const FRecallEquipmentContext& Context);
RECALLINVENTORYMODULE_API extern void UpdateEquipmentEffects(const FRecallEquipmentContext& Context);

RECALLINVENTORYMODULE_API extern void CreateDropItem(FMassExtendedEntityManager& System,
	const FRecallDropItemDefinition& Definition, const FVector& Position);
	
/// Generate a drop item entity based on item data.
/// @param System Entity manager to create this entity from.
/// @param ItemTag The item to drop.
/// @param Location Location where to generate the drop item.
/// @param ColliderLayer Layer of the entity collider
/// @param Force Force applied to the item on spawn
/// @param ColliderSize Size of the entity collider
/// @param bCollectable Make this item as a collectable
RECALLINVENTORYMODULE_API extern void CreateDropItem(FMassExtendedEntityManager& System, const FGameplayTag& ItemTag, int32 ItemCount,
	const FVector& Location, float Force = 0.0f);
	
} // namespace Recall::Inventory::Utils
