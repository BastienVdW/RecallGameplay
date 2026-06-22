// Van de Walle Bastien

#pragma once

#include "StateTree/RecallStateTreeEvaluatorBase.h"
#include "StateTreeExecutionTypes.h"

#include "RecallGridEmptyCellsEvaluator.generated.h"

class URecallGridSelectionSubsystem;

USTRUCT()
struct FRecallGridEmptyCellsEvaluatorInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, Category = "Output")
	TArray<int32> EmptyCellIndices;
	
	UPROPERTY(VisibleAnywhere, Category = "Output")
	int32 EmptyCellCount = 0;
};

USTRUCT(meta=(DisplayName="RE Grid Empty Cells"))
struct RECALLGRIDSELECTION_API FRecallGridEmptyCellsEvaluator : public FRecallStateTreeEvaluatorBase
{
	GENERATED_BODY()
	
	using FInstanceDataType = FRecallGridEmptyCellsEvaluatorInstanceData;
	
protected:
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual void TreeStart(FStateTreeExecutionContext& Context) const override;
	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

protected:
	TStateTreeExternalDataHandle<class URecallGridSelectionSubsystem> GridSelectionSystemHandle;
};