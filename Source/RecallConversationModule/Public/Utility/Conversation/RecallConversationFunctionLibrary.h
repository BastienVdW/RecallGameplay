// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "RecallConversationFunctionLibrary.generated.h"

struct FConversationChoiceReference;

UCLASS()
class RECALLCONVERSATIONMODULE_API URecallConversationFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category="Conversation", meta=(WorldContext="WorldContextObject", UnsafeDuringActorConstruction="true"))
	static void SetInputConversationCommand(const UObject* WorldContextObject, FConversationChoiceReference ChoiceReference, int32 PlayerIndex = 0);
	
};
