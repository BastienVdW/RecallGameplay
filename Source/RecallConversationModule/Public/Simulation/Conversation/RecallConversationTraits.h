// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "MassExtendedEntityTraitBase.h"
#include "GameplayTagContainer.h"
#include "Conversation/RecallConversationTriggerSettings.h"

#include "RecallConversationTraits.generated.h"

/**
* Trait for an entity which can hold conversations.
*/
UCLASS(meta=(DisplayName="RE Conversation"))
class RECALLCONVERSATIONMODULE_API URecallConversationTrait : public UMassExtendedEntityTraitBase
{
	GENERATED_BODY()

public:
	virtual void BuildTemplate(FMassExtendedEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;

protected:
	/**
	 * Conversation tree used when conversing with this entity.
	 */
	UPROPERTY(EditAnywhere)
	TObjectPtr<const UConversationDatabase> Conversation = nullptr;

	/**
	 * Default entry point for the conversation tree.
	 * It can be overriden as the game progress, or reset to its default value.
	 */
	UPROPERTY(EditAnywhere, meta=(GameplayTagFilter="Conversation.EntryPoint"))
	FGameplayTag DefaultConversationEntryPoint;
	
	UPROPERTY(EditAnywhere)
	FString DefaultConversationEntryPointIdentifier;
	
	/**
	 * Settings for triggers that can start this conversation.
	 */
	UPROPERTY(EditAnywhere)
	FRecallConversationTriggerSettings TriggerSettings;
};

/**
* Trait for an entity which participate in a conversation.
*/
UCLASS(meta=(DisplayName="RE Conversation Participant"))
class RECALLCONVERSATIONMODULE_API URecallConversationParticipantTrait : public UMassExtendedEntityTraitBase
{
	GENERATED_BODY()

public:
	virtual void BuildTemplate(FMassExtendedEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;

protected:
	/**
	 * Define the ID used to reference this participant during a conversation.
	 * Only one participant per ID can join the same conversation.
	 */
	UPROPERTY(EditAnywhere, meta=(GameplayTagFilter="Conversation.Participant"))
	FGameplayTag ParticipantID;
};
