// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallDeviceCostTypes.h"

//----------------------------------------------------------------------//
// FRecallDeviceCostBase
//----------------------------------------------------------------------//
bool FRecallDeviceCostBase::Evaluate(const FRecallDeviceCostContext& Context) const
{
	unimplemented();
	return false;
}

void FRecallDeviceCostBase::Consume(const FRecallDeviceCostContext& Context) const
{
	unimplemented();
}

//----------------------------------------------------------------------//
// FRecallDeviceCostAnd
//----------------------------------------------------------------------//
bool FRecallDeviceCostAnd::Evaluate(const FRecallDeviceCostContext& Context) const
{
	for (const FInstancedStruct& Cost : Costs)
	{
		const FRecallDeviceCostBase* CostPtr = Cost.GetPtr<FRecallDeviceCostBase>();
		if (CostPtr != nullptr && !CostPtr->Evaluate(Context))
		{
			return false;
		}
	}

	return true;
}

void FRecallDeviceCostAnd::Consume(const FRecallDeviceCostContext& Context) const
{	
	for (const FInstancedStruct& Cost : Costs)
	{
		const FRecallDeviceCostBase* CostPtr = Cost.GetPtr<FRecallDeviceCostBase>();
		if (CostPtr != nullptr)
		{
			CostPtr->Consume(Context);
		}
	}
}

//----------------------------------------------------------------------//
// FRecallDeviceCostOr
//----------------------------------------------------------------------//
bool FRecallDeviceCostOr::Evaluate(const FRecallDeviceCostContext& Context) const
{
	return Evaluate_Internal(Context);
}

void FRecallDeviceCostOr::Consume(const FRecallDeviceCostContext& Context) const
{
	Evaluate_Internal(Context, true);
}

bool FRecallDeviceCostOr::Evaluate_Internal(const FRecallDeviceCostContext& Context, bool bConsume) const
{
	bool bResult = true;
	
	for (const FInstancedStruct& Cost : Costs)
	{
		const FRecallDeviceCostBase* CostPtr = Cost.GetPtr<FRecallDeviceCostBase>();
		if (CostPtr == nullptr)
		{
			continue;
		}

		bResult |= CostPtr->Evaluate(Context);
		if (bResult)
		{
			CostPtr->Consume(Context);
			break;
		}
		else
		{
			return false;
		}
	}

	return bResult;
}
