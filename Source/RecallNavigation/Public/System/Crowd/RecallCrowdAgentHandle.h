// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"

#include "RecallCrowdAgentHandle.generated.h"

#define RECALL_CROW_HANDLE_INVALID 0

USTRUCT()
struct RECALLNAVIGATION_API FRecallCrowdAgentHandle
{
	GENERATED_BODY()

	FRecallCrowdAgentHandle() = default;
	FRecallCrowdAgentHandle(uint32 InUniqueId) : UniqueId(InUniqueId) {}

	UPROPERTY(VisibleAnywhere)
	uint32 UniqueId = RECALL_CROW_HANDLE_INVALID;

	FORCEINLINE bool IsValid() const { return UniqueId != RECALL_CROW_HANDLE_INVALID; }

	FORCEINLINE void Reset()
	{
		UniqueId = RECALL_CROW_HANDLE_INVALID;
	}

	bool operator==(const FRecallCrowdAgentHandle Other) const
	{
		return UniqueId == Other.UniqueId;
	}

	bool operator!=(const FRecallCrowdAgentHandle Other) const
	{
		return !operator==(Other);
	}

	friend uint32 GetTypeHash(const FRecallCrowdAgentHandle& Handle)
	{
		return Handle.UniqueId;
	}
};
