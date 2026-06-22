// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "Simulation/Representation/RecallTrackerRepresentationTraits.h"

#include "MassEntityTemplateRegistry.h"
#include "MassEntityView.h"
#include "Simulation/Representation/RecallTrackerRepresentationFragments.h"
#include "Simulation/Transform/RecallTransformFragments.h"
#include "Utility/Trait/RecallTraitUtils.h"

//----------------------------------------------------------------------//
// URecallTrackerRepresentationTrait
//----------------------------------------------------------------------//
void URecallTrackerRepresentationTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	FMassEntityManager& EntityManager = UE::Mass::Utils::GetEntityManagerChecked(World);
	
	BuildContext.RequireFragment<FRecallTransformFragment>();

	FRecallTrackerRepresentationConstSharedFragment SharedFragment;
	SharedFragment.TrackerWidgetClass = TrackerWidgetClass;

	BuildContext.AddConstSharedFragment(EntityManager.GetOrCreateConstSharedFragment(SharedFragment));
}
