// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "MassExtendedEntityTypes.h"
#include "System/Actor/RecallActorTypes.h"

#include "RecallCinematicFragments.generated.h"

USTRUCT() struct RECALLGAMEPLAY_API FRecallCinematicPlayingTag : public FMassExtendedTag { GENERATED_BODY() };

UENUM(meta=(BitFlags, UseEnumValuesAsMaskValuesInEditor="true"))
enum class ERecallCinematicCallback : uint16
{
	None	= 0,
	End		= 1 << 0,
	Start	= 1 << 1,
};
ENUM_CLASS_FLAGS(ERecallCinematicCallback)

constexpr bool EnumHasAnyFlags(uint16 Flags, ERecallCinematicCallback Contains) { return (Flags & uint16(Contains)) != 0; }
inline uint16& operator|=(uint16& Lhs, ERecallCinematicCallback Rhs) { return Lhs |= uint16(Rhs); }

USTRUCT()
struct RECALLGAMEPLAY_API FRecallCinematicBinding
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	FName Name = NAME_None;

	UPROPERTY(VisibleAnywhere)
	FRecallActorHandle ActorHandle;
};

#define RECALL_CINEMATIC_ASSET_LOAD_DEFAULT_DURATION 15

/*
* Data used to trigger our cinematic
*/
USTRUCT()
struct RECALLGAMEPLAY_API FRecallCinematicFragment : public FMassExtendedFragment
{
	GENERATED_BODY()

	// Asset to play
	UPROPERTY(VisibleAnywhere, meta=(AllowedClasses="/Script/LevelSequence.LevelSequence"))
	FSoftObjectPath LevelSequenceAsset;

	UPROPERTY(VisibleAnywhere)
	TArray<FRecallCinematicBinding> Bindings;

	UPROPERTY(VisibleAnywhere)
	int32 NumLoops = -1;
	
	UPROPERTY(VisibleAnywhere, meta=(Bitmask, BitmaskEnum="/Script/RecallGameplay.ERecallCinematicCallback"))
	uint16 Callback = 0;

	// How many frames to wait before considering this asset as loaded
	UPROPERTY(VisibleAnywhere)
	int32 LoadDuration = RECALL_CINEMATIC_ASSET_LOAD_DEFAULT_DURATION;
	
	// Players for whom the cinematic should be played (play for all if empty)
	UPROPERTY(VisibleAnywhere)
	TArray<FString> FilteredPlayers;

	// TODO: Fade in/fade out

	void Reset()
	{
		LevelSequenceAsset.Reset();
		Bindings.Reset();
		NumLoops = -1;
		Callback = 0;
		LoadDuration = RECALL_CINEMATIC_ASSET_LOAD_DEFAULT_DURATION;
		FilteredPlayers.Empty();
	}
};

USTRUCT()
struct RECALLGAMEPLAY_API FRecallCinematicSharedFragment : public FMassExtendedConstSharedFragment
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	FName DefaultInstigatorBinding = TEXT("Instigator");
};
