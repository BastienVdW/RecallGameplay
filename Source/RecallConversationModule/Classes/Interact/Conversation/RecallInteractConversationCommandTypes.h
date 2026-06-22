// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Interact/RecallInteractCommandTypes.h"
#include "GameplayTagContainer.h"

#include "RecallInteractConversationCommandTypes.generated.h"

/**
 * Conversation this entity when the interaction is executed.
 * The entity must have the "RE Conversationable" trait.
 */
USTRUCT(DisplayName="Conversation")
struct RECALLCONVERSATIONMODULE_API FRecallConversationCommand : public FRecallInteractCommand
{
	GENERATED_BODY()

public:
	virtual bool Validate(const FRecallInteractContext& Context) const override;
	virtual void OnExecute(const FRecallInteractContext& Context) const override;

protected:
	UPROPERTY(EditAnywhere, meta=(GameplayTagFilter="Conversation.EntryPoint"))
	FGameplayTag ConversationEntryPoint;
	
	UPROPERTY(EditAnywhere)
	FString ConversationEntryPointIdentifier;
};
