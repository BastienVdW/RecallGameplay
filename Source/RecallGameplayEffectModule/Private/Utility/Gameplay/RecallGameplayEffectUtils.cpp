// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "Utility/Gameplay/RecallGameplayEffectUtils.h"

#include "Gameplay/RecallGameplayEffect.h"
#include "Gameplay/RecallGameplayEffectContainer.h"
#include "Representation/Gameplay/RecallGameplayEffectRepresentation.h"

namespace Recall::GameplayEffect::Utils
{
	
TArray<FRecallGameplayEffectRepresentation> GetGameplayEffectRepresentations(
	const FRecallGameplayEffectContainer& Container)
{
	const TArray<TSubclassOf<URecallGameplayEffect>> EffectClasses = Container.GetEffectClasses();

	TMap<const URecallGameplayEffect*, int32> EffectMap;
	EffectMap.Reserve(EffectClasses.Num());
	
	TArray<FRecallGameplayEffectRepresentation> Result;
	Result.Reserve(EffectClasses.Num());

	for (const TSubclassOf<URecallGameplayEffect>& EffectClass : EffectClasses)
	{
		const URecallGameplayEffect* EffectCDO = EffectClass.GetDefaultObject();
		if (!ensure(EffectCDO))
		{
			continue;
		}

		if (int32* EffectIndex = EffectMap.Find(EffectCDO))
		{
			Result[*EffectIndex].StackCount++;
		}
		else
		{
			EffectMap.Add(EffectCDO, Result.Num());
			
			FRecallGameplayEffectRepresentation& NewEffect = Result.AddDefaulted_GetRef();
			NewEffect.EffectName = EffectCDO->Name;
			NewEffect.EffectDescription = EffectCDO->Description;
			NewEffect.StackCount = 1;
			NewEffect.IconTexture = EffectCDO->Icon;
		}
	}
	
	return Result;
}
	
} // namespace Recall::GameplayEffect::Utils
