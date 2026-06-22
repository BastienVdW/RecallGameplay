// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "Simulation/Cinematic/RecallCinematicTraits.h"

#include "MassEntityTemplateRegistry.h"
#include "RecallCinematicInternalFragments.h"
#include "Simulation/Cinematic/RecallCinematicFragments.h"
#include "Utility/Trait/RecallTraitUtils.h"

//----------------------------------------------------------------------//
// URecallCinematicTrait
//----------------------------------------------------------------------//
void URecallCinematicTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	FMassEntityManager& EntityManager = UE::Mass::Utils::GetEntityManagerChecked(World);

	BuildContext.AddFragment<FRecallCinematicFragment>();
	BuildContext.AddFragment<FRecallCinematicInternalFragment>();

	FRecallCinematicInternalSharedFragment InternalSharedFragment;
	InternalSharedFragment.LevelSequenceActorClass = LevelSequenceActorClass;

	BuildContext.AddConstSharedFragment(EntityManager.GetOrCreateConstSharedFragment(InternalSharedFragment));

	FRecallCinematicSharedFragment SharedFragment;
	SharedFragment.DefaultInstigatorBinding = DefaultInstigatorBinding;

	BuildContext.AddConstSharedFragment(EntityManager.GetOrCreateConstSharedFragment(SharedFragment));
}
