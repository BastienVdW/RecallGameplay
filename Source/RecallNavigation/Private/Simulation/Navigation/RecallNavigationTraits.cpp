// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "Simulation/Navigation/RecallNavigationTraits.h"

#include "MassExtendedEntityTemplateRegistry.h"
#include "Simulation/Crowd/RecallCrowdFragments.h"
#include "Simulation/Movement/RecallMovementFragments.h"
#include "Simulation/Navigation/RecallNavigationFragments.h"
#include "Simulation/Navigation/RecallPathFollowingFragments.h"
#include "Simulation/Navigation/RecallNavigationObstacleFragments.h"
#include "Simulation/Navigation/RecallNavLinkFragments.h"
#include "Simulation/Physics/RecallPhysicsBodyFragment.h"
#include "Simulation/Transform/RecallTransformFragments.h"

//----------------------------------------------------------------------//
// URecallNavigationAgentBaseTrait
//----------------------------------------------------------------------//
void URecallNavigationAgentBaseTrait::BuildTemplate(FMassExtendedEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	BuildContext.RequireFragment<FRecallTransformFragment>();
	BuildContext.RequireFragment<FRecallMovementFragment>();
	BuildContext.RequireFragment<FRecallPhysicsBodyFragment>();

	// Add base path following fragments (navigation-system agnostic)
	BuildContext.AddFragment<FRecallPathFollowerFragment>();
	BuildContext.AddFragment<FRecallNavLinkTraversalFragment>();

	if (bCrowdAgent)
	{
		BuildContext.AddFragment<FRecallCrowdAgentFragment>();
		BuildContext.AddTag<FRecallCrowdMovementTag>();
	}

	// Add path following configuration shared fragment
	FMassExtendedEntityManager& EntityManager = UE::MassExtended::Utils::GetEntityManagerChecked(World);
	FRecallPathFollowingConstSharedFragment PathFollowingFragment;
	PathFollowingFragment.Settings = PathFollowingSettings;
	BuildContext.AddConstSharedFragment(EntityManager.GetOrCreateConstSharedFragment(PathFollowingFragment));

	// Add crowd agent collision configuration shared fragment if agent uses crowd system
	if (bCrowdAgent)
	{
		FRecallCrowdAgentCollisionConstSharedFragment CrowdAgentCollisionFragment;
		CrowdAgentCollisionFragment.Settings = CrowdAgentCollisionSettings;
		CrowdAgentCollisionFragment.Settings.AgentHeight = PathFollowingSettings.AgentHeight;
		BuildContext.AddConstSharedFragment(EntityManager.GetOrCreateConstSharedFragment(CrowdAgentCollisionFragment));
	}

	// Add obstacle fragment if enabled
	if (bNavigationObstacle)
	{
		auto& ObstacleFragment = BuildContext.AddFragment_GetRef<FRecallNavigationObstacleFragment>();
		ObstacleFragment.Radius = PathFollowingSettings.Radius;
	}
}

//----------------------------------------------------------------------//
// URecallNavigationAgentTrait
//----------------------------------------------------------------------//
void URecallNavigationAgentTrait::BuildTemplate(FMassExtendedEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	// Build base fragments first (now includes path following and crowd agent collision shared fragments, and obstacle fragment)
	Super::BuildTemplate(BuildContext, World);

	FMassExtendedEntityManager& EntityManager = UE::MassExtended::Utils::GetEntityManagerChecked(World);

	// Add NavMesh-specific path request fragment
	BuildContext.AddFragment<FRecallNavMeshPathRequestFragment>();

	// Add NavMesh-specific shared fragment for navigation-system-specific settings
	FRecallNavigationAgentConstSharedFragment ConstSharedFragment;
	ConstSharedFragment.Agent = AgentSettings;

	BuildContext.AddConstSharedFragment(EntityManager.GetOrCreateConstSharedFragment(ConstSharedFragment));
}
