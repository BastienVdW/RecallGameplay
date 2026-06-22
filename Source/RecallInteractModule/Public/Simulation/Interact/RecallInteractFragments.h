// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Mass/EntityElementTypes.h"
#include "Mass/EntityHandle.h"
#include "MassEntityTypes.h"
#include "Data/Interact/RecallInteractTypes.h"

#include "RecallInteractFragments.generated.h"

#define RECALL_INTERACT_OVERLAP_UPDATE_RATE 0.1f

/**
 * Represents a single interaction position on an interactable entity.
 * Multiple positions enable cooperative multi-player interactions at different physical locations.
 */
USTRUCT()
struct RECALLINTERACTMODULE_API FRecallInteractionPosition
{
	GENERATED_BODY()

	/** Relative/local location where interaction can occur (relative to entity transform) */
	UPROPERTY()
	FVector RelativeLocation = FVector::ZeroVector;

	/**
	 * Convert relative position to world space.
	 * @param EntityTransform The transform of the interactable entity
	 * @return World-space location of this interaction position
	 */
	FVector GetWorldLocation(const FTransform& EntityTransform) const
	{
		return EntityTransform.TransformPosition(RelativeLocation);
	}

	// Future extensibility:
	// FRotator RelativeRotation;
	// FGameplayTag PositionTag;
};

// Tag to identify entities that can interact with other entities
USTRUCT() struct FRecallInteractorTag : public FMassTag { GENERATED_BODY() };

USTRUCT()
struct RECALLINTERACTMODULE_API FRecallInteractorFragment : public FMassFragment
{
	GENERATED_BODY()

	// Closest interactable entity that can be interacted with
	UPROPERTY(VisibleAnywhere)
	FMassEntityHandle ClosestInteractableEntity;

	/**
	 * Position index within closest/current interactable.
	 * - INDEX_NONE (-1): No position selected / use center position fallback
	 * - 0+: Index into InteractableFragment.InteractionPositions
	 * - Represents closest available position when idle
	 * - Represents current interaction position when actively interacting
	 */
	UPROPERTY(VisibleAnywhere)
	int32 ClosestPositionIndex = INDEX_NONE;

	// Contextual interactable entity for contextual actions
	UPROPERTY(VisibleAnywhere)
	FMassEntityHandle ContextualInteractEntity;

	// Entity that is currently being interacted with
	UPROPERTY(VisibleAnywhere)
	FMassEntityHandle CurrentInteractEntity;

	// Index of the current interaction event on the interact entity.
	UPROPERTY(VisibleAnywhere)
	int32 InteractEventIndex = INDEX_NONE;

	/**
	 * Progress of the current interaction.
	 */
	UPROPERTY(VisibleAnywhere)
	float InteractionProgress = 0.0f;

	/**
	 * How long the interaction lasted since it started.
	 */
	UPROPERTY(VisibleAnywhere)
	float InteractTimeSeconds = 0.0f;

	/**
	 * Keep track of the last frame at which an interaction was executed.
	 */
	UPROPERTY(VisibleAnywhere)
	uint32 LastInteractionFrame = 0;

	/**
	 * Whether the current interaction has been completed.
	 */
	UPROPERTY(VisibleAnywhere)
	bool bInteractionComplete = false;
};

USTRUCT()
struct RECALLINTERACTMODULE_API FRecallInteractorSharedFragment : public FMassConstSharedFragment
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere)
	FName SensorName = NAME_None;
};

USTRUCT()
struct RECALLINTERACTMODULE_API FRecallInteractionEventData
{
	GENERATED_BODY()
	
	/**
	 * How many times this event was executed.
	 */
	UPROPERTY(VisibleAnywhere)
	int32 InteractionCount = 0;

	/**
	 * Persistent progress of this event, only set if progress is persistent.
	 */
	UPROPERTY(VisibleAnywhere)
	float PersistentProgressTimeSeconds = 0.0f;

	/**
	 * Persistent progress tick count.
	 */
	UPROPERTY(VisibleAnywhere)
	int32 PersistentProgressTickCount = 0;

	/**
	 * Last time this interaction event was executed (for cooldown tracking).
	 */
	UPROPERTY(VisibleAnywhere)
	double LastExecutionTimeSeconds = 0.0;
};

class URecallInteractAsset;

