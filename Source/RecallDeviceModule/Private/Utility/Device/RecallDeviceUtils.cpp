// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "Utility/Device/RecallDeviceUtils.h"

#include "Components/DecalComponent.h"
#include "MassEntityConfigAsset.h"
#include "MassEntityView.h"
#include "MassExecutionContext.h"
#include "Components/MeshComponent.h"
#include "Data/Device/RecallDeviceAsset.h"
#include "Device/RecallDeviceCostTypes.h"
#include "Device/RecallDeviceCostContextTypes.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Physics/RecallPhysicsObjects.h"
#include "Physics/JPRPhysicsBody.h"
#include "Simulation/Device/RecallDeviceFragments.h"
#include "Simulation/Device/RecallDeviceTypes.h"
#include "Simulation/GameplayTag/RecallGameplayTagFragments.h"
#include "Simulation/Physics/RecallPhysicsBodyFragment.h"
#include "Simulation/Physics/RecallPhysicsColliderTraits.h"
#include "Simulation/Representation/RecallActorRepresentationFragments.h"
#include "Simulation/Transform/RecallTransformFragments.h"
#include "System/Actor/RecallActorSubsystem.h"
#include "System/Entity/RecallEntitySubsystem.h"
#include "System/Physics/RecallPhysicsSubsystem.h"
#include "System/Representation/RecallRepresentationEventSubsystem.h"

