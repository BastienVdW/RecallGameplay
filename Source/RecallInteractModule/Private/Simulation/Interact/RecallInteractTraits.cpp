// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "Simulation/Interact/RecallInteractTraits.h"

#include "Data/Interact/RecallInteractAsset.h"
#include "MassExtendedEntityConfigAsset.h"
#include "MassExtendedEntityTemplateRegistry.h"
#include "MassExtendedEntityView.h"
#include "Simulation/Interact/RecallInteractFragments.h"
#include "Simulation/Physics/RecallPhysicsBodyFragment.h"
#include "Simulation/Physics/RecallPhysicsSensorFragment.h"
#include "Simulation/Sensor/RecallSensorTraits.h"
#include "Simulation/Transform/RecallTransformFragments.h"
#include "Utility/Trait/RecallTraitUtils.h"

//----------------------------------------------------------------------//
// URecallInteractorTrait
//----------------------------------------------------------------------//
void URecallInteractorTrait::BuildTemplate(FMassExtendedEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	FMassExtendedEntityManager& EntityManager = UE::MassExtended::Utils::GetEntityManagerChecked(World);

	BuildContext.RequireFragment<FRecallTransformFragment>();
	BuildContext.RequireFragment<FRecallPhysicsSensorFragment>();

	BuildContext.AddFragment<FRecallInteractorFragment>();

	BuildContext.AddTag<FRecallInteractorTag>();

	FRecallInteractorSharedFragment SharedFragment;
	SharedFragment.SensorName = SensorName;

	BuildContext.AddConstSharedFragment(EntityManager.GetOrCreateConstSharedFragment(SharedFragment));
}

TArray<FName> URecallInteractorTrait::GetSensorNames() const
{
	TArray<FName> Results;

	if (const FMassExtendedEntityConfig* EntityConfig = Recall::Trait::Utils::GetEntityConfig(GetOuter()))
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

	return Results;
}

//----------------------------------------------------------------------//
// URecallInteractableTrait
//----------------------------------------------------------------------//
URecallInteractableTrait::URecallInteractableTrait()
	: Super()
{
	InteractionPositionTag = TEXT("Interact");
}

void URecallInteractableTrait::BuildTemplate(FMassExtendedEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	FMassExtendedEntityManager& EntityManager = UE::MassExtended::Utils::GetEntityManagerChecked(World);

	BuildContext.RequireFragment<FRecallTransformFragment>();
	BuildContext.RequireFragment<FRecallPhysicsBodyFragment>();

	BuildContext.AddFragment<FRecallInteractableFragment>();

	// Register Object Fragment Initializer to detect interaction positions from actor components
	BuildContext.GetMutableObjectFragmentInitializers().Add(
		[InteractionPositionTag = this->InteractionPositionTag](
			UObject& Owner,
			FMassExtendedEntityView& EntityView,
			const EMassExtendedTranslationDirection CurrentDirection)
		{
			// Skip if no tag configured (backward compatibility)
			if (InteractionPositionTag == NAME_None)
				return;

			// Access static actor via utility function
			const AActor* Actor = Recall::Trait::Utils::AsActor(Owner);
			if (!Actor)
				return;

			// Find all components with the specified tag
			TArray<FRecallInteractionPosition> InteractionPositions;
			TArray<UActorComponent*> Components = Actor->GetComponentsByTag(
				USceneComponent::StaticClass(),
				InteractionPositionTag
			);

			for (UActorComponent* Component : Components)
			{
				if (const USceneComponent* SceneComp = Cast<USceneComponent>(Component))
				{
					const FVector RelativeLocation = Actor->GetActorTransform().InverseTransformPosition(SceneComp->GetComponentLocation());

					// Validate position (not NaN/Inf)
					if (!RelativeLocation.ContainsNaN())
					{
						FRecallInteractionPosition Position;
						// Convert world location to relative location
						Position.RelativeLocation = RelativeLocation;
						InteractionPositions.Add(Position);
					}
				}
			}

			// Populate the InteractionPositions array directly in the interactable fragment
			FRecallInteractableFragment& InteractableFragment =
				EntityView.GetFragmentData<FRecallInteractableFragment>();
			InteractableFragment.InteractionPositions = MoveTemp(InteractionPositions);
		}
	);

	FRecallInteractableConstSharedFragment SharedFragment;

	if (InteractAsset)
	{
		SharedFragment.SetInteractAsset(InteractAsset);
	}
	else
	{
		SharedFragment.SetDefaultInteraction(Interaction);
	}

	BuildContext.AddConstSharedFragment(EntityManager.GetOrCreateConstSharedFragment(SharedFragment));
}
