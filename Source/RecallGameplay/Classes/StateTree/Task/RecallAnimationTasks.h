// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "StateTree/RecallStateTreeTaskBase.h"

#include "RecallAnimationTasks.generated.h"

USTRUCT()
struct RECALLGAMEPLAY_API FRecallAnimationTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY()
	int32 WaitDelay = 0;
};

USTRUCT(meta=(DisplayName="Play Animation"))
struct RECALLGAMEPLAY_API FRecallAnimationTask : public FRecallStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallAnimationTaskInstanceData;

protected:
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	
	UPROPERTY(EditAnywhere, meta=(AllowedClasses="/Script/LevelSequence.LevelSequence"))
	FSoftObjectPath LevelSequence;

protected:
	TStateTreeExternalDataHandle<struct FRecallCinematicFragment> CinematicFragmentHandle;
	TStateTreeExternalDataHandle<struct FRecallCinematicSharedFragment> CinematicSharedFragmentHandle;
	TStateTreeExternalDataHandle<struct FRecallActorRepresentationFragment> ActorRepresentationFragmentHandle;
};
