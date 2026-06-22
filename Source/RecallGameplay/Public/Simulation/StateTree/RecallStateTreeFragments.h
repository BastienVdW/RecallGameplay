// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "MassExtendedEntityElementTypes.h"
#include "MassExtendedEntityHandle.h"
#include "System/AI/RecallStateTreeInstanceTypes.h"

#include "RecallStateTreeFragments.generated.h"

class UStateTree;

// Special tag to know if the state tree has been activated
USTRUCT() struct RECALLGAMEPLAY_API FRecallStateTreeActivatedTag : public FMassExtendedTag { GENERATED_BODY() };

// Special tag to know if the state tree is running
USTRUCT() struct RECALLGAMEPLAY_API FRecallStateTreeRunningTag : public FMassExtendedTag { GENERATED_BODY() };

USTRUCT()
struct RECALLGAMEPLAY_API FRecallStateTreeSharedFragment : public FMassExtendedConstSharedFragment
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	bool bAutoStart = true;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<const UStateTree> StateTree;
};

USTRUCT()
struct RECALLGAMEPLAY_API FRecallStateTreeInstanceFragment : public FMassExtendedFragment
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere)
	FMassExtendedEntityHandle OwnerEntityHandle;

	/** Handle to a StateTree instance data in MassStateTreeSubsystem. */
	UPROPERTY(VisibleAnywhere, DisplayName="Root Instance Handle")
	FRecallStateTreeInstanceHandle InstanceHandle;

	/** The last update time use to calculate ticking delta time. */
	UPROPERTY(VisibleAnywhere)
	double LastUpdateTimeInSeconds = 0.;

	UPROPERTY(VisibleAnywhere)
	TArray<FRecallStateTreeInstanceHandle> SubInstanceHandles;

	UPROPERTY(VisibleAnywhere)
	TMap<FName, FRecallStateTreeInstanceHandle> SubInstanceHandleMap;
};
