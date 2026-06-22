// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "Mass/EntityHandle.h"

#include "RecallStateTreeTokenHandle.generated.h"

#define MS_STATE_TREE_TOKEN_INVALID_ID 0

USTRUCT()
struct FRecallStateTreeTokenHandle
{
	GENERATED_BODY()

	FRecallStateTreeTokenHandle() = default;
	
	FORCEINLINE bool IsSet() const
	{
		return OwnerEntity.IsSet()
			&& TokenID != MS_STATE_TREE_TOKEN_INVALID_ID;
	}
	
	FORCEINLINE void Reset()
	{
		OwnerEntity.Reset();
		TokenID = MS_STATE_TREE_TOKEN_INVALID_ID;
	}
	
	static FRecallStateTreeTokenHandle Make(const FMassEntityHandle& OwnerEntity, int32 TokenID)
	{
		FRecallStateTreeTokenHandle NewHandle;
		NewHandle.OwnerEntity = OwnerEntity;
		NewHandle.TokenID = TokenID;
		return NewHandle;
	}
	
	bool operator!=(const FRecallStateTreeTokenHandle& Other) const { return !(*this == Other); }
	bool operator==(const FRecallStateTreeTokenHandle& Other) const
	{
		return OwnerEntity == Other.OwnerEntity
			&& TokenID == Other.TokenID;
	}

	friend uint32 GetTypeHash(const FRecallStateTreeTokenHandle& Handle)
	{
		return HashCombine(GetTypeHash(Handle.OwnerEntity), Handle.TokenID);
	}

	const FMassEntityHandle& GetTokenOwnerEntity() const
	{
		return OwnerEntity;
	}
	
protected:
	UPROPERTY(VisibleAnywhere)
	FMassEntityHandle OwnerEntity;
	
	UPROPERTY(VisibleAnywhere)
	uint32 TokenID = MS_STATE_TREE_TOKEN_INVALID_ID;
};
