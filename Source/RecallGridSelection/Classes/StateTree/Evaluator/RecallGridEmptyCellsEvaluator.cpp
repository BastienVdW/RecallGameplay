// Van de Walle Bastien


#include "StateTree/Evaluator/RecallGridEmptyCellsEvaluator.h"

#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "System/Grid/RecallGridSelectionSubsystem.h"

bool FRecallGridEmptyCellsEvaluator::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(GridSelectionSystemHandle);
	return FRecallStateTreeEvaluatorBase::Link(Linker);
}

void FRecallGridEmptyCellsEvaluator::TreeStart(FStateTreeExecutionContext& Context) const
{
	FRecallStateTreeEvaluatorBase::TreeStart(Context);
	
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);
	
	// Initialize arrays
	InstanceData.EmptyCellIndices.Reset();
	InstanceData.EmptyCellCount = 0;
}

void FRecallGridEmptyCellsEvaluator::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FRecallStateTreeEvaluatorBase::Tick(Context, DeltaTime);
	
	const URecallGridSelectionSubsystem& GridSelectionSystem = Context.GetExternalData(GridSelectionSystemHandle);
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);
	
	// Get empty cells from grid selection system
	InstanceData.EmptyCellIndices.Reset();
	GridSelectionSystem.GetEmptyCellIndices(InstanceData.EmptyCellIndices);
	InstanceData.EmptyCellCount = InstanceData.EmptyCellIndices.Num();
}