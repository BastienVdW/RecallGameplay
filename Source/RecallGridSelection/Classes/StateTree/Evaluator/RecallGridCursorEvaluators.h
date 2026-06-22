// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "StateTree/RecallStateTreeEvaluatorBase.h"
#include "StateTreeExecutionTypes.h"
#include "MassExtendedEntityHandle.h"

#include "RecallGridCursorEvaluators.generated.h"

USTRUCT()
struct RECALLGRIDSELECTION_API FRecallGridCursorSelectionEvaluatorInstanceData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category=Output)
	bool bFoundCell = false;

	UPROPERTY(VisibleAnywhere, Category=Output)
	int32 GridCellIndex = INDEX_NONE;
	
	UPROPERTY(VisibleAnywhere, Category=Output)
	FVector GridCellPosition = FVector::ZeroVector;
	
	UPROPERTY(VisibleAnywhere, Category=Output)
	FMassExtendedEntityHandle GridCellEntity;
	
	UPROPERTY(VisibleAnywhere, Category=Output)
	bool bIsGridCellEmpty = false;
};

USTRUCT(meta=(DisplayName="Grid Cursor Selection"))
struct RECALLGRIDSELECTION_API FRecallGridCursorSelectionEvaluator : public FRecallStateTreeEvaluatorBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallGridCursorSelectionEvaluatorInstanceData;

protected:
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual void TreeStart(FStateTreeExecutionContext& Context) const override;
	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

protected:
	TStateTreeExternalDataHandle<struct FRecallGridCursorOwnerFragment> GridCursorOwnerFragmentHandle;
	TStateTreeExternalDataHandle<class URecallGridSelectionSubsystem> GridSelectionSystemHandle;
};
