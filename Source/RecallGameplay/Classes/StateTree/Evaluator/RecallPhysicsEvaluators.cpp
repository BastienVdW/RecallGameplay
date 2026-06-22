// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallPhysicsEvaluators.h"

#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "Simulation/Physics/RecallPhysicsSensorFragment.h"

//----------------------------------------------------------------------//
// FRecallOverlappingEntityEvaluator
//----------------------------------------------------------------------//
bool FRecallOverlappingEntityEvaluator::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(SensorFragmentHandle);
	return true;
}

void FRecallOverlappingEntityEvaluator::TreeStart(FStateTreeExecutionContext& Context) const
{
}

void FRecallOverlappingEntityEvaluator::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	const FRecallPhysicsSensorFragment& SensorFragment = Context.GetExternalData(SensorFragmentHandle);

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);	
	InstanceData.bFound = SensorFragment.IsSensorOverlapping(SensorName);
	InstanceData.Entities = SensorFragment.GetOverlappingEntities(SensorName);
}
