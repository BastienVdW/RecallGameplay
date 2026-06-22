// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "StateTree/RecallStateTreeEvaluatorBase.h"
#include "GameplayTagContainer.h"
#include "StateTreePropertyRef.h"
#include "Mass/EntityHandle.h"
#include "System/AI/RecallStateTreeInstanceTypes.h"

#include "RecallCommonEvaluators.generated.h"

USTRUCT()
struct RECALLGAMEPLAY_API FRecallTowardLocationEvaluatorInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category=Parameter)
	FVector2f Direction = FVector2f::ZeroVector;
	
	UPROPERTY(EditAnywhere, Category=Parameter)
	bool bNormalize = false;
	
	UPROPERTY(EditAnywhere, Category=Parameter)
	bool bAllowZero = true;
	
	UPROPERTY(EditAnywhere, Category=Parameter, meta=(EditCondition="!bAllowZero"))
	FVector2f DefaultZeroValue = FVector2f::ZeroVector;
		
	UPROPERTY(VisibleAnywhere, Category=Output)
	FVector Location = FVector::ZeroVector;
};

USTRUCT(meta=(DisplayName="Toward Location"))
struct RECALLGAMEPLAY_API FRecallTowardLocationEvaluator : public FRecallStateTreeEvaluatorBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallTowardLocationEvaluatorInstanceData;

protected:
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual void TreeStart(FStateTreeExecutionContext& Context) const override;
	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	
	UPROPERTY(EditAnywhere)
	float AheadDistance = 100.0f;

protected:
	TStateTreeExternalDataHandle<struct FRecallTransformFragment> TransformFragmentHandle;
};

USTRUCT()
struct RECALLGAMEPLAY_API FRecallForwardVectorEvaluatorInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category=Input)
	FQuat Rotation = FQuat::Identity;
	
	UPROPERTY(VisibleAnywhere, Category=Output)
	FVector Vector = FVector::ZeroVector;
};

USTRUCT(meta=(DisplayName="Forward Vector"))
struct RECALLGAMEPLAY_API FRecallForwardVectorEvaluator : public FRecallStateTreeEvaluatorBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallForwardVectorEvaluatorInstanceData;

protected:
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual void TreeStart(FStateTreeExecutionContext& Context) const override;
	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
};

UENUM()
enum class ERecallEventPayloadType : uint8
{
	Vector,
	Entity,
};

UENUM()
enum class ERecallEventPayloadArrayOperation : uint8
{
	AddUnique,
	Add,
	Remove,
};

USTRUCT()
struct RECALLGAMEPLAY_API FRecallEventPayloadEvaluatorInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category=Parameter)
	ERecallEventPayloadType Type = ERecallEventPayloadType::Vector;
	
	UPROPERTY(VisibleAnywhere, Category=Input, meta=(CanRefToArray, Optional, EditCondition="Type == ERecallEventPayloadType::Vector", EditConditionHides), DisplayName="Result")
	TStateTreePropertyRef<FVector> VectorResult;
	
	UPROPERTY(VisibleAnywhere, Category=Input, meta=(CanRefToArray, Optional, EditCondition="Type == ERecallEventPayloadType::Entity", EditConditionHides), DisplayName="Result")
	TStateTreePropertyRef<FMassEntityHandle> EntityResult;

	UPROPERTY(Transient)
	TArray<FInstancedStruct> Payloads;
};

/**
* Get payloads for an event
*/
USTRUCT(meta=(DisplayName="Event Payload"))
struct RECALLGAMEPLAY_API FRecallEventPayloadEvaluator : public FRecallStateTreeEvaluatorBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallEventPayloadEvaluatorInstanceData;

protected:
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual void TreeStart(FStateTreeExecutionContext& Context) const override;
	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	
	UPROPERTY(EditAnywhere, meta=(Categories="StateTreeEvent"))
	FGameplayTag Tag;
	
	UPROPERTY(EditAnywhere)
	ERecallEventPayloadArrayOperation ArrayOperation = ERecallEventPayloadArrayOperation::AddUnique;
};

USTRUCT()
struct RECALLGAMEPLAY_API FRecallStateTreeEvaluatorInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, Category=Output)
	FRecallStateTreeInstanceHandle InstanceHandle;
};

/**
* Run parallel state tree
*/
USTRUCT(meta=(DisplayName="State Tree"))
struct RECALLGAMEPLAY_API FRecallStateTreeEvaluator : public FRecallStateTreeEvaluatorBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallStateTreeEvaluatorInstanceData;

protected:
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual void TreeStart(FStateTreeExecutionContext& Context) const override;
	virtual void TreeStop(FStateTreeExecutionContext& Context) const override;
	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStateTree> StateTree;

	UPROPERTY(EditAnywhere)
	FName StateTreeName = NAME_None;

private:
	TStateTreeExternalDataHandle<struct FRecallStateTreeInstanceFragment> StateTreeInstanceFragmentHandle;
	TStateTreeExternalDataHandle<class URecallStateTreeSubsystem> StateTreeSystemHandle;
};
