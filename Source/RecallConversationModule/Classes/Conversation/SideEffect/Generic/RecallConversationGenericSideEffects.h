// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Conversation/SideEffect/RecallConversationSideEffect.h"
#include "GameplayTagContainer.h"

#include "RecallConversationGenericSideEffects.generated.h"

/**
 * Send a conversation trigger event.
 */
UCLASS(DisplayName="Conversation Trigger")
class RECALLCONVERSATIONMODULE_API URecallConversationTriggerSideEffect : public URecallConversationSideEffectNode
{
	GENERATED_BODY()

protected:
	virtual void CauseSideEffect_Implementation(const FConversationContext& Context) const override;
	
protected:
	/**
	 * Conversation triggers to push.
	 */
	UPROPERTY(EditAnywhere, meta=(GameplayTagFilter="Conversation.Trigger"))
	FGameplayTagContainer ConversationTriggers;
};
