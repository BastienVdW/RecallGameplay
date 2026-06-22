// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "StateTree/RecallStateTreeTaskBase.h"
#include "Simulation/StateTree/RecallStateTreeTokenHandle.h"
#include "Mass/EntityHandle.h"

#include "RecallTokenTasks.generated.h"

USTRUCT()
struct RECALLGAMEPLAY_API FRecallTokenTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category=Parameter)
	FMassEntityHandle TargetEntity;
	
	UPROPERTY(EditAnywhere, Category=Parameter, meta=(ClampMin=1))
	int32 TokenLimit = 5;

	UPROPERTY()
	FRecallStateTreeTokenHandle TokenHandle;
};


USTRUCT(meta=(DisplayName="Request Token"))
struct RECALLGAMEPLAY_API FRecallTokenTask : public FRecallStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallTokenTaskInstanceData;

public:
	FRecallTokenTask();

protected:
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

protected:
	TStateTreeExternalDataHandle<struct FRecallStateTreeTokenSharedFragment> TokenSharedFragmentHandle;
};
