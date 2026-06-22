// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "UObject/Interface.h"
#include "Data/Interact/RecallInteractTypes.h"

#include "RecallInteractReactInterface.generated.h"

USTRUCT(BlueprintType)
struct RECALLGAMEPLAYCORE_API FRecallInteractEventState
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UInputAction> InputAction;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FText Text;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bFailed = false;
};

UENUM(BlueprintType)
enum class ERecallInteractStateType : uint8
{
	None,
	Idle,
	InProgress,
};

/**
 * Representation of the current interaction progress.
 */
USTRUCT(BlueprintType)
struct RECALLGAMEPLAYCORE_API FRecallInteractProgressRepresentation
{
	GENERATED_BODY()
	
	/**
	 * Current state of this interaction.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ERecallInteractStateType State = ERecallInteractStateType::None;
	
	/**
	 * Whether the interaction in progress is contextual or not.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsContextual = false;
	
	/**
	 * Progress of the current interaction.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Progress = 0.0f; 

	/**
	 * Flag to hide the progress of the interaction.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bHideProgress = false;

	FORCEINLINE bool IsInProgress() const
	{
		return State == ERecallInteractStateType::InProgress;
	}

	FORCEINLINE bool IsIdle() const
	{
		return State == ERecallInteractStateType::Idle;
	}
};

/**
 * Representation of an interaction, with an interactable entity or contextual.
 */
USTRUCT(BlueprintType)
struct RECALLGAMEPLAYCORE_API FRecallInteractionRepresentation
{
	GENERATED_BODY()
	
	/**
	 * State of each event for the interaction, per input.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TMap<ERecallInteractInput, FRecallInteractEventState> EventMap;
};

USTRUCT(BlueprintType)
struct RECALLGAMEPLAYCORE_API FRecallInteractState
{
	GENERATED_BODY()

	/**
	 * Keep track of the progress of the current interaction.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FRecallInteractProgressRepresentation Progress;
	
	/**
	 * Location of the interactable.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector InteractableLocation = FVector::ZeroVector;

	/**
	 * Representation of the interaction for the nearest interactable entity.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FRecallInteractionRepresentation Interactable;

	/**
	 * Representation of contextual interactions if any.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FRecallInteractionRepresentation Contextual;
	
	FORCEINLINE bool IsValid() const
	{
		return Progress.State != ERecallInteractStateType::None;
	}
};

UINTERFACE()
class RECALLGAMEPLAYCORE_API URecallInteractReactInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 * Interface to react to interaction by local players.
 */
class RECALLGAMEPLAYCORE_API IRecallInteractReactInterface
{
	GENERATED_IINTERFACE_BODY()
	
public:
	virtual void SetInteraction(const FRecallInteractState& State, int32 PlayerIndex = INDEX_NONE) {}

};
