// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "UObject/Interface.h"

#include "RecallGridCursorActorInterface.generated.h"

struct FGameplayTagContainer;

UINTERFACE(MinimalAPI, Blueprintable)
class URecallGridCursorActorInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class RECALLGRIDSELECTION_API IRecallGridCursorActorInterface
{
	GENERATED_IINTERFACE_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category="Grid Cursor")
	void SetOwnerFaction(const FGameplayTagContainer& Factions);
};
