// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"

#include "RecallGameplayEffectRepresentation.generated.h"

USTRUCT(BlueprintType)
struct RECALLGAMEPLAYCORE_API FRecallGameplayEffectRepresentation
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, Transient)
	FText EffectName;
	
	UPROPERTY(BlueprintReadWrite, Transient)
	FText EffectDescription;

	UPROPERTY(BlueprintReadWrite, Transient)
	int32 StackCount = 1;
	
	UPROPERTY(BlueprintReadWrite, Transient)
	TSoftObjectPtr<UTexture2D> IconTexture;
};
