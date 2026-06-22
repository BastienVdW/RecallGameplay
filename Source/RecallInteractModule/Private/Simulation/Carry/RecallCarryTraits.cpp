// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "Simulation/Carry/RecallCarryTraits.h"

#include "MassExtendedEntityTemplateRegistry.h"
#include "Simulation/Carry/RecallCarryFragments.h"
#include "Simulation/Interact/RecallInteractFragments.h"
#include "Simulation/Transform/RecallTransformFragments.h"
#include "Utility/Trait/RecallTraitUtils.h"

void URecallCarryableTrait::BuildTemplate(FMassExtendedEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	FMassExtendedEntityManager& EntityManager = UE::MassExtended::Utils::GetEntityManagerChecked(World);

	BuildContext.RequireFragment<FRecallTransformFragment>();
	BuildContext.RequireFragment<FRecallInteractableFragment>();

	BuildContext.AddFragment<FRecallCarryableFragment>();

	BuildContext.AddTag<FRecallCarryableTag>();

	FRecallCarryableConstSharedFragment SharedFragment;
	SharedFragment.Settings = Settings;

	BuildContext.AddConstSharedFragment(EntityManager.GetOrCreateConstSharedFragment(SharedFragment));
}
