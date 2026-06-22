// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallGameRuleAsset.h"

#include "Data/Game/RecallGameRuleTypes.h"
#include "Utility/Player/RecallPlayerUtils.h"

URecallGameRuleAsset::URecallGameRuleAsset()
	: Super()
{
	PlayerStartTags.Add(TEXT("P1"));
	PlayerStartTags.Add(TEXT("P2"));
	PlayerStartTags.Add(TEXT("P3"));
	PlayerStartTags.Add(TEXT("P4"));
}

FName URecallGameRuleAsset::GetPlayerStart(const UObject* WorldContextObject, const FString& PlayerID, const FInstancedStruct& CustomParameters) const
{
	const int32 PlayerIndex = Recall::Player::Utils::GetPlayerIndexFromId(PlayerID);
	if (PlayerStartTags.Num())
	{
		return PlayerStartTags.IsValidIndex(PlayerIndex) ? PlayerStartTags[PlayerIndex] : PlayerStartTags.Last();
	}
	return NAME_None;
}

bool URecallGameRuleAsset::IsBlockInputForMatchState(const FName& MatchState) const
{
	if (MatchState == Recall::Game::State::InProgress)
	{
		return false;
	}
	if (MatchState == Recall::Game::State::WaitingToStart)
	{
		return EnumHasAnyFlags(PlayerInputRule, ERecallGameRulePlayerInput::BlockPreMatch);
	}
	if (MatchState == Recall::Game::State::WaitingPostMatch)
	{
		return EnumHasAnyFlags(PlayerInputRule, ERecallGameRulePlayerInput::BlockPostMatch);
	}
	
	return false;
}
