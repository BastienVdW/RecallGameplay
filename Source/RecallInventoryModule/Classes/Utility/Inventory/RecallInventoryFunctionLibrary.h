// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"

#include "RecallInventoryFunctionLibrary.generated.h"

struct FRecallInventoryInputCommand;

UCLASS()
class RECALLINVENTORYMODULE_API URecallInventoryFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category="Inventory", meta=(WorldContext="WorldContextObject", UnsafeDuringActorConstruction="true"))
	static void SetInventoryInputCommand(const UObject* WorldContextObject, FRecallInventoryInputCommand Command, int32 PlayerIndex = 0);
	
};
