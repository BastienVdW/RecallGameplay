// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "MassExtendedEntityTypes.h"
#include "GameplayTagContainer.h"
#include "Conversation/RecallConversationTriggerSettings.h"
#include "Conversation/RecallConversationHandle.h"

#include "RecallConversationFragments.generated.h"

/** Tag assigned to participant entities who participate in a conversation. */
USTRUCT() struct RECALLCONVERSATIONMODULE_API FRecallConversationParticipantInsideTag : public FMassExtendedTag { GENERATED_BODY() };

/** Tag assigned to conversation which are currently active. */
USTRUCT() struct RECALLCONVERSATIONMODULE_API FRecallConversationActiveTag : public FMassExtendedTag { GENERATED_BODY() };

USTRUCT()
struct RECALLCONVERSATIONMODULE_API FRecallConversationFragment : public FMassExtendedFragment
{
	GENERATED_BODY()

	/**
	 * Handle of the active conversation instance.
	 */
	UPROPERTY(VisibleAnywhere)
	FRecallConversationHandle ConversationHandle;

	/**
	 * Entry point for the conversation.
	 */
	UPROPERTY(VisibleAnywhere, meta=(GameplayTagFilter="Conversation.EntryPoint"))
	FGameplayTag ConversationEntryPoint;

	UPROPERTY(VisibleAnywhere)
	FString ConversationEntryIdentifier;
};

USTRUCT()
struct RECALLCONVERSATIONMODULE_API FRecallConversationConstSharedFragment : public FMassExtendedConstSharedFragment
{
	GENERATED_BODY()

	/**
	 * Conversation tree for this entity.
	 */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<const UConversationDatabase> Conversation;

	/**
	 * Keep track of the conversation default entry point so it can potentially be reset.
	 */
	UPROPERTY(VisibleAnywhere, meta=(GameplayTagFilter="Conversation.EntryPoint"))
	FGameplayTag DefaultConversationEntryPoint;

	UPROPERTY(VisibleAnywhere)
	FString DefaultConversationEntryPointIdentifier;
	
	/**
	 * Settings for triggers that can start this conversation.
	 */
	UPROPERTY(VisibleAnywhere)
	FRecallConversationTriggerSettings TriggerSettings;
};

USTRUCT()
struct RECALLCONVERSATIONMODULE_API FRecallConversationParticipantFragment : public FMassExtendedFragment
{
	GENERATED_BODY()

	/**
	 * Handle of the conversation this participant is currently taking part of.
	 */
	UPROPERTY(VisibleAnywhere)
	FRecallConversationHandle ConversationHandle;
};

USTRUCT()
struct RECALLCONVERSATIONMODULE_API FRecallConversationParticipantConstSharedFragment : public FMassExtendedConstSharedFragment
{
	GENERATED_BODY()

	/**
	 * ID used to reference this participant during a conversation.
	 */
	UPROPERTY(VisibleAnywhere, meta=(GameplayTagFilter="Conversation.Participant"))
	FGameplayTag ParticipantID;
};
