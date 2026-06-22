// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallGameRuleCommonConditions.h"

#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "System/Game/RecallGameRuleSubsystem.h"

//----------------------------------------------------------------------//
// FRecallGameRuleInProgressCondition
//----------------------------------------------------------------------//
bool FRecallGameRuleInProgressCondition::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(GameRuleSystemHandle);
	return true;
}

bool FRecallGameRuleInProgressCondition::TestCondition(FStateTreeExecutionContext& Context) const
{
	const URecallGameRuleSubsystem& GameRuleSystem = Context.GetExternalData(GameRuleSystemHandle);
	return GameRuleSystem.IsInProgress() != bInvert;
}