USTRUCT()
struct RECALLINTERACTMODULE_API FRecallInteractableConstSharedFragment : public FMassConstSharedFragment
{
	GENERATED_BODY()

private:
	/**
	* Parameters about our interactable entity that define the interaction.
	*/
	UPROPERTY(VisibleAnywhere)
	FRecallInteractionParameters DefaultInteraction;

	/**
	 * Keep track of the interact asset for reference.
	 */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<const URecallInteractAsset> InteractAsset;

public:
	/**
	 * Set the default interaction parameters.
	 */
	void SetDefaultInteraction(const FRecallInteractionParameters& InDefaultInteraction)
	{
		DefaultInteraction = InDefaultInteraction;
	}

	/**
	 * Set the interact asset reference.
	 */
	void SetInteractAsset(const URecallInteractAsset* InInteractAsset)
	{
		InteractAsset = InInteractAsset;
	}

	/**
	 * Get the default interaction parameters (for internal use).
	 */
	const FRecallInteractionParameters& GetDefaultInteraction() const
	{
		return DefaultInteraction;
	}

	/**
	 * Get the interact asset reference (for internal use).
	 */
	const URecallInteractAsset* GetInteractAsset() const
	{
		return InteractAsset.Get();
	}
};

USTRUCT()
struct RECALLINTERACTMODULE_API FRecallInteractableFragment : public FMassFragment
{
	GENERATED_BODY()

	/**
	* A way to override the entity interaction with another one.
	*/
	UPROPERTY(VisibleAnywhere, meta=(BaseStruct="/Script/RecallGameplayCore.RecallInteractionParameters", ExcludeBaseStruct))
	FInstancedStruct OverrideInteraction;

	UPROPERTY(VisibleAnywhere)
	mutable TArray<FRecallInteractionEventData> InteractionEventData;
		
	// Entities that are currently interacting with this interactable
	UPROPERTY(VisibleAnywhere)
	TArray<FMassEntityHandle> InProgressInstigators;
	// Entities that finished interacting with this interactable
	UPROPERTY(VisibleAnywhere)
	TArray<FMassEntityHandle> ExecuteInstigators;

	// Can be interacted with
	UPROPERTY(VisibleAnywhere)
	bool bAllowInteraction = true;

	/**
	 * Multiple interaction positions detected from component tags.
	 * - Empty array = single-position interact (backward compatible)
	 * - Non-empty = multi-position interact enabled
	 */
	UPROPERTY(VisibleAnywhere)
	TArray<FRecallInteractionPosition> InteractionPositions;

public:
	/** Check if this interactable supports multiple positions */
	bool HasMultiplePositions() const { return InteractionPositions.Num() > 1; }

	/** Validate position index is within bounds */
	bool IsValidPositionIndex(int32 Index) const { return InteractionPositions.IsValidIndex(Index); }

public:
	/**
	 * Get total event count from entire asset hierarchy (including OverrideInteraction, InteractAsset chain, and DefaultInteraction).
	 */
	int32 GetInteractEventCount(const FRecallInteractableConstSharedFragment* ConstSharedFragmentPtr) const;

	/**
	 * Get event by flattened index across the entire hierarchy.
	 * Index 0 starts from OverrideInteraction, then InteractAsset chain, then DefaultInteraction.
	 */
	const FRecallInteractionEvent& GetInteractEventChecked(int32 FlatIndex, 
		const FRecallInteractableConstSharedFragment* ConstSharedFragmentPtr) const;

	/**
	 * Get IconSocketName from the first interaction source in hierarchy that has one.
	 */
	FName GetIconSocketName(const FRecallInteractableConstSharedFragment* ConstSharedFragmentPtr) const;

	/**
	 * @deprecated Use GetInteractEventCount and GetInteractEventChecked instead
	 */
	UE_DEPRECATED(5.6, "Use GetInteractEventCount and GetInteractEventChecked instead")
	const FRecallInteractionParameters& GetInteraction(
		const FRecallInteractableConstSharedFragment* ConstSharedFragmentPtr) const;
	
	const FRecallInteractionEventData& GetInteractionEventDataChecked(int32 EventIndex,
		const FRecallInteractableConstSharedFragment* ConstSharedFragmentPtr) const;	
	FRecallInteractionEventData& GetMutableInteractionEventDataChecked(int32 EventIndex,
		const FRecallInteractableConstSharedFragment* ConstSharedFragmentPtr);
};

template <>
struct TMassFragmentTraits<FRecallInteractableFragment> final
{ enum { AuthorAcceptsItsNotTriviallyCopyable = true }; };
