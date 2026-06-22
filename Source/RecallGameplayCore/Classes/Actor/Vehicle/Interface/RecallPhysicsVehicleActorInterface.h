// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Actor/Vehicle/RecallVehicleRepresentationTypes.h"

#include "RecallPhysicsVehicleActorInterface.generated.h"

struct FRecallVehicleRepresentationInfo;

/**
 * Interface for the actor representation of a physics vehicle.
 */
UINTERFACE()
class RECALLGAMEPLAYCORE_API URecallPhysicsVehicleActorInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class RECALLGAMEPLAYCORE_API IRecallPhysicsVehicleActorInterface
{
	GENERATED_IINTERFACE_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category=Vehicle)
	void SetDriverInput(float Forward, float Right, float Brake, float HandBrake);
	
	UFUNCTION(BlueprintNativeEvent, Category=Vehicle)
	void SetVehicleRepresentationInfo(const FRecallVehicleRepresentationInfo& Info);

};
