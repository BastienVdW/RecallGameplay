// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "Attribute/RecallAttributeSetTypes.h"

#include "RecallAttributeModMagnitudeCalculation.generated.h"

UCLASS()
class URecallAttributeModMagnitudeCalculationContext : public UObject
{
	GENERATED_BODY()

public:
	void InitAttributeCalculationContext(const FRecallAttributeSet& InAttributeSet, const FGameplayTag& InAttribute);

public:
	UFUNCTION(BlueprintPure, Category=Attribute)
	float GetAttributeValue(UPARAM(meta=(GameplayTagFilter="Attribute")) FGameplayTag InAttribute) const;

private:
	UPROPERTY(Transient)
	FGameplayTag Attribute;
	
	UPROPERTY(Transient)
	FRecallAttributeSet AttributeSet;
};

UCLASS(Abstract, Blueprintable)
class RECALLATTRIBUTEMODULE_API URecallAttributeModMagnitudeCalculation : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent)
	float CalculateValue(float DeltaTime, const URecallAttributeModMagnitudeCalculationContext* Context, float BaseValue) const;
};
