// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "Simulation/Collect/RecallCollectTraits.h"

#include "MassEntityConfigAsset.h"
#include "MassEntityTemplateRegistry.h"
#include "Simulation/Collect/RecallCollectFragments.h"
#include "Simulation/Physics/RecallPhysicsBodyFragment.h"
#include "Simulation/Sensor/RecallSensorTraits.h"
#include "Simulation/Transform/RecallTransformFragments.h"
#include "Utility/Trait/RecallTraitUtils.h"

//----------------------------------------------------------------------//
// URecallCollectableTrait
//----------------------------------------------------------------------//
void URecallCollectableTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	FMassEntityManager& EntityManager = UE::Mass::Utils::GetEntityManagerChecked(World);

	BuildContext.RequireFragment<FRecallTransformFragment>();
	
	if (CollectableSettings.bAutoCollect)
	{
		BuildContext.RequireFragment<FRecallPhysicsSensorFragment>();
	}
	
	FRecallCollectableFragment& CollectableFragment = BuildContext.AddFragment_GetRef<FRecallCollectableFragment>();
	CollectableFragment.InventoryItem = InventoryItem;
	CollectableFragment.ItemCount = ItemCount;
	
	FRecallCollectableConstSharedFragment ConstSharedFragment;
	ConstSharedFragment.CollectableSettings = CollectableSettings;

	BuildContext.AddConstSharedFragment(EntityManager.GetOrCreateConstSharedFragment(ConstSharedFragment));
}

TArray<FName> URecallCollectableTrait::GetSensorNames() const
{
	TArray<FName> Results;

#if WITH_EDITOR
	if (const FMassEntityConfig* EntityConfig = Recall::Trait::Utils::GetEntityConfig(GetOuter()))
	{
		if (const URecallSensorTrait* SensorTrait = Cast<URecallSensorTrait>(EntityConfig->FindTrait(URecallSensorTrait::StaticClass())))
		{
			Results.Append(SensorTrait->GetSensorNames());
		}
		else
		{
			Results.Add(NAME_None);
		}
	}
	else
	{
		Results.Add(NAME_None);
	}
#endif // WITH_EDITOR

	return Results;
}

//----------------------------------------------------------------------//
// URecallCollectPointTrait
//----------------------------------------------------------------------//
void URecallCollectPointTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext,
	const UWorld& World) const
{
	FRecallCollectPointFragment& CollectPointFragment = BuildContext.AddFragment_GetRef<FRecallCollectPointFragment>();
	CollectPointFragment.OwnerInventoryTag = OwnerTag;
}
