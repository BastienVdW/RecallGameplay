// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"

#include "RecallConversationHandle.generated.h"

#define RECALL_CONVERSATION_HANDLE_NULL 0

USTRUCT()
struct RECALLCONVERSATIONMODULE_API FRecallConversationHandle
{
	GENERATED_BODY()

	FRecallConversationHandle() = default;
	FRecallConversationHandle(uint32 InSerialNumber, int32 InIndex) : SerialNumber(InSerialNumber), Index(InIndex) {}

	UPROPERTY(VisibleAnywhere)
	uint32 SerialNumber = RECALL_CONVERSATION_HANDLE_NULL;
	
	UPROPERTY(VisibleAnywhere)
	int32 Index = INDEX_NONE;

	FORCEINLINE bool IsSet() const
	{
		return SerialNumber != RECALL_CONVERSATION_HANDLE_NULL
			&& Index != INDEX_NONE;
	}

	FORCEINLINE bool IsValid() const
	{
		return IsSet();
	}

	FORCEINLINE void Reset()
	{
		SerialNumber = RECALL_CONVERSATION_HANDLE_NULL;
		Index = INDEX_NONE;
	}
	
	static FORCEINLINE FRecallConversationHandle Invalid() { return FRecallConversationHandle(); }

	bool operator==(const FRecallConversationHandle Other) const
	{
		return SerialNumber == Other.SerialNumber
			&& Index == Other.Index;
	}

	bool operator!=(const FRecallConversationHandle Other) const
	{
		return !operator==(Other);
	}

	friend uint32 GetTypeHash(const FRecallConversationHandle& Handle)
	{
		return HashCombine(Handle.SerialNumber, Handle.Index);
	}
};
