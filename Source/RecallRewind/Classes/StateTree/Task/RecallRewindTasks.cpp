// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallRewindTasks.h"

#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "System/Rewind/RecallRewindSubsystem.h"

//----------------------------------------------------------------------//
// FRecallRewindTask
//----------------------------------------------------------------------//
bool FRecallRewindTask::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(RewindSystemHandle);
	return true;
}

EStateTreeRunStatus FRecallRewindTask::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	URecallRewindSubsystem& RewindSystem = Context.GetExternalData(RewindSystemHandle);
	RewindSystem.Rewind();

	return EStateTreeRunStatus::Succeeded;
}

void FRecallRewindTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	return Super::ExitState(Context, Transition);
}

EStateTreeRunStatus FRecallRewindTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	return Super::Tick(Context, DeltaTime);
}
