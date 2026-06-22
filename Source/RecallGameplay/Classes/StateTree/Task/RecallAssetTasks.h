// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "StateTree/RecallStateTreeTaskBase.h"
#include "System/Asset/RecallAssetManagerTypes.h"

#include "RecallAssetTasks.generated.h"

USTRUCT()
struct RECALLGAMEPLAY_API FRecallJRPGLoadAssetTaskInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, Category=Output)
	FRecallAssetLoadHandle AssetHandle;
	
	UPROPERTY(VisibleAnywhere, Category=Output)
	bool bLoaded = false;
};

USTRUCT(meta=(DisplayName="Load Asset"))
struct RECALLGAMEPLAY_API FRecallJRPGLoadAssetTask : public FRecallStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallJRPGLoadAssetTaskInstanceData;

protected:
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	
	UPROPERTY(EditAnywhere)
	FSoftObjectPath AssetPath;
	
protected:
	TStateTreeExternalDataHandle<class URecallAssetManagerSubsystem> AssetManagerSystemHandle;
};
