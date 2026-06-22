// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallAnimationTasks.h"

#include "MassExtendedExecutionContext.h"
#include "RecallSignalSubsystem.h"
#include "Simulation/Cinematic/RecallCinematicFragments.h"
#include "Simulation/Cinematic/RecallCinematicSignalTypes.h"
#include "Simulation/Representation/RecallActorRepresentationFragments.h"
#include "StateTree/RecallStateTreeExecutionContext.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"

//----------------------------------------------------------------------//
// FRecallAnimationTask
//----------------------------------------------------------------------//
bool FRecallAnimationTask::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(CinematicFragmentHandle);
	Linker.LinkExternalData(CinematicSharedFragmentHandle);
	Linker.LinkExternalData(ActorRepresentationFragmentHandle);
	return true;
}

EStateTreeRunStatus FRecallAnimationTask::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	if (LevelSequence.IsNull())
	{
		return EStateTreeRunStatus::Failed;
	}

	const FRecallStateTreeExecutionContext& RecallContext = static_cast<FRecallStateTreeExecutionContext&>(Context);

	// Already playing something else
	const bool bIsPlaying = RecallContext.GetMassExecutionContext().DoesArchetypeHaveTag<FRecallCinematicPlayingTag>();
	if (bIsPlaying)
	{
		return EStateTreeRunStatus::Failed;
	}

	const FRecallCinematicSharedFragment& CinematicSharedFragment = Context.GetExternalData(CinematicSharedFragmentHandle);
	const FRecallActorRepresentationFragment& ActorFragment = Context.GetExternalData(ActorRepresentationFragmentHandle);

	FRecallCinematicFragment& CinematicFragment = Context.GetExternalData(CinematicFragmentHandle);
	CinematicFragment.LevelSequenceAsset = LevelSequence;
	CinematicFragment.Bindings.Add(FRecallCinematicBinding{ CinematicSharedFragment.DefaultInstigatorBinding, ActorFragment.ActorHandle });

	RecallContext.GetSignalSystem().SignalEntity(Recall::Cinematic::Signals::Player::Play, RecallContext.GetEntity());

	// Wait a frame so our level sequence get started
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.WaitDelay = 1;

	return Super::EnterState(Context, Transition);
}

void FRecallAnimationTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	return Super::ExitState(Context, Transition);
}

EStateTreeRunStatus FRecallAnimationTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.WaitDelay > 0)
	{
		InstanceData.WaitDelay--;
		return Super::Tick(Context, DeltaTime);
	}

	const FRecallStateTreeExecutionContext& RecallContext = static_cast<FRecallStateTreeExecutionContext&>(Context);
	const FRecallCinematicFragment& CinematicFragment = Context.GetExternalData(CinematicFragmentHandle);

	// Wait until we are done playing
	const bool bIsPlaying = RecallContext.GetMassExecutionContext().DoesArchetypeHaveTag<FRecallCinematicPlayingTag>();
	if (bIsPlaying && CinematicFragment.LevelSequenceAsset == LevelSequence)
	{
		return Super::Tick(Context, DeltaTime);
	}

	return EStateTreeRunStatus::Succeeded;
}
