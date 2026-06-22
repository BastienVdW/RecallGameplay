// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "RecallStateTreeTokenHandle.h"

#include "RecallStateTreeTokenRegistry.generated.h"

USTRUCT()
struct FRecallStateTreeTokenRegistry
{
	GENERATED_BODY()

public:
	FORCEINLINE FRecallStateTreeTokenHandle RequestToken(const FMassExtendedEntityHandle& OwnerEntity)
	{
		const FRecallStateTreeTokenHandle NewHandle = FRecallStateTreeTokenHandle::Make(OwnerEntity, ++TokenIdGenerator);
		Tokens.Add(NewHandle);
		return NewHandle;
	}

	FORCEINLINE void ReleaseToken(FRecallStateTreeTokenHandle& Handle)
	{
		Tokens.Remove(Handle);
		Handle.Reset();
	}
	
	FORCEINLINE int32 GetTokenCount() const	
	{
		return Tokens.Num();
	}

protected:
	UPROPERTY(VisibleAnywhere)
	TSet<FRecallStateTreeTokenHandle> Tokens;
	
	UPROPERTY(VisibleAnywhere)
	uint32 TokenIdGenerator = MS_STATE_TREE_TOKEN_INVALID_ID;
};
