// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "RecallGameRuleReactInterface.generated.h"

UINTERFACE()
class RECALLGAMEPLAYCORE_API URecallGameRuleReactInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 * Observer interface to react to game rule events defined by URecallGameRuleAsset.
 */
class RECALLGAMEPLAYCORE_API IRecallGameRuleReactInterface
{
	GENERATED_IINTERFACE_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent, Category=GameRule)
	void OnPreMatchTimerStart();

	UFUNCTION(BlueprintImplementableEvent, Category=GameRule)
	void OnPreMatchTimerEnd();
	
	UFUNCTION(BlueprintImplementableEvent, Category=GameRule)
	void SetPreMatchTimerRemainingTime(float TimeSeconds);

	UFUNCTION(BlueprintImplementableEvent, Category=GameRule)
	void OnMatchTimerStart();

	UFUNCTION(BlueprintImplementableEvent, Category=GameRule)
	void OnMatchTimerEnd();

	UFUNCTION(BlueprintImplementableEvent, Category=GameRule)
	void SetMatchTimerRemainingTime(float TimeSeconds);
};
