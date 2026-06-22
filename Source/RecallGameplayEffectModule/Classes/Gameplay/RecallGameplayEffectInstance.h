// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"

#include "RecallGameplayEffectInstance.generated.h"

class URecallGameplayEffect;

USTRUCT()
struct RECALLGAMEPLAYEFFECTMODULE_API FRecallGameplayEffectInstance
{
	GENERATED_BODY()

	/**
	 * Entity owning this gameplay effect instance.
	 */
	UPROPERTY(VisibleAnywhere)
	FMassExtendedEntityHandle OwnerEntity;

	/**
	 * Class of the gameplay effect for this instance.
	 */
	UPROPERTY(VisibleAnywhere)
	TSubclassOf<URecallGameplayEffect> EffectClass;

	/**
	 * Time elapsed for this effect.
	 */
	UPROPERTY(VisibleAnywhere)
	double TimeSeconds = 0.0f;

	/**
	 * Handles to keep track of the attribute modifiers added by this effect instance so they can be removed.
	 */
	UPROPERTY(VisibleAnywhere)
	TArray<uint32> AttributeModHandles;
};
