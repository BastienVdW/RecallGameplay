// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallInteractEntityConditionTypes.h"

#include "MassExtendedEntityView.h"
#include "Simulation/Controller/RecallControllerFragments.h"
#include "Simulation/Transform/RecallTransformFragments.h"
#include "Utility/Entity/RecallEntityUtils.h"
#include "Utility/Interact/RecallInteractPositionUtils.h"

//----------------------------------------------------------------------//
// FRecallInteractHasChildrenCondition
//----------------------------------------------------------------------//
bool FRecallInteractHasChildrenCondition::EvaluateCondition(const FRecallInteractContext& Context, FText& OutFailedText) const
{
	const FMassExtendedEntityHandle TargetEntity = Context.GetTargetEntity(Target);
	const FMassExtendedEntityView TargetView(Context.GetEntityManagerChecked(), TargetEntity);
	const FRecallTransformFragment& TargetTransformFragment = TargetView.GetFragmentData<FRecallTransformFragment>();
	
	if (TargetTransformFragment.HasChildren() != bInvert)
	{
		return true;
	}
	else
	{
		OutFailedText = FailedText;
		return false;
	}
}

//----------------------------------------------------------------------//
// FRecallControlTargetCondition
//----------------------------------------------------------------------//
bool FRecallControlTargetCondition::EvaluateCondition(const FRecallInteractContext& Context, FText& OutFailedText) const
{
	FMassExtendedEntityManager& EntityManager = Context.GetEntityManagerChecked();
	
	// Get instigator position and forward vector
	const FMassExtendedEntityView InstigatorView(EntityManager, Context.InstigatorEntity);
	const FVector InstigatorPosition = Recall::Entity::Utils::GetEntityTransform(EntityManager, Context.InstigatorEntity).GetLocation();
	
	FVector ForwardVector;
	if (bUseControlRotation)
	{
		// Use controller rotation forward vector
		const FRecallControllerFragment* ControllerFragmentPtr = InstigatorView.GetFragmentDataPtr<FRecallControllerFragment>();
		if (!ControllerFragmentPtr)
		{
			OutFailedText = FText::FromString(TEXT("Instigator entity missing controller fragment"));
			return false;
		}
		ForwardVector = ControllerFragmentPtr->ControlRotation.Vector();
	}
	else
	{
		// Use instigator entity forward vector
		const FTransform InstigatorTransform = Recall::Entity::Utils::GetEntityTransform(EntityManager, Context.InstigatorEntity);
		ForwardVector = InstigatorTransform.GetRotation().GetForwardVector();
	}
	
	FVector InteractablePosition = Recall::Entity::Utils::GetEntityTransform(EntityManager, Context.InteractableEntity).GetLocation();
	if (bUseBoundingBoxProjection)
	{
		// Get projected interactable position using bounding box projection if enabled
		const FVector VectorToTarget = InteractablePosition - InstigatorPosition;
		const FVector TargetAlongForwardVector = InstigatorPosition + FVector::DotProduct(VectorToTarget, ForwardVector) * ForwardVector;
		InteractablePosition = Recall::Interact::Position::Utils::GetProjectedLocationFromPosition(
			EntityManager, TargetAlongForwardVector, Context.InteractableEntity);
	}	

	// Calculate direction to target
	const FVector DirectionToTarget = (InteractablePosition - InstigatorPosition).GetSafeNormal();
	
	// Calculate angle between forward vector and direction to target
	const float DotProduct = FVector::DotProduct(ForwardVector, DirectionToTarget);
	const float AngleRadians = FMath::Acos(FMath::Clamp(DotProduct, -1.0f, 1.0f));
	const float AngleDegrees = FMath::RadiansToDegrees(AngleRadians);
	
	if (AngleDegrees > MaxTargetingAngle)
	{
		const FString SourceType = bUseControlRotation ? TEXT("controller") : TEXT("entity");
		OutFailedText = FText::Format(
			FText::FromString(TEXT("Target outside {0} targeting angle: {1}° > {2}°")),
			FText::FromString(SourceType),
			FText::AsNumber(AngleDegrees),
			FText::AsNumber(MaxTargetingAngle)
		);
		return false;
	}
	
	return true;
}
