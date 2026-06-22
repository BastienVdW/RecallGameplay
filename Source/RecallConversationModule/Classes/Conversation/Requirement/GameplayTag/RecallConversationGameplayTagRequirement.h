// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Conversation/Requirement/RecallConversationRequirement.h"
#include "Conversation/RecallConversationNodeTypes.h"
#include "Data/GameplayTag/RecallGameplayTagConditionTypes.h"

#include "RecallConversationGameplayTagRequirement.generated.h"

/**
 * Gameplay tag condition to use as requirement.
 */
UCLASS(DisplayName="Gameplay Tag Requirement")
class RECALLCONVERSATIONMODULE_API URecallConversationGameplayTagRequirementNode : public URecallConversationRequirementNode
{
	GENERATED_BODY()

protected:
	virtual EConversationRequirementResult IsRequirementSatisfied_Implementation(const FConversationContext& Context) const override;

protected:
	UPROPERTY(EditAnywhere, Category=Conversation)
	bool bInvert = false;
	
	UPROPERTY(EditAnywhere, Category=Conversation)
	bool bGlobalTags = false;
	
	UPROPERTY(EditAnywhere, Category=Conversation, meta=(EditCondition="!bGlobalTags"))
	ERecallConversationNodeTarget Target = ERecallConversationNodeTarget::Player;
	
	UPROPERTY(EditAnywhere, Category=Conversation)
	FRecallGameplayTagCondition GameplayTagCondition;

	bool EvaluateGameplayTagCondition(const FConversationContext& Context) const;
};
