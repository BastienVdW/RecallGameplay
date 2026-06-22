// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Conversation/SideEffect/RecallConversationSideEffect.h"
#include "GameplayTagContainer.h"
#include "Conversation/RecallConversationNodeTypes.h"

#include "RecallConversationStateTreeEventSideEffect.generated.h"

/**
 * Send state tree event.
 */
UCLASS(DisplayName="State Tree Event")
class RECALLCONVERSATIONMODULE_API URecallConversationStateTreeEventSideEffect : public URecallConversationSideEffectNode
{
	GENERATED_BODY()

protected:
	virtual void CauseSideEffect_Implementation(const FConversationContext& Context) const override;
	
protected:
	/**
	 * Entity used as payload for event.
	 */
	UPROPERTY(EditAnywhere, Category=Conversation)
	ERecallConversationNodeTarget Source = ERecallConversationNodeTarget::Owner;
	
	/**
	 * Target entity for the event.
	 */
	UPROPERTY(EditAnywhere, Category=Conversation)
	ERecallConversationNodeTarget Target = ERecallConversationNodeTarget::Player;
	
	/**
	 * Tag describing the event
	 */
	UPROPERTY(EditAnywhere, Category=Conversation, meta=(Categories="StateTreeEvent"))
	FGameplayTag Tag;

	/**
	 * Optional info to describe who sent the event.
	 */
	UPROPERTY(EditAnywhere, Category=Conversation)
	FName Origin = NAME_None;
};
