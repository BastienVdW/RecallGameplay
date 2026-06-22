// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "Simulation/Device/RecallDeviceTraits.h"

#include "MassEntityTemplateRegistry.h"
#include "Simulation/Device/RecallDeviceFragments.h"
#include "Simulation/GameplayTag/RecallGameplayTagFragments.h"
#include "Simulation/Transform/RecallTransformFragments.h"

//----------------------------------------------------------------------//
// URecallDevicePlacerTrait
//----------------------------------------------------------------------//
void URecallDevicePlacerTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	BuildContext.RequireFragment<FRecallTransformFragment>();
	BuildContext.RequireFragment<FRecallGameplayTagFragment>();

	BuildContext.AddFragment<FRecallDevicePlacerFragment>();
}
