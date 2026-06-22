// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Mass/EntityElementTypes.h"
#include "Mass/EntityHandle.h"

#include "RecallDeviceFragments.generated.h"

USTRUCT()
struct RECALLDEVICEMODULE_API FRecallDevicePlacerFragment : public FMassFragment
{
	GENERATED_BODY()

	/**
	 * Keep track of the entity handle of the device currently being placed.
	 */
	UPROPERTY(VisibleAnywhere)
	FMassEntityHandle PlaceDeviceEntity;

	/**
	 * In the process of building a device.
	 */
	UPROPERTY(VisibleAnywhere)
	bool bBuildDevice = false;
	
	/**
	 * Keep track of the current device slot entity where we can build an entity.
	 */
	UPROPERTY(VisibleAnywhere)
	FMassEntityHandle BuildDeviceSlotEntity;
};
