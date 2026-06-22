// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "MassExtendedEntityElementTypes.h"
#include "RecallStateTreeTokenRegistry.h"

#include "RecallStateTreeTokenFragments.generated.h"

USTRUCT()
struct RECALLGAMEPLAY_API FRecallStateTreeTokenSharedFragment : public FMassExtendedSharedFragment
{
	GENERATED_BODY()

public:
	FORCEINLINE FRecallStateTreeTokenHandle RequestToken(const FMassExtendedEntityHandle& Entity)
	{
		FRecallStateTreeTokenRegistry& TokenRegistry = TokenRegistryMap.FindOrAdd(Entity);
		return TokenRegistry.RequestToken(Entity);
	}
	
	FORCEINLINE void ReleaseToken(FRecallStateTreeTokenHandle& Handle)
	{
		const FMassExtendedEntityHandle OwnerEntity = Handle.GetTokenOwnerEntity();
		if (FRecallStateTreeTokenRegistry* TokenRegistry = TokenRegistryMap.Find(OwnerEntity))
		{
			TokenRegistry->ReleaseToken(Handle);			
			if (TokenRegistry->GetTokenCount() == 0)
			{
				TokenRegistryMap.Remove(OwnerEntity);
			}
		}
		else
		{
			Handle.Reset();
		}
	}

	FORCEINLINE int32 GetTokenCount(const FMassExtendedEntityHandle& Entity) const
	{		
		if (const FRecallStateTreeTokenRegistry* TokenRegistry = TokenRegistryMap.Find(Entity))
		{
			return TokenRegistry->GetTokenCount();
		}
		return 0;
	}

protected:
	UPROPERTY(VisibleAnywhere)
	TMap<FMassExtendedEntityHandle, FRecallStateTreeTokenRegistry> TokenRegistryMap;
};
