// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "GameplayTagContainer.h"
#include "Interact/RecallInteractCommandTypes.h"

#include "RecallInteractGameplayTagCommandTypes.generated.h"

UENUM()
enum class ERecallInteractGameplayTagOperation : uint8
{
	Add,
	Remove,
};

/**
 * Gameplay Tag operation.
 */
USTRUCT(DisplayName="Gameplay Tag")
struct RECALLINTERACTMODULE_API FRecallInteractGameplayTagCommand : public FRecallInteractCommand
{
	GENERATED_BODY()

public:
	virtual void OnExecute(const FRecallInteractContext& Context) const override;

protected:
	/**
	 * Target entity to apply the gameplay tag operation.
	 */
	UPROPERTY(EditAnywhere)
	ERecallInteractTarget Target = ERecallInteractTarget::Owner;
	
	UPROPERTY(EditAnywhere)
	FGameplayTagContainer GameplayTags;

	UPROPERTY(EditAnywhere)
	ERecallInteractGameplayTagOperation Operation = ERecallInteractGameplayTagOperation::Add;
};
