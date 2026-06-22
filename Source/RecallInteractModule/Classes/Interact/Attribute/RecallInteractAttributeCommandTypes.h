// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Interact/RecallInteractCommandTypes.h"
#include "GameplayTagContainer.h"

#include "RecallInteractAttributeCommandTypes.generated.h"

/**
 * Modify the attribute of an entity.
 * The target entity must have the "RE Attribute Set" trait.
 */
USTRUCT(DisplayName="Change Attribute")
struct RECALLINTERACTMODULE_API FRecallInteractAttributeCommand : public FRecallInteractCommand
{
	GENERATED_BODY()

public:
	virtual void OnExecute(const FRecallInteractContext& Context) const override;

protected:
	UPROPERTY(EditAnywhere, meta=(GameplayTagFilter="Attribute"))
	FGameplayTag Attribute;
	
	UPROPERTY(EditAnywhere)
	float Modifier = 1.0f;
	
	UPROPERTY(EditAnywhere)
	ERecallInteractTarget Target = ERecallInteractTarget::Instigator;
};

UENUM()
enum class ERecallInteractAttributeModAction : uint8
{
	Add,
	Remove,
};

/**
 * Command to add or remove an attribute mod.
 * The target entity must have the "RE Attribute Set" trait.
 */
USTRUCT(DisplayName="Attribute Mod")
struct RECALLINTERACTMODULE_API FRecallInteractAttributeModCommand : public FRecallInteractCommand
{
	GENERATED_BODY()

public:
	virtual void OnExecute(const FRecallInteractContext& Context) const override;

protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class URecallAttributeMod> AttributeModClass;
	
	UPROPERTY(EditAnywhere)
	ERecallInteractAttributeModAction Action = ERecallInteractAttributeModAction::Add;
	
	UPROPERTY(EditAnywhere)
	ERecallInteractTarget Target = ERecallInteractTarget::Instigator;
};
