// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallInteractCarryCommandTypes.h"

#include "MassEntityView.h"
#include "RecallSignalSubsystem.h"
#include "Simulation/Carry/RecallCarryFragments.h"
#include "Simulation/Carry/RecallCarrySignalTypes.h"

//----------------------------------------------------------------------//
// FRecallCarryCommand
//----------------------------------------------------------------------//
bool FRecallInteractCarryCommand::Validate(const FRecallInteractContext& Context) const
{
	const FMassEntityView CarryableView(Context.GetEntityManagerChecked(), Context.InteractableEntity);
	const FRecallCarryableFragment* CarryableFragmentPtr = CarryableView.GetFragmentDataPtr<FRecallCarryableFragment>();
	if (CarryableFragmentPtr == nullptr)
	{
		UE_LOG(LogRecallInteractCommand, Warning,
			TEXT("%hs Can only carry carryable entities."), __FUNCTION__);
		return false;
	}

	const auto& CarryableConstSharedFragment = CarryableView.GetConstSharedFragmentData<FRecallCarryableConstSharedFragment>();
	const bool bIsCarrier = CarryableFragmentPtr->CarrierEntities.Contains(Context.InstigatorEntity);
	const int32 CarrierCount = CarryableFragmentPtr->GetCarrierCount();

	// Allow picking up if instigator is already a carrier (e.g., to adjust shoulder)
	if (bIsCarrier)
	{
		return true;
	}

	// Block if the carryable is already being carried and we've reached the maximum carrier limit
	if (CarryableConstSharedFragment.CanAddCarrier(CarrierCount))
	{
		return true;
	}

	return false;
}

void FRecallInteractCarryCommand::OnBegin(const FRecallInteractContext& Context) const
{
	if (Context.SignalSystemPtr != nullptr)
	{
		Context.SignalSystemPtr->SignalEntity(Recall::Carry::Signals::Start, Context.InteractableEntity);
	}
}

void FRecallInteractCarryCommand::OnEnd(const FRecallInteractContext& Context) const
{
	if (Context.SignalSystemPtr != nullptr)
	{
		Context.SignalSystemPtr->SignalEntity(Recall::Carry::Signals::Stop, Context.InteractableEntity);
	}
}
