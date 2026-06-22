// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "ConversationRequirementNode.h"

#include "RecallConversationRequirement.generated.h"

/**
 * Base class for simulation friendly conversation requirement nodes.
 */
UCLASS(Abstract)
class RECALLCONVERSATIONMODULE_API URecallConversationRequirementNode : public UConversationRequirementNode
{
	GENERATED_BODY()

protected:
	virtual EConversationRequirementResult IsRequirementSatisfied_Implementation(const FConversationContext& Context) const override;

protected:
	UPROPERTY(EditAnywhere, Category=Conversation)
	EConversationRequirementResult RequirementSatisfiedResult = EConversationRequirementResult::Passed;
	
	UPROPERTY(EditAnywhere, Category=Conversation)
	EConversationRequirementResult RequirementNotSatisfiedResult = EConversationRequirementResult::FailedAndHidden;
};
