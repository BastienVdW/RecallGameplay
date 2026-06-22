// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "Simulation/GameplayEffect/RecallGameplayEffectTraits.h"

#include "MassExtendedEntityTemplateRegistry.h"
#include "Simulation/GameplayEffect/RecallGameplayEffectFragments.h"
#include "Simulation/Physics/RecallPhysicsBodyFragment.h"
#include "Simulation/Transform/RecallTransformFragments.h"

//----------------------------------------------------------------------//
// URecallGameplayEffectTrait
//----------------------------------------------------------------------//
void URecallGameplayEffectTrait::BuildTemplate(FMassExtendedEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	BuildContext.AddFragment<FRecallGameplayEffectFragment>();
}

//----------------------------------------------------------------------//
// URecallGameplayEffectAreaTrait
//----------------------------------------------------------------------//
void URecallGameplayEffectAreaTrait::BuildTemplate(FMassExtendedEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	BuildContext.RequireFragment<FRecallTransformFragment>();
	BuildContext.RequireFragment<FRecallPhysicsSensorFragment>();
	
	auto& GameplayEffectAreaFragment = BuildContext.AddFragment_GetRef<FRecallGameplayEffectAreaFragment>();
	GameplayEffectAreaFragment.EffectClass = EffectClass;
}
