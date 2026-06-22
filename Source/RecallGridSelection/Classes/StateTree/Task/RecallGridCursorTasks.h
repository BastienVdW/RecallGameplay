// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "StateTree/RecallStateTreeTaskBase.h"

#include "RecallGridCursorTasks.generated.h"

USTRUCT()
struct RECALLGRIDSELECTION_API FRecallGridSetCursorPositionTaskInstanceData
{
	GENERATED_BODY()
	
	/** Target grid cell index to set cursor position to */
	UPROPERTY(EditAnywhere, Category=Input)
	int32 GridCellIndex = INDEX_NONE;
};

USTRUCT(meta=(DisplayName="RE Grid Set Cursor Position"))
struct RECALLGRIDSELECTION_API FRecallGridSetCursorPositionTask : public FRecallStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallGridSetCursorPositionTaskInstanceData;

protected:
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	/** When true, the cursor position will follow the mouse position instead of using the specified grid cell index */
	UPROPERTY(EditAnywhere, Category=Parameter)
	bool bUseMousePosition = false;
	
protected:
	TStateTreeExternalDataHandle<struct FRecallGridCursorOwnerFragment> CursorOwnerFragmentHandle;
};

USTRUCT()
struct RECALLGRIDSELECTION_API FRecallGridMoveCursorToTargetTaskInstanceData
{
	GENERATED_BODY()
	
	/** Target grid cell index to move cursor to */
	UPROPERTY(EditAnywhere, Category=Input)
	int32 TargetGridCellIndex = INDEX_NONE;
	
	/** Movement speed in cells per second */
	UPROPERTY(EditAnywhere, Category=Parameter)
	float MovementSpeed = 3.0f;
	
	/** Timer for movement timing */
	UPROPERTY()
	float MovementTimer = 0.0f;
	
	/** 
	 * Calculates the time interval between cursor movements based on movement speed
	 * @return Time in seconds between movement steps. Returns 0.1s if movement speed is 0 or negative
	 */
	FORCEINLINE float GetMovementInterval() const
	{
		return MovementSpeed > 0.0f ? (1.0f / MovementSpeed) : 0.1f;
	}
};

USTRUCT(meta=(DisplayName="RE Grid Move Cursor To Target"))
struct RECALLGRIDSELECTION_API FRecallGridMoveCursorToTargetTask : public FRecallStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallGridMoveCursorToTargetTaskInstanceData;

protected:
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

protected:
	TStateTreeExternalDataHandle<struct FRecallGridCursorOwnerFragment> CursorOwnerFragmentHandle;
	TStateTreeExternalDataHandle<class URecallGridSelectionSubsystem> GridSelectionSystemHandle;
};
