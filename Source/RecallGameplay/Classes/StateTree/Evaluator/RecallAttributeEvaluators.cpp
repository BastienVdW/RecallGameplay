// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallAttributeEvaluators.h"

#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "Simulation/Attribute/RecallAttributeFragments.h"

//----------------------------------------------------------------------//
// FRecallAttributeEvaluator
//----------------------------------------------------------------------//
bool FRecallAttributeEvaluator::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(AttributeFragmentHandle);
	return true;
}

void FRecallAttributeEvaluator::TreeStart(FStateTreeExecutionContext& Context) const
{
}

void FRecallAttributeEvaluator::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{	
	const FRecallAttributeFragment& AttributeFragment = Context.GetExternalData(AttributeFragmentHandle);
	
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.Value = AttributeFragment.AttributeSet.GetValue(InstanceData.Attribute);
}
