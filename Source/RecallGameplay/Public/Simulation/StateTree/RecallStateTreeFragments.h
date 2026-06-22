// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Mass/EntityElementTypes.h"
#include "Mass/EntityHandle.h"
#include "MassEntityTypes.h"
#include "System/AI/RecallStateTreeInstanceTypes.h"

#include "RecallStateTreeFragments.generated.h"

class UStateTree;

// Special tag to know if the state tree has been activated
USTRUCT() struct RECALLGAMEPLAY_API FRecallStateTreeActivatedTag : public FMassTag { GENERATED_BODY() };

// Special tag to know if the state tree is running
USTRUCT() struct RECALLGAMEPLAY_API FRecallStateTreeRunningTag : public FMassTag { GENERATED_BODY() };

USTRUCT()
struct RECALLGAMEPLAY_API FRecallStateTreeSharedFragment : public FMassConstSharedFragment
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	bool bAutoStart = true;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<const UStateTree> StateTree;
};

USTRUCT()
struct RECALLGAMEPLAY_API FRecallStateTreeInstanceFragment : public FMassFragment
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere)
	FMassEntityHandle OwnerEntityHandle;

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

template <>
struct TMassFragmentTraits<FRecallStateTreeInstanceFragment> final
{ enum { AuthorAcceptsItsNotTriviallyCopyable = true }; };
