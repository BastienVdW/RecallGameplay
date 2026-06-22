// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "Simulation/Inventory/RecallDropItemTypes.h"

#include "Data/Inventory/RecallInventoryItemAsset.h"
#include "MassExtendedEntityManager.h"
#include "MassExtendedEntityView.h"
#include "Simulation/Collect/RecallCollectFragments.h"
#include "Simulation/Physics/RecallPhysicsBodyFragment.h"
#include "Simulation/Representation/RecallActorRepresentationFragments.h"
#include "System/Actor/RecallActorSubsystem.h"
#include "System/Random/RecallRandomNumberSubsystem.h"
#include "Utility/Math/RecallMathUtils.h"

void FRecallDropItemEntitySpawnCommand::OnSpawn(FMassExtendedEntityManager& System,
                                                  const TArray<FMassExtendedEntityHandle>& Entities) const
{
	if (!ensure(ItemAsset))
	{
		return;
	}

	URecallActorSubsystem* ActorSystem = UWorld::GetSubsystem<URecallActorSubsystem>(System.GetWorld());
	if (!ensureAlwaysMsgf(IsValid(ActorSystem), TEXT("%hs Invalid actor system"), __FUNCTION__))
	{
		return;
	}

	URecallRandomNumberSubsystem* RandomNumberSystem = UWorld::GetSubsystem<URecallRandomNumberSubsystem>(System.GetWorld());
	if (!ensureAlwaysMsgf(IsValid(RandomNumberSystem), TEXT("%hs Invalid random number system"), __FUNCTION__))
	{
		return;
	}

	const float AccelerationMetersPerSecondSquared = Force * 100.0f;
	const float Acceleration = Recall::Math::Utils::UnitsPerSecondSquaredToPerFrameSquared(AccelerationMetersPerSecondSquared);
	const FRandomStream& RandomStream = RandomNumberSystem->GetRandomStream();
	
	const FRecallRepresentationDescBase* MeshDescPtr = ItemAsset->MeshDesc.GetPtr<FRecallRepresentationDescBase>();
	
	for (const FMassExtendedEntityHandle& Entity : Entities)
	{
		// We must add new fragments BEFORE we access the entity archetype through the entity view, because the archetype will change
		if (MeshDescPtr != nullptr)
		{
			System.AddFragmentToEntity(Entity, FRecallActorRepresentationFragment::StaticStruct());
		}
		
		const FMassExtendedEntityView EntityView(System, Entity);
		
		FRecallPhysicsBodyFragment* BodyFragmentPtr = EntityView.GetFragmentDataPtr<FRecallPhysicsBodyFragment>();
		
		if (FRecallActorRepresentationFragment* ActorFragmentPtr = EntityView.GetFragmentDataPtr<FRecallActorRepresentationFragment>())
		{
			if (MeshDescPtr != nullptr && ensure(!ActorFragmentPtr->ActorHandle.IsSet()))
			{
				const float HalfHeight = BodyFragmentPtr != nullptr ? BodyFragmentPtr->Extents.Z : 0.0f;
				
				ActorFragmentPtr->ActorHandle = ActorSystem->CreateActor(ItemAsset->MeshDesc);
				ActorFragmentPtr->Scale = ItemAsset->MeshScale;
				ActorFragmentPtr->Offset = FTransform(-FVector::UpVector * HalfHeight); // Center the actor
			}
		}

		if (BodyFragmentPtr != nullptr)
		{
			BodyFragmentPtr->StartVelocity = RandomStream.VRand() * Acceleration;
		}

		if (FRecallCollectableFragment* CollectableFragmentPtr = EntityView.GetFragmentDataPtr<FRecallCollectableFragment>())
		{
			CollectableFragmentPtr->InventoryItem = ItemAsset->ItemTag;
		}
	}
}
