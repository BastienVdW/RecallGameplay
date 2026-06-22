// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "Utility/GameRule/RecallGameRuleUtils.h"

#include "Data/Game/RecallGameRuleAsset.h"
#include "Engine/World.h"
#include "System/Game/RecallGameRuleSubsystem.h"

namespace Recall::GameRule::Utils
{

float GetMatchDuration(const UObject* WorldContextObject)
{
	const UWorld* World = WorldContextObject ? WorldContextObject->GetWorld() : nullptr;
	const URecallGameRuleSubsystem* GameRuleSystem = UWorld::GetSubsystem<URecallGameRuleSubsystem>(World);
	if (IsValid(GameRuleSystem))
	{
		if (const URecallGameRuleAsset* GameRuleAsset = GameRuleSystem->GetGameRuleAsset<URecallGameRuleAsset>())
		{
			return GameRuleAsset->MatchDuration;
		}
	}
	return 0.0f;
}

float GetPreMatchDuration(const UObject* WorldContextObject)
{
	const UWorld* World = WorldContextObject ? WorldContextObject->GetWorld() : nullptr;
	const URecallGameRuleSubsystem* GameRuleSystem = UWorld::GetSubsystem<URecallGameRuleSubsystem>(World);
	if (IsValid(GameRuleSystem))
	{
		if (const URecallGameRuleAsset* GameRuleAsset = GameRuleSystem->GetGameRuleAsset<URecallGameRuleAsset>())
		{
			return GameRuleAsset->PreMatchDuration;
		}
	}
	return 0.0f;
}

} // namespace Recall::GameRule::Utils