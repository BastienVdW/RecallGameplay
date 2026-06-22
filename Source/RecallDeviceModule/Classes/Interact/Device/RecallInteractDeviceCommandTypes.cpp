// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallInteractDeviceCommandTypes.h"

#include "Mass/EntityElementTypes.h"
#include "MassEntityView.h"
#include "Simulation/Device/RecallDeviceFragments.h"
#include "Utility/Representation/RecallRepresentationUtils.h"

static FRecallDevicePlacerFragment* GetDevicePlacerFragment(const FRecallInteractContext& Context)
{
	const FMassEntityHandle TargetEntity = Context.GetTargetEntity(ERecallInteractTarget::Instigator);
	const FMassEntityView TargetEntityView(Context.GetEntityManagerChecked(), TargetEntity);

	auto* DevicePlacerFragmentPtr = TargetEntityView.GetFragmentDataPtr<FRecallDevicePlacerFragment>();
	if (DevicePlacerFragmentPtr == nullptr)
	{
		UE_LOG(LogRecallInteractCommand, Warning,
			TEXT("%hs Requires device placer fragment to build a device"), __FUNCTION__);
	}

	return DevicePlacerFragmentPtr;
}

//----------------------------------------------------------------------//
// FRecallInteractDeviceBuildCommand
//----------------------------------------------------------------------//
void FRecallInteractDeviceBuildCommand::OnBegin(const FRecallInteractContext& Context) const
{
}

void FRecallInteractDeviceBuildCommand::OnEnd(const FRecallInteractContext& Context) const
{
	FRecallDevicePlacerFragment* DevicePlacerFragmentPtr = GetDevicePlacerFragment(Context);	
	
	if (DevicePlacerFragmentPtr != nullptr)
	{
		// Close the device selection UI, if it is opened
		if (DevicePlacerFragmentPtr->BuildDeviceSlotEntity.IsSet())
		{
			Recall::Representation::Utils::SendUIEvent(
				Context.GetWorld(), Context.InstigatorEntity, UIEvent_CloseDeviceSelection);
		
			DevicePlacerFragmentPtr->BuildDeviceSlotEntity.Reset();
		}
		DevicePlacerFragmentPtr->bBuildDevice = false;
	}
}

void FRecallInteractDeviceBuildCommand::OnExecute(const FRecallInteractContext& Context) const
{
	// Open the device selection UI, and start the build device process
	FRecallDevicePlacerFragment* DevicePlacerFragmentPtr = GetDevicePlacerFragment(Context);
	if (DevicePlacerFragmentPtr != nullptr && ensure(!DevicePlacerFragmentPtr->BuildDeviceSlotEntity.IsSet()))
	{
		DevicePlacerFragmentPtr->bBuildDevice = true;
		DevicePlacerFragmentPtr->BuildDeviceSlotEntity = Context.InteractableEntity;
	
		Recall::Representation::Utils::SendUIEvent(
			Context.GetWorld(), Context.InstigatorEntity, UIEvent_OpenDeviceSelection);
	}
}

bool FRecallInteractDeviceBuildCommand::CanEndInteraction(const FRecallInteractContext& Context) const
{
	// Prevent the interaction to end if the device building process is still running
	const FRecallDevicePlacerFragment* DevicePlacerFragmentPtr = GetDevicePlacerFragment(Context);
	if (DevicePlacerFragmentPtr != nullptr)
	{
		return !DevicePlacerFragmentPtr->bBuildDevice;
	}
	return true;
}
