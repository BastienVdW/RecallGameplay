// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallInteractConditionPositionOccupied.h"

#include "MassExtendedEntityView.h"
#include "MassExtendedExecutionContext.h"
#include "Simulation/Interact/RecallInteractFragments.h"
#include "Utility/Interact/RecallInteractPositionUtils.h"

#define LOCTEXT_NAMESPACE "RecallInteractCondition"

bool FRecallInteractConditionPositionOccupied::EvaluateCondition(const FRecallInteractContext& Context, FText& OutFailedText) const
{
	FMassExtendedEntityManager& EntityManager = Context.ExecutionContext.GetEntityManagerChecked();

	if (!EntityManager.IsEntityValid(Context.InteractableEntity))
	{
		OutFailedText = LOCTEXT("InvalidInteractable", "Invalid interactable entity");
		return false;
	}

	const FMassExtendedEntityView InteractableView(EntityManager, Context.InteractableEntity);
	const FMassExtendedEntityView InteractorView(EntityManager, Context.InstigatorEntity);

	// Determine which position to check
	int32 TargetPosition = PositionIndexToCheck;
	if (TargetPosition == INDEX_NONE)
	{
		// Use interactor's current closest position
		const FRecallInteractorFragment& InteractorFragment =
			InteractorView.GetFragmentData<FRecallInteractorFragment>();
		TargetPosition = InteractorFragment.ClosestPositionIndex;
	}

	// Check if position is occupied using the utility function
	const bool bIsOccupied = Recall::Interact::Position::Utils::IsPositionOccupied(
		InteractableView,
		TargetPosition,
		EntityManager,
		&Context.InstigatorEntity
	);

	// Apply inversion logic
	const bool bConditionPasses = bInvertCondition ? bIsOccupied : !bIsOccupied;

	if (!bConditionPasses)
	{
		if (bInvertCondition)
		{
			// Inverted: we required position to be occupied but it's not
			if (TargetPosition == INDEX_NONE)
			{
				OutFailedText = LOCTEXT("PositionNotOccupied", "Position is not occupied");
			}
			else
			{
				OutFailedText = FText::Format(
					LOCTEXT("PositionNotOccupiedIndex", "Position {0} is not occupied"),
					FText::AsNumber(TargetPosition)
				);
			}
		}
		else
		{
			// Normal: position is occupied when we need it to be available
			if (TargetPosition == INDEX_NONE)
			{
				OutFailedText = LOCTEXT("PositionOccupied", "Position is occupied");
			}
			else
			{
				OutFailedText = FText::Format(
					LOCTEXT("PositionOccupiedIndex", "Position {0} is occupied"),
					FText::AsNumber(TargetPosition)
				);
			}
		}
	}

	return bConditionPasses;
}

#undef LOCTEXT_NAMESPACE
