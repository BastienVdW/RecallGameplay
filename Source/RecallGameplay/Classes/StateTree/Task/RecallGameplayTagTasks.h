// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Mass/EntityHandle.h"
#include "StateTree/RecallStateTreeTaskBase.h"
#include "StateTreePropertyRef.h"

#include "RecallGameplayTagTasks.generated.h"

UENUM()
enum class ERecallGetTaggedEntityTaskSource : uint8
{
	AllMutable,
	Selection,
};

USTRUCT()
struct RECALLGAMEPLAY_API FRecallGetTaggedEntityTaskInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	ERecallGetTaggedEntityTaskSource Source = ERecallGetTaggedEntityTaskSource::AllMutable;

	UPROPERTY(EditAnywhere, meta=(EditCondition="Source == ERecallGetTaggedEntityTaskSource::Selection", EditConditionHides))
	TArray<FMassEntityHandle> SelectedEntities;
	
	UPROPERTY(EditAnywhere, Category=Input, meta=(CanRefToArray), DisplayName="Result")
	TStateTreePropertyRef<FMassEntityHandle> Entity;
	
	UPROPERTY(EditAnywhere, Category=Parameter)
	FGameplayTagContainer RequiredGameplayTags;
};

USTRUCT(meta=(DisplayName="Entity By Tag"))
struct RECALLGAMEPLAY_API FRecallGetTaggedEntityTask : public FRecallStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallGetTaggedEntityTaskInstanceData;

protected:
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	/** This task succeed as soon as it get the entity */
	UPROPERTY(EditAnywhere)
	bool bSucceedOnGet = true;
	
	UPROPERTY(EditAnywhere)
	TArray<FName> RequiredNameTags;

private:
	TArray<FMassEntityHandle> GetEntities(FStateTreeExecutionContext& Context) const;
};

UENUM()
enum class ERecallAddGameplayTagTarget : uint8
{
	Self,
	Entity,
};

USTRUCT()
struct RECALLGAMEPLAY_API FRecallAddGameplayTagTaskInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category=Parameter)
	ERecallAddGameplayTagTarget Target = ERecallAddGameplayTagTarget::Self;
	
	UPROPERTY(EditAnywhere, Category=Parameter, meta=(EditCondition="Target == ERecallAddGameplayTagTarget::Entity", EditConditionHides))
	FMassEntityHandle Entity;
};

/**
 * Add a gameplay tag. By default this gameplay tag will be removed once the state tree exit this task.
 */
USTRUCT(meta=(DisplayName="Add Gameplay Tag"))
struct RECALLGAMEPLAY_API FRecallAddGameplayTagTask : public FRecallStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallAddGameplayTagTaskInstanceData;

public:
	FRecallAddGameplayTagTask();

protected:
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

protected:
	UPROPERTY(EditAnywhere, Category=Parameter)
	FGameplayTagContainer GameplayTags;
	
	UPROPERTY(EditAnywhere, Category=Parameter)
	bool bRemoveOnExit = true;
	
private:	
	TStateTreeExternalDataHandle<struct FRecallGameplayTagFragment> GameplayTagFragmentHandle;

	struct FRecallGameplayTagFragment* GetTargetGameplayTagFragment(FStateTreeExecutionContext& Context) const;	
};
