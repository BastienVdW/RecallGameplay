// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "StateTree/RecallStateTreeTaskBase.h"

#include "RecallGridNavigationTasks.generated.h"

USTRUCT()
struct RECALLGRIDSELECTION_API FRecallGridCellOffsetTaskInstanceData
{
	GENERATED_BODY()
	
	/** Source grid cell index to apply offset from */
	UPROPERTY(EditAnywhere, Category=Input)
	int32 GridCellIndex = INDEX_NONE;
	
	/** Horizontal offset (-/+ for left/right) */
	UPROPERTY(EditAnywhere, Category=Parameter)
	int32 OffsetX = 0;
	
	/** Vertical offset (-/+ for up/down) */
	UPROPERTY(EditAnywhere, Category=Parameter)
	int32 OffsetY = 0;
	
	/** Resulting grid cell index after applying offset */
	UPROPERTY(EditAnywhere, Category=Output)
	int32 OutputGridCellIndex = INDEX_NONE;
};

USTRUCT(meta=(DisplayName="RE Grid Selection Offset"))
struct RECALLGRIDSELECTION_API FRecallGridCellOffsetTask : public FRecallStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallGridCellOffsetTaskInstanceData;

protected:
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	/** If true, the task will return Succeeded status when offset is applied */
	UPROPERTY(EditAnywhere, Category=Parameter)
	bool bSucceedOnOffset = false;
	
protected:
	TStateTreeExternalDataHandle<class URecallGridSelectionSubsystem> GridSelectionSystemHandle;
};