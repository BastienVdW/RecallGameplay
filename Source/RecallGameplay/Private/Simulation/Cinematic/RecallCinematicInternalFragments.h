// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "MassExtendedEntityTypes.h"
#include "System/Actor/RecallActorTypes.h"
#include "System/Asset/RecallASsetManagerTypes.h"

#include "RecallCinematicInternalFragments.generated.h"

USTRUCT()
struct FRecallCinematicLabelSignal
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	double TimeSeconds = 0.0;

	UPROPERTY(VisibleAnywhere)
	FName Label = NAME_None;
};

/*
* Internal data for our cinematic
*/
USTRUCT()
struct FRecallCinematicInternalFragment : public FMassExtendedFragment
{
	GENERATED_BODY()

	// Handle to load level sequence
	UPROPERTY(VisibleAnywhere)
	FRecallAssetLoadHandle CinematicAssetHandle;

	// Handle for our level sequence actor
	UPROPERTY(VisibleAnywhere)
	FRecallActorHandle CinematicActorHandle;

	UPROPERTY(VisibleAnywhere)
	double TimeSeconds = 0.0;

	UPROPERTY(VisibleAnywhere)
	double DurationSeconds = 0.0;

	UPROPERTY(VisibleAnywhere)
	int32 NumLoopLeft = 0;

	UPROPERTY(VisibleAnywhere)
	TArray<FRecallCinematicLabelSignal> LabelSignals;
};

USTRUCT()
struct FRecallCinematicInternalSharedFragment : public FMassExtendedConstSharedFragment
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, meta = (AllowedClasses = "/Script/LevelSequence.LevelSequenceActor"))
	FSoftClassPath LevelSequenceActorClass;
};

