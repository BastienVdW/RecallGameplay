// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "RecallGameplayTagConditionTypes.generated.h"

/**
 * Matching mode for gameplay tag conditions
 */
UENUM()
enum class ERecallGameplayTagMatchingMode : uint8
{
	/** Exact tag matching - requires exact tag match */
	Exact,
	/** Hierarchical matching - parent tags match child tags (e.g., "Arrow" matches "Arrow.Left") */
	Hierarchical
};

/**
 * Condition based on gameplay tags attached to the entity.
 */
USTRUCT()
struct RECALLGAMEPLAYCORE_API FRecallGameplayTagCondition
{
	GENERATED_BODY()
	
public:	
	/**
	 * Tags required for this condition to succeed.
	 */
	UPROPERTY(EditAnywhere)
	FGameplayTagContainer RequiredTags;
	
	/**
	 * Tags that are forbidden for this condition to succeed.
	 */
	UPROPERTY(EditAnywhere)
	FGameplayTagContainer ForbiddenTags;

	/**
	 * Matching mode for required tags evaluation
	 */
	UPROPERTY(EditAnywhere)
	ERecallGameplayTagMatchingMode MatchingMode = ERecallGameplayTagMatchingMode::Exact;

	FORCEINLINE bool IsEmpty() const { return RequiredTags.IsEmpty() && ForbiddenTags.IsEmpty(); }
};
