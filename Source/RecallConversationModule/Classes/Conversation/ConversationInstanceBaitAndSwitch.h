// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "ConversationMemory.h"
#include "ConversationTypes.h"
#include "Math/RandomStream.h"

#include "ConversationInstanceBaitAndSwitch.generated.h"

/**
 * Bait and switch for UConversationInstance so we can access its data.
 */
UCLASS()
class RECALLCONVERSATIONMODULE_API UConversationInstanceBaitAndSwitch : public UObject
{
	GENERATED_BODY()

public:
	// Server notification sent after all participants have been individually notified of conversation start
	DECLARE_EVENT_OneParam(UConversationInstance, FOnAllParticipantsNotifiedOfStartEvent, UConversationInstance* /*ConversationInstance*/);
	FOnAllParticipantsNotifiedOfStartEvent OnAllParticipantsNotifiedOfStart;

	TArray<FClientConversationOptionEntry> CurrentUserChoices;

	UPROPERTY()
	FConversationParticipants Participants;

	UPROPERTY()
	TObjectPtr<const UConversationDatabase> ActiveConversationGraph = nullptr;

	FGameplayTag StartingEntryGameplayTag;
	FConversationBranchPoint StartingBranchPoint;

	FConversationBranchPoint CurrentBranchPoint;

	struct FCheckpoint
	{
		FConversationBranchPoint ClientBranchPoint;
		TArray<FConversationChoiceReference> ScopeStack;
	};

	TArray<FCheckpoint> ClientBranchPoints;

	TArray<FConversationBranchPoint> CurrentBranchPoints;

	TArray<FConversationChoiceReference> ScopeStack;

	FRandomStream ConversationRNG;

//@ BASTIEN ADD
// #if WITH_SERVER_CODE
//@ BASTIEN END
	FConversationMemory InstanceMemory;
//@ BASTIEN ADD
// #endif
//@ BASTIEN END

	bool bConversationStarted = false;
};