namespace Recall::Device::Utils
{
	
FVector GetDevicePosition(const UMassEntityConfigAsset* EntityConfigAsset,
	const FRecallTransformFragment& TransformFragment, const FRecallPhysicsBodyFragment* BodyFragmentPtr,
	bool bSnapToGrid, float GridSize)
{
	FVector GroundPosition = TransformFragment.Position;

	if (BodyFragmentPtr != nullptr)
	{
		GroundPosition.Z -= BodyFragmentPtr->Extents.Z;
	}

	if (bSnapToGrid)
	{
		GroundPosition.X = FMath::RoundToFloat(GroundPosition.X / GridSize) * GridSize;
		GroundPosition.Y = FMath::RoundToFloat(GroundPosition.Y / GridSize) * GridSize;
	}

	if (!ensure(IsValid(EntityConfigAsset)))
	{
		return GroundPosition;
	}

	FVector PlacementPosition = GroundPosition;
	
	if (const URecallPhysicsBodyTrait* PhysicsBodyTrait = Cast<URecallPhysicsBodyTrait>(
		EntityConfigAsset->FindTrait(URecallPhysicsBodyTrait::StaticClass())))
	{
		PlacementPosition.Z += PhysicsBodyTrait->GetExtents().Z + 5.0f;	
	}

	return PlacementPosition;
}

void RequestChangeDeviceColor(const FMassEntityManager& EntityManager, const FMassEntityHandle& DeviceEntity,
	const FName& ParameterName, const FColor& Color, URecallRepresentationEventSubsystem& RepresentationEventSystem)
{
	RepresentationEventSystem.PushEvent([&EntityManager, DeviceEntity, ParameterName, Color]()
	{
		const URecallActorSubsystem* ActorSystem = UWorld::GetSubsystem<URecallActorSubsystem>(EntityManager.GetWorld());
		if (!EntityManager.IsEntityValid(DeviceEntity) || !IsValid(ActorSystem))
		{
			return;
		}

		const FMassEntityView DeviceView(EntityManager, DeviceEntity);
		const auto* DeviceActorFragmentPtr = DeviceView.GetFragmentDataPtr<FRecallActorRepresentationFragment>();
		const TWeakObjectPtr<AActor> DeviceActor = DeviceActorFragmentPtr != nullptr ?
			ActorSystem->GetActor(DeviceActorFragmentPtr->ActorHandle) : nullptr;
		if (!DeviceActor.IsValid())
		{
			return;
		}

		DeviceActor->ForEachComponent<UMeshComponent>(false, [ParameterName, Color](UMeshComponent* MeshComponent)
		{
			for (int32 MaterialIndex = 0; MaterialIndex < MeshComponent->GetNumMaterials(); MaterialIndex++)
			{
				UMaterialInstanceDynamic* DynamicMaterial = MeshComponent->CreateAndSetMaterialInstanceDynamic(MaterialIndex);
				if (IsValid(DynamicMaterial))
				{
					DynamicMaterial->SetVectorParameterValue(ParameterName, Color);
				}
			}
		});
		
		DeviceActor->ForEachComponent<UDecalComponent>(false, [ParameterName, Color](UDecalComponent* DecalComponent)
		{
			UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(DecalComponent->GetDecalMaterial());
			if (!IsValid(DynamicMaterial))
			{
				DynamicMaterial = DecalComponent->CreateDynamicMaterialInstance();
			}
			if (IsValid(DynamicMaterial))
			{
				DynamicMaterial->SetVectorParameterValue(ParameterName, Color);
			}
		});
	});
}

bool CheckDevicePosition(const FMassEntityManager& EntityManager, const URecallPhysicsSubsystem& PhysicsSystem,
	const FMassEntityHandle& DeviceEntity, const FVector& Position)
{
	if (!EntityManager.IsEntityValid(DeviceEntity))
	{
		return false;
	}
	
	const FMassEntityView DeviceView(EntityManager, DeviceEntity);
	const FRecallPhysicsBodyFragment* DeviceBodyFragmentPtr = DeviceView.GetFragmentDataPtr<FRecallPhysicsBodyFragment>();
	const FConstRecallPhysicsBodyView Body = DeviceBodyFragmentPtr != nullptr ?
		PhysicsSystem.GetBody(DeviceBodyFragmentPtr->BodyHandle) : nullptr;
	if (!Body.IsValid())
	{
		return true;
	}

	uint32 ContactBodyID = INVALID_PHYSICS_BODY_SERIAL_NUMBER;
	FVector ContactPosition = FVector::ZeroVector;
	FVector ContactNormal = FVector::ZeroVector;
	return !Body.Pin()->CollideShape(Position, ContactBodyID, ContactPosition, ContactNormal);
}

void SpawnDevicePlaceEntity(const FMassExecutionContext& Context, const FMassEntityHandle& OwnerEntity,
	const UMassEntityConfigAsset* DeviceEntityConfig, const FVector& DevicePosition, const FName& ColorParameterName, const FColor& Color,
	URecallEntitySubsystem& EntitySystem, URecallRepresentationEventSubsystem& RepresentationEventSystem)
{
	Context.Defer().PushCommand<FMassDeferredCreateCommand>(
		[&EntitySystem, &RepresentationEventSystem, OwnerEntity, ColorParameterName, Color,
			DeviceEntityConfig, DevicePosition]
				(FMassEntityManager& System)
	{
		if (!IsValid(DeviceEntityConfig) || !System.IsEntityValid(OwnerEntity))
		{
			return;
		}

		// Make sure that the owner is still placing the device when it spawns.
		const FMassEntityView OwnerView(System, OwnerEntity);
		const FRecallGameplayTagFragment& OwnerGameplayTagFragment = OwnerView.GetFragmentData<FRecallGameplayTagFragment>();
		if (!OwnerGameplayTagFragment.GameplayTagCountMap.HasTag(State_DevicePlace))
		{
			return;
		}
			
		TArray<FMassEntityHandle> Entities;
		EntitySystem.CreateEntities(DeviceEntityConfig, 1, Entities);

		const FMassEntityHandle& DeviceEntity = Entities[0];
		const FMassEntityView DeviceView(System, DeviceEntity);
		FRecallTransformFragment& DeviceTransformFragment = DeviceView.GetFragmentData<FRecallTransformFragment>();
		DeviceTransformFragment.Position = DevicePosition;

		// Ignore all physics collision for the device preview
		if (FRecallPhysicsBodyFragment* DeviceBodyFragmentPtr = DeviceView.GetFragmentDataPtr<FRecallPhysicsBodyFragment>())
		{
			DeviceBodyFragmentPtr->StartParameters = 0;
		}

		FRecallDevicePlacerFragment& DevicePlacerFragment = OwnerView.GetFragmentData<FRecallDevicePlacerFragment>();
		DevicePlacerFragment.PlaceDeviceEntity = DeviceEntity;
			
		Recall::Device::Utils::RequestChangeDeviceColor(System, DeviceEntity, ColorParameterName, Color, RepresentationEventSystem);
	});
}

bool EvaluateDeviceCost(const UWorld* World, const FMassEntityHandle& OwnerEntity,
	const TObjectPtr<const URecallDeviceAsset>& DeviceAsset)
{
	if (!ensure(IsValid(World) && DeviceAsset))
	{
		return false;
	}
	
	const FMassEntityManager& EntityManager = UE::Mass::Utils::GetEntityManagerChecked(*World);
	
	const FRecallDeviceCostContext DeviceCostContext(EntityManager, OwnerEntity);

	if (const FRecallDeviceCostBase* DeviceCostPtr = DeviceAsset->Cost.GetPtr<FRecallDeviceCostBase>())
	{
		if (!DeviceCostPtr->Evaluate(DeviceCostContext))
		{
			return false;
		}
	}
	
	return true;
}

void ConsumeDeviceCost(const UWorld* World, const FMassEntityHandle& OwnerEntity,
	const TObjectPtr<const URecallDeviceAsset>& DeviceAsset)
{
	if (!ensure(IsValid(World) && DeviceAsset))
	{
		return;
	}
	
	const FMassEntityManager& EntityManager = UE::Mass::Utils::GetEntityManagerChecked(*World);
	
	const FRecallDeviceCostContext DeviceCostContext(EntityManager, OwnerEntity);

	if (const FRecallDeviceCostBase* DeviceCostPtr = DeviceAsset->Cost.GetPtr<FRecallDeviceCostBase>())
	{
		DeviceCostPtr->Consume(DeviceCostContext);
	}
}
	
} // namespace Recall::Device::Utils
