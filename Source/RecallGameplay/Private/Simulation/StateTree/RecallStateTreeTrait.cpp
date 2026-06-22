// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "Simulation/StateTree/RecallStateTreeTrait.h"

#include "MassExtendedEntityTemplateRegistry.h"
#include "Simulation/StateTree/RecallStateTreeFragments.h"
#include "StateTree/RecallStateTreeTypes.h"
#include "StateTree.h"
#include "Simulation/StateTree/RecallStateTreeTokenFragments.h"
#include "System/AI/RecallStateTreeSubsystem.h"
#include "Utility/Trait/RecallTraitUtils.h"

URecallStateTreeTrait::URecallStateTreeTrait()
	: Super()
{
}

void URecallStateTreeTrait::BuildTemplate(FMassExtendedEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	FMassExtendedEntityManager& EntityManager = UE::MassExtended::Utils::GetEntityManagerChecked(World);

	URecallStateTreeSubsystem* StateTreeSubsystem = World.GetSubsystem<URecallStateTreeSubsystem>();
	if (!StateTreeSubsystem)
	{
		UE_LOG(LogRecallStateTree, Error, TEXT("Failed to get Mass StateTree Subsystem."));
		return;
	}

	if (!StateTree)
	{
		UE_LOG(LogRecallStateTree, Error, TEXT("StateTree asset is not set or unavailable."));
		return;
	}

	if (!StateTree->IsReadyToRun())
	{
		UE_LOG(LogRecallStateTree, Error, TEXT("StateTree asset is ready to run."));
		return;
	}

	FRecallStateTreeSharedFragment StateTreeConstSharedFragment;
	StateTreeConstSharedFragment.bAutoStart = bAutoStart;
	StateTreeConstSharedFragment.StateTree = StateTree;

	BuildContext.AddConstSharedFragment(EntityManager.GetOrCreateConstSharedFragment(StateTreeConstSharedFragment));

	BuildContext.AddFragment<FRecallStateTreeInstanceFragment>();

	FRecallStateTreeTokenSharedFragment TokenSharedFragment;
	BuildContext.AddSharedFragment(EntityManager.GetOrCreateSharedFragment(TokenSharedFragment));
}
