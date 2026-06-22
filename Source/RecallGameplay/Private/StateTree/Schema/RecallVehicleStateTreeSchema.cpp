// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "StateTree/Schema/RecallVehicleStateTreeSchema.h"

#include "Simulation/Physics/RecallPhysicsBodyFragment.h"
#include "Simulation/Physics/RecallPhysicsVehicleFragments.h"
#include "Simulation/Player/Input/RecallPlayerInputFragments.h"
#include "StateTree/Player/RecallPlayerConditionBase.h"
#include "StateTree/Player/RecallPlayerEvaluatorBase.h"
#include "StateTree/RecallStateTreeEvaluatorBase.h"
#include "StateTree/RecallStateTreeTaskBase.h"

//----------------------------------------------------------------------//
// URecallVehicleStateTreeSchema
//----------------------------------------------------------------------//
URecallVehicleStateTreeSchema::URecallVehicleStateTreeSchema()
	: Super()
{
	LinkContextData(VehicleFragment,			FName("Vehicle Data"),		FGuid(TEXT("15E0D2A9-4C4C-A605-6755-02B071DFE467")));
}

bool URecallVehicleStateTreeSchema::IsStructAllowed(const UScriptStruct* InScriptStruct) const
{
	return Super::IsStructAllowed(InScriptStruct)
		|| InScriptStruct->IsChildOf(FRecallStateTreeEvaluatorBase::StaticStruct())
		|| InScriptStruct->IsChildOf(FRecallStateTreeTaskBase::StaticStruct());
}

//----------------------------------------------------------------------//
// URecallPlayerVehicleStateTreeSchema
//----------------------------------------------------------------------//
URecallPlayerVehicleStateTreeSchema::URecallPlayerVehicleStateTreeSchema()
	: Super()
{
	LinkContextData(PlayerInputFragment,		FName("Input Data"),		FGuid(TEXT("1F9A3657-4E61-F9EC-8038-BE91D36B562B")));
}

bool URecallPlayerVehicleStateTreeSchema::IsStructAllowed(const UScriptStruct* InScriptStruct) const
{
	return Super::IsStructAllowed(InScriptStruct)
		|| InScriptStruct->IsChildOf(FRecallPlayerConditionBase::StaticStruct())
		|| InScriptStruct->IsChildOf(FRecallPlayerEvaluatorBase::StaticStruct());
}
