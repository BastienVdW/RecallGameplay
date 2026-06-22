// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallInteractPlayerConditions.h"

#include "Simulation/Interact/RecallInteractFragments.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"

//----------------------------------------------------------------------//
// FRecallStateTreeClosestInteractCondition
//----------------------------------------------------------------------//
bool FRecallStateTreeClosestInteractCondition::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(InteractorFragmentHandle);
	return true;
}

bool FRecallStateTreeClosestInteractCondition::TestCondition(FStateTreeExecutionContext& Context) const
{
	const FRecallInteractorFragment& InputFragment = Context.GetExternalData(InteractorFragmentHandle);
	return InputFragment.ClosestInteractableEntity.IsSet() != bInvert;
}
