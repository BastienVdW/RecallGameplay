// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "Data/Interact/RecallInteractTypes.h"
#include "Mass/EntityHandle.h"
#include "StateTreePropertyRef.h"
#include "StateTree/RecallStateTreeTaskBase.h"

#include "RecallInteractTasks.generated.h"

struct FRecallInteractExecuteContext;

UENUM(BlueprintType)
enum class ERecallInteractSelection : uint8
{
	Target,
	Closest				UMETA(DisplayName="Closest Interactable"),
	Contextual,
};

USTRUCT()
struct RECALLINTERACTMODULE_API FRecallInteractTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category=Parameter)
	ERecallInteractInput Input = ERecallInteractInput::Primary;
	
	UPROPERTY(EditAnywhere, Category=Parameter)
	ERecallInteractSelection Selection = ERecallInteractSelection::Target;
	
	UPROPERTY(EditAnywhere, Category=Input, meta=(Optional, EditCondition="Selection==ERecallInteractSelection::Target", EditConditionHides))
	TStateTreePropertyRef<FMassEntityHandle> Target;

	/**
	 * Whether the state tree is allowed to exit the interaction on release.
	 */
	UPROPERTY(EditAnywhere, Category=Output)
	bool bExitOnRelease = true;
};

USTRUCT(meta=(DisplayName="Interact"))
struct RECALLINTERACTMODULE_API FRecallInteractTask : public FRecallStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallInteractTaskInstanceData;

	FRecallInteractTask();

protected:
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

protected:
	TStateTreeExternalDataHandle<struct FRecallInteractorFragment> InteractorFragmentHandle;
	TStateTreeExternalDataHandle<struct FRecallGameplayTagFragment, EStateTreeExternalDataRequirement::Optional> GameplayTagFragmentHandle;
	TStateTreeExternalDataHandle<struct FRecallAttributeFragment, EStateTreeExternalDataRequirement::Optional> AttributeFragmentHandle;

private:
	FRecallInteractExecuteContext CreateInteractionExecutionContext(FStateTreeExecutionContext& Context) const;
	float GetInteractionProgressTickRate(FStateTreeExecutionContext& Context) const;
	FMassEntityHandle GetInteractSelection(FStateTreeExecutionContext& Context) const;
	bool IsContextualInteraction(FStateTreeExecutionContext& Context) const;
};

USTRUCT()
struct RECALLINTERACTMODULE_API FRecallFindInteractableTaskInstanceData
{
	GENERATED_BODY()

	/** Array of entities to search through */
	UPROPERTY(EditAnywhere, Category=Input)
	TArray<FMassEntityHandle> Entities;

	/** Interaction input type to validate (Primary, Secondary, etc.) */
	UPROPERTY(EditAnywhere, Category=Parameter)
	ERecallInteractInput Input = ERecallInteractInput::Any;

	/** The best interactable entity found (output) */
	UPROPERTY(VisibleAnywhere, Category=Output)
	FMassEntityHandle BestInteractable;

	/** Nearest position index for the best interactable (INDEX_NONE if center position) */
	UPROPERTY(VisibleAnywhere, Category=Output)
	int32 NearestPositionIndex = INDEX_NONE;

	/** World location of the interaction point (output) */
	UPROPERTY(VisibleAnywhere, Category=Output)
	FVector InteractLocation = FVector::ZeroVector;
};

/**
 * Finds the best interactable entity from an array based on distance and interaction validity.
 * Returns the entity, its nearest interaction position index, and world location.
 */
USTRUCT(meta=(DisplayName="Find Best Interactable"))
struct RECALLINTERACTMODULE_API FRecallFindInteractableTask : public FRecallStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallFindInteractableTaskInstanceData;

protected:
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	/** This task succeeds as soon as we find a valid interactable entity */
	UPROPERTY(EditAnywhere, Category=Parameter)
	bool bSucceedOnFind = true;

private:
	TStateTreeExternalDataHandle<struct FRecallInteractorFragment> InteractorFragmentHandle;
	TStateTreeExternalDataHandle<struct FRecallGameplayTagFragment, EStateTreeExternalDataRequirement::Optional> GameplayTagFragmentHandle;
	TStateTreeExternalDataHandle<struct FRecallAttributeFragment, EStateTreeExternalDataRequirement::Optional> AttributeFragmentHandle;
	TStateTreeExternalDataHandle<struct FRecallTransformFragment> TransformFragmentHandle;
};
