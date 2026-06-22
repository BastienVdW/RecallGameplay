// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"

#include "RecallGameplayEffectHandle.generated.h"

#define RECALL_GAMEPLAY_EFFECT_INVALID 0

USTRUCT()
struct RECALLGAMEPLAYEFFECTMODULE_API FRecallGameplayEffectHandle
{
	GENERATED_BODY()

	FRecallGameplayEffectHandle() = default;
	FRecallGameplayEffectHandle(uint32 InSerialNumber) : SerialNumber(InSerialNumber) {}
	
	UPROPERTY(VisibleAnywhere)
	uint32 SerialNumber = RECALL_GAMEPLAY_EFFECT_INVALID;

	bool operator==(const FRecallGameplayEffectHandle& Other) const
	{
		return SerialNumber == Other.SerialNumber;
	}

	bool operator!=(const FRecallGameplayEffectHandle& Other) const
	{
		return !operator==(Other);
	}

	static FRecallGameplayEffectHandle Invalid() { return FRecallGameplayEffectHandle(); }
	static FRecallGameplayEffectHandle Make(uint32 InSerialNumber) { return FRecallGameplayEffectHandle(InSerialNumber); }

	void Invalidate()
	{
		SerialNumber = RECALL_GAMEPLAY_EFFECT_INVALID;
	}

	bool IsSet() const
	{
		return SerialNumber != RECALL_GAMEPLAY_EFFECT_INVALID;
	}

	FORCEINLINE bool IsValid() const
	{
		return IsSet();
	}

	friend uint32 GetTypeHash(const FRecallGameplayEffectHandle& Handle)
	{
		return Handle.SerialNumber;
	}
};
