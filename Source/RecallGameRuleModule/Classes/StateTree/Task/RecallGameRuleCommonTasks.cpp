// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallGameRuleCommonTasks.h"

#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "System/Representation/RecallRepresentationEventSubsystem.h"
#include "Utility/Game/RecallGameUtils.h"

//----------------------------------------------------------------------//
// FRecallGameRuleEndMatchTask
//----------------------------------------------------------------------//
bool FRecallGameRuleEndMatchTask::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(RepresentationEventSystemHandle);
	return true;
}

EStateTreeRunStatus FRecallGameRuleEndMatchTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	URecallRepresentationEventSubsystem& RepresentationEventSystem = Context.GetExternalData(RepresentationEventSystemHandle);
	
	Recall::Game::Utils::EndGame(RepresentationEventSystem, InstanceData.Reason);
	
	return Super::EnterState(Context, Transition);
}
