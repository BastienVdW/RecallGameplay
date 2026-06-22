// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Interact/RecallInteractCommandTypes.h"
#include "GameplayTagContainer.h"

#include "RecallInteractStateTreeCommandTypes.generated.h"

/**
 * Send an event to the StateTree of the interactable entity.
 * The payload is made of the interactor entity.
 */
USTRUCT(DisplayName="Send State Tree Event")
struct RECALLINTERACTMODULE_API FRecallInteractSendStateTreeEventCommand : public FRecallInteractCommand
{
	GENERATED_BODY()

public:
	virtual void OnExecute(const FRecallInteractContext& Context) const override;

protected:
	/**
	 * Target who will receive the event.
	 */
	UPROPERTY(EditAnywhere)
	ERecallInteractTarget EventTarget = ERecallInteractTarget::Owner;
	
	/**
	 * Tag describing the event
	 */
	UPROPERTY(EditAnywhere, Category="Default", meta=(Categories="StateTreeEvent"))
	FGameplayTag Tag;

	/**
	 * Entity which will be passed as the payload of this event.
	 */
	UPROPERTY(EditAnywhere, Category="Default")
	ERecallInteractTarget PayloadTarget = ERecallInteractTarget::Instigator;
	
	/**
	 * Optional info to describe who sent the event.
	 */
	UPROPERTY(EditAnywhere, Category="Default")
	FName Origin = NAME_None;
};
