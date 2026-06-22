// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "StateTree/RecallStateTreeTaskBase.h"
#include "Gameplay/RecallGameplayEffectHandle.h"

#include "RecallGameplayEffectTasks.generated.h"

USTRUCT()
struct RECALLGAMEPLAYEFFECTMODULE_API FRecallAddGameplayEffectTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category=Output)
	FRecallGameplayEffectHandle EffectHandle;
};

USTRUCT(meta=(DisplayName="Add Gameplay Effect"))
struct RECALLGAMEPLAYEFFECTMODULE_API FRecallAddGameplayEffectTask : public FRecallStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallAddGameplayEffectTaskInstanceData;

	FRecallAddGameplayEffectTask();
	
protected:
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	UPROPERTY(EditAnywhere, Category=Parameter)
	bool bSucceedOnAdd = true;
	
	UPROPERTY(EditAnywhere, Category=Parameter, meta=(EditCondition="!bSucceedOnAdd"))
	bool bRemoveOnExit = false;
	
	UPROPERTY(EditAnywhere, Category=Parameter)
	TSubclassOf<class URecallGameplayEffect> Effect;

private:
	TStateTreeExternalDataHandle<struct FRecallGameplayEffectFragment> GameplayEffectFragmentHandle;
};

USTRUCT()
struct RECALLGAMEPLAYEFFECTMODULE_API FRecallRemoveGameplayEffectTaskInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category=Parameter)
	FRecallGameplayEffectHandle EffectHandle;
};

USTRUCT(meta=(DisplayName="Remove Gameplay Effect"))
struct RECALLGAMEPLAYEFFECTMODULE_API FRecallRemoveGameplayEffectTask : public FRecallStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallRemoveGameplayEffectTaskInstanceData;

	FRecallRemoveGameplayEffectTask();
	
protected:
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	UPROPERTY(EditAnywhere, Category=Parameter)
	TSubclassOf<class URecallGameplayEffect> Effect;
	
	UPROPERTY(EditAnywhere, Category=Parameter)
	bool bSucceedOnRemove = true;
	
private:
	TStateTreeExternalDataHandle<struct FRecallGameplayEffectFragment> GameplayEffectFragmentHandle;
};
