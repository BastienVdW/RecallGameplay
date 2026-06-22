// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallAttributeConditions.h"

#include "MassEntityManager.h"
#include "MassEntityView.h"
#include "Simulation/Attribute/RecallAttributeFragments.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "StateTree/RecallStateTreeExecutionContext.h"

namespace Recall::StateTree::Conditions
{

	template<typename T>
	bool CompareNumbers(const T Left, const T Right, const EGenericAICheck Operator)
	{
		switch (Operator)
		{
		case EGenericAICheck::Equal:
			return Left == Right;

		case EGenericAICheck::NotEqual:
			return Left != Right;

		case EGenericAICheck::Less:
			return Left < Right;

		case EGenericAICheck::LessOrEqual:
			return Left <= Right;

		case EGenericAICheck::Greater:
			return Left > Right;

		case EGenericAICheck::GreaterOrEqual:
			return Left >= Right;

		default:
			ensureMsgf(false, TEXT("Unhandled operator %d"), Operator);
			return false;
		}
	}

} // UE::StateTree::Conditions

//----------------------------------------------------------------------//
// FRecallStateTreeAttributeCompareCondition
//----------------------------------------------------------------------//
bool FRecallStateTreeAttributeCompareCondition::Link(FStateTreeLinker& Linker)
{
	return true;
}

bool FRecallStateTreeAttributeCompareCondition::TestCondition(FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	const FRecallStateTreeExecutionContext& MassContext = static_cast<FRecallStateTreeExecutionContext&>(Context);
	
	const FMassEntityManager& EntityManager = MassContext.GetEntityManager();	
	if (!EntityManager.IsEntityValid(InstanceData.EntityHandle))
	{
		return false;
	}
	
	const FMassEntityView EntityView(EntityManager, InstanceData.EntityHandle);
	const FRecallAttributeFragment* AttributeFragmentPtr = EntityView.GetFragmentDataPtr<FRecallAttributeFragment>();
	if (AttributeFragmentPtr == nullptr)
	{
	    return false;	
	}
	
	const int32 Left = AttributeFragmentPtr->AttributeSet.GetValue(InstanceData.Left);
	const bool bResult = Recall::StateTree::Conditions::CompareNumbers<int32>(Left, InstanceData.Right, Operator);
	return bResult != bInvert;
}
