// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "Simulation/Grid/RecallGridCursorTraits.h"

#include "MassExtendedEntityTemplateRegistry.h"
#include "Simulation/Controller/RecallControllerFragments.h"
#include "Simulation/Grid/RecallGridCursorFragments.h"
#include "Simulation/Transform/RecallTransformFragments.h"

//----------------------------------------------------------------------//
// URecallGridCursorTrait
//----------------------------------------------------------------------//
void URecallGridCursorTrait::BuildTemplate(FMassExtendedEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	BuildContext.RequireFragment<FRecallTransformFragment>();
	
	BuildContext.AddFragment<FRecallGridSelectionFragment>();
}

//----------------------------------------------------------------------//
// URecallGridCursorOwnerTrait
//----------------------------------------------------------------------//
void URecallGridCursorOwnerTrait::BuildTemplate(FMassExtendedEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	FMassExtendedEntityManager& EntityManager = UE::MassExtended::Utils::GetEntityManagerChecked(World);
	
	BuildContext.RequireFragment<FRecallControllerFragment>();
	
	BuildContext.AddFragment<FRecallGridCursorOwnerFragment>();
	
	FRecallGridCursorOwnerConstSharedFragment SharedFragment;
	SharedFragment.GridSelectionEntityConfig = GridSelectionEntityConfig;
	SharedFragment.CursorActorConfig = CursorActorConfig;

	BuildContext.AddConstSharedFragment(EntityManager.GetOrCreateConstSharedFragment(SharedFragment));
}
