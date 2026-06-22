// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Data/Game/RecallGameRuleBaseAsset.h"

#include "RecallGameRuleAsset.generated.h"

UENUM(meta=(BitFlags, UseEnumValuesAsMaskValuesInEditor="true"))
enum class ERecallGameRulePlayerInput : uint16
{
	None				= 0,

	BlockPreMatch		= 1 << 0,
	BlockPostMatch		= 1 << 1,
};
ENUM_CLASS_FLAGS(ERecallGameRulePlayerInput)

constexpr bool EnumHasAnyFlags(uint16 Flags, ERecallGameRulePlayerInput Contains) { return (Flags & uint16(Contains)) != 0; }
inline uint16& operator|=(uint16& Lhs, ERecallGameRulePlayerInput Rhs) { return Lhs |= uint16(Rhs); }

/**
 * Asset to define generic game rules that can be reused between games.
 */
UCLASS()
class RECALLGAMEPLAYCORE_API URecallGameRuleAsset : public URecallGameRuleBaseAsset
{
	GENERATED_BODY()

public:
	URecallGameRuleAsset();

	// URecallGameRuleBaseAsset implementation Begin
public:
	virtual FName GetPlayerStart(const UObject* WorldContextObject, const FString& PlayerID, const FInstancedStruct& CustomParameters) const override;	
	virtual bool IsBlockInputForMatchState(const FName& MatchState) const override;
	// URecallGameRuleBaseAsset implementation End

public:
	/**
	 * Timer until the match starts.
	 */
	UPROPERTY(EditAnywhere, Category=Game, meta=(Units=Seconds))
	float PreMatchDuration = 0.0f;

	/**
	 * Timer until the match ends.
	 */
	UPROPERTY(EditAnywhere, Category=Game, meta=(Units=Seconds))
	float MatchDuration = 0.0f;

	UPROPERTY(EditAnywhere, Category=Game)
	FString MatchTimerRunOutReason = TEXT("Finish");
	
	/**
	 * Player input rule flags.
	 */
	UPROPERTY(EditAnywhere, Category=Player, meta=(Bitmask, BitmaskEnum="/Script/RecallGameplayCore.ERecallGameRulePlayerInput"))
	uint16 PlayerInputRule = static_cast<uint8>(ERecallGameRulePlayerInput::BlockPreMatch | ERecallGameRulePlayerInput::BlockPostMatch);
	
	/**
	 * Entity config asset to use for the game rule.
	 * Only spawned while the match is in progress.
	 */
	UPROPERTY(EditAnywhere, Category=Game)
	TSoftObjectPtr<class UMassEntityConfigAsset> GameRuleEntityConfig;
	
protected:
	/**
	 * Player start for each player by Index.
	 */
	UPROPERTY(EditAnywhere, Category=Player)
	TArray<FName> PlayerStartTags;
};
