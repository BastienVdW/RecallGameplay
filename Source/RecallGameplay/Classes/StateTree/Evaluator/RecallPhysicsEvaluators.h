// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "StateTree/RecallStateTreeEvaluatorBase.h"
#include "MassExtendedEntityTypes.h"

#include "RecallPhysicsEvaluators.generated.h"

USTRUCT()
struct RECALLGAMEPLAY_API FRecallOverlappingEntityEvaluatorInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, Category=Output)
	bool bFound = false;

	UPROPERTY(VisibleAnywhere, Category=Output)
	TArray<FMassExtendedEntityHandle> Entities;
};

/**
* Return the first entity sensed by the sensor trait
*/
USTRUCT(meta=(DisplayName="Sense Entity"))
struct RECALLGAMEPLAY_API FRecallOverlappingEntityEvaluator : public FRecallStateTreeEvaluatorBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallOverlappingEntityEvaluatorInstanceData;

protected:
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual void TreeStart(FStateTreeExecutionContext& Context) const override;
	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

	UPROPERTY(EditAnywhere)
	FName SensorName = NAME_None;

protected:
	TStateTreeExternalDataHandle<struct FRecallPhysicsSensorFragment> SensorFragmentHandle;
};
