// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Interact/RecallInteractCommandTypes.h"
#include "GameplayTagContainer.h"

#include "RecallInteractGameplayEffectCommandTypes.generated.h"

UENUM()
enum class ERecallInteractGameplayEffectAction : uint8
{
	Add,
	Remove,
	WhileActive,
};

USTRUCT(DisplayName="Gameplay Effect")
struct RECALLINTERACTMODULE_API FRecallInteractGameplayEffectCommand : public FRecallInteractCommand
{
	GENERATED_BODY()

public:
	virtual void OnBegin(const FRecallInteractContext& Context) const override;
	virtual void OnEnd(const FRecallInteractContext& Context) const override;
	virtual void OnExecute(const FRecallInteractContext& Context) const override;

protected:
	/**
	 * Target entity to which the effect should be applied.
	 */
	UPROPERTY(EditAnywhere)
	ERecallInteractTarget Target = ERecallInteractTarget::Owner;

	/**
	 * Whether this effect should be added or removed.
	 */
	UPROPERTY(EditAnywhere)
	ERecallInteractGameplayEffectAction Action = ERecallInteractGameplayEffectAction::Add;

	/**
	 * Remove all the effects with this tag.
	 */
	UPROPERTY(EditAnywhere, meta=(GameplayTagFilter="Effect", EditCondition="Action == ERecallInteractGameplayEffectAction::Remove", EditConditionHides))
	FGameplayTag RemoveEffectTag;
	
	/**
	 * Effect to add or remove from the entity.
	 */
	UPROPERTY(EditAnywhere)
	TSubclassOf<class URecallGameplayEffect> GameplayEffect;
};
