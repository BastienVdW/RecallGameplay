// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "Simulation/Attribute/RecallAttributeTraits.h"

#include "MassEntityTemplateRegistry.h"
#include "Simulation/Attribute/RecallAttributeFragments.h"

void URecallAttributeTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	FMassEntityManager& EntityManager = UE::Mass::Utils::GetEntityManagerChecked(World);

	BuildContext.AddFragment<FRecallAttributeFragment>();
	
	FRecallAttributeConstSharedFragment SharedFragment;
	SharedFragment.AttributeSet = AttributeSet;

	BuildContext.AddConstSharedFragment(EntityManager.GetOrCreateConstSharedFragment(SharedFragment));
}
