// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Conversation/SideEffect/RecallConversationSideEffect.h"
#include "Conversation/RecallConversationNodeTypes.h"
#include "GameplayTagContainer.h"

#include "RecallConversationGameplayTagSideEffect.generated.h"

struct FMassExtendedEntityHandle;

UENUM()
enum class ERecallConversationGameplayTagOperation : uint8
{
	Add,
	Remove,
};

/**
 * Quick way to modify gameplay tags.
 * For example an item could be added or removed this way.
 */
UCLASS(DisplayName="Gameplay Tag")
class RECALLCONVERSATIONMODULE_API URecallConversationGameplayTagSideEffectNode : public URecallConversationSideEffectNode
{
	GENERATED_BODY()

protected:	
	virtual void CauseSideEffect_Implementation(const FConversationContext& Context) const override;
	
protected:
	/**
	 * Gameplay tag operation to apply to the target entities.
	 */
	UPROPERTY(EditAnywhere, Category=Conversation)
	ERecallConversationGameplayTagOperation Operation = ERecallConversationGameplayTagOperation::Add;

	/**
	 * Global gameplay tags are shared between all the entities.
	 */
	UPROPERTY(EditAnywhere, Category=Conversation)
	bool bGlobalTags = false;

	/**
	 * Target to which the gameplay tag operation should be applied.
	 */
	UPROPERTY(EditAnywhere, Category=Conversation, meta=(EditCondition="!bGlobalTags"))
	ERecallConversationNodeTarget Target = ERecallConversationNodeTarget::Player;
	
	/**
	 * Gameplay tags to use.
	 */
	UPROPERTY(EditAnywhere, Category=Conversation)
	FGameplayTagContainer GameplayTags;

	void ApplyGameplayTags(const FConversationContext& Context) const;
};
