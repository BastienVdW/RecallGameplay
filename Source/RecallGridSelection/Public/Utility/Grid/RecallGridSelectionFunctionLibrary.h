// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "RecallGridSelectionFunctionLibrary.generated.h"

struct FRecallGridSelectionInputCommand;

UCLASS()
class RECALLGRIDSELECTION_API URecallGridSelectionFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category="Recall|Grid Selection", meta=(WorldContext="WorldContextObject", UnsafeDuringActorConstruction="true"))
	static void SetGridSelectionInputCommand(const UObject* WorldContextObject, FRecallGridSelectionInputCommand Command, int32 PlayerIndex = 0);
	
};
