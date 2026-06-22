// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "RecallAttributeRepresentationTypes.generated.h"

USTRUCT(BlueprintType)
struct RECALLGAMEPLAYCORE_API FRecallAttributeRepresentation
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float CurrentValue = 0.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float MinValue = 0.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float MaxValue = 0.0f;
	
	FORCEINLINE float GetPercent() const
	{
		if (MaxValue > 0.0f && MaxValue > MinValue)
		{
			return (CurrentValue - MinValue) / (MaxValue - MinValue);
		}
		return 0.0f;
	}
	
	FORCEINLINE bool IsEmpty() const
	{
		return CurrentValue == MinValue;
	}
	
	FORCEINLINE bool IsFull() const
	{
		return CurrentValue >= MaxValue;
	}
	
	bool operator==(const FRecallAttributeRepresentation& Other) const
	{
		return CurrentValue == Other.CurrentValue
			&& MinValue == Other.MinValue
			&& MaxValue == Other.MaxValue;
	}
};

USTRUCT(BlueprintType)
struct RECALLGAMEPLAYCORE_API FRecallAttributeTrackerRepresentation
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<FGameplayTag, FRecallAttributeRepresentation> Attributes;
};
