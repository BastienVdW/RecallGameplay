// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "StateTree/RecallStateTreeSchemaBase.h"

#include "RecallVehicleStateTreeSchema.generated.h"

/**
 * StateTreeSchema to control a vehicle.
 */
UCLASS(BlueprintType, EditInlineNew, CollapseCategories, meta=(DisplayName="RE Vehicle Behavior", CommonSchema))
class RECALLGAMEPLAY_API URecallVehicleStateTreeSchema : public URecallStateTreeSchemaBase
{
	GENERATED_BODY()

public:
	URecallVehicleStateTreeSchema();

protected:
	virtual bool IsStructAllowed(const UScriptStruct* InScriptStruct) const override;

private:
	TStateTreeExternalDataHandle<struct FRecallPhysicsVehicleFragment> VehicleFragment;
};

/**
 * StateTreeSchema to control a vehicle owned by a Player.
 */
UCLASS(BlueprintType, EditInlineNew, CollapseCategories, meta=(DisplayName="RE Player Vehicle Behavior", CommonSchema))
class RECALLGAMEPLAY_API URecallPlayerVehicleStateTreeSchema : public URecallVehicleStateTreeSchema
{
	GENERATED_BODY()

public:
	URecallPlayerVehicleStateTreeSchema();

protected:
	virtual bool IsStructAllowed(const UScriptStruct* InScriptStruct) const override;

private:
	TStateTreeExternalDataHandle<struct FRecallPlayerInputFragment> PlayerInputFragment;
};
