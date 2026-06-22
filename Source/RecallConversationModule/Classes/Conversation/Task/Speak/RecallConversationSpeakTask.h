// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Conversation/Task/RecallConversationTask.h"
#include "GameplayTagContainer.h"

#include "RecallConversationSpeakTask.generated.h"

/**
 * Most basic speak conversation node.
 */
UCLASS(DisplayName="Speak Task")
class RECALLCONVERSATIONMODULE_API URecallConversationSpeakTaskNode : public URecallConversationTaskNode
{
	GENERATED_BODY()

protected:
	virtual FConversationTaskResult ExecuteTaskNode_Implementation(const FConversationContext& Context) const override;
	
protected:
	UPROPERTY(EditAnywhere, Category=Conversation)
	FGameplayTag SpeakerID;
	
	UPROPERTY(EditAnywhere, Category=Conversation, meta=(MultiLine))
	FText Text;
};
