// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "Simulation/Movement/RecallMovementTraits.h"

#include "MassEntityTemplateRegistry.h"
#include "Simulation/Movement/RecallMovementFragments.h"
#include "Simulation/Physics/RecallPhysicsBodyFragment.h"
#include "Simulation/Transform/RecallTransformFragments.h"

void URecallMovementTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	FMassEntityManager& EntityManager = UE::Mass::Utils::GetEntityManagerChecked(World);

	BuildContext.RequireFragment<FRecallTransformFragment>();
	BuildContext.RequireFragment<FRecallPhysicsBodyFragment>();

	BuildContext.AddFragment<FRecallMovementFragment>();

	FRecallMovementSharedFragment SharedFragment;
	SharedFragment.MovementSettings = MovementSettings;

	BuildContext.AddConstSharedFragment(EntityManager.GetOrCreateConstSharedFragment(SharedFragment));
}
