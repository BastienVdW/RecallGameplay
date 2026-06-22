// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Interact/RecallInteractConditionTypes.h"
#include "Data/GameplayTag/RecallGameplayTagConditionTypes.h"

#include "RecallInteractGameplayTagConditionTypes.generated.h"

USTRUCT(DisplayName="Gameplay Tag Requirement")
struct RECALLINTERACTMODULE_API FRecallInteractGameplayTagCondition : public FRecallInteractCondition
{
	GENERATED_BODY()
	
public:
	virtual bool EvaluateCondition(const FRecallInteractContext& Context, FText& OutFailedText) const override;

protected:
	/**
	 * Tag condition for the interactor entity (the one who instigate the interaction).
	 */
	UPROPERTY(EditAnywhere, meta=(ShowOnlyInnerProperties))
	FRecallGameplayTagCondition GameplayTagCondition;
	
	/**
	 * Tag condition for the interactable entity (which is being interacted with), and "owns" this condition.
	 */
	UPROPERTY(EditAnywhere, meta=(ShowOnlyInnerProperties))
	FRecallGameplayTagCondition OwnerGameplayTagCondition;

	/**
	 * Text displayed when this condition fails.
	 */
	UPROPERTY(EditAnywhere)
	FText FailedText;
};

UENUM()
enum class ERecallInteractGameplayTagCountComparison : uint8
{
	None UMETA(Hidden),
	GreaterThan,
	LessThan,
	GreaterThanOrEqual,
	LessThanOrEqual,
	NotEqual,
	Equal,
	MAX UMETA(Hidden)
};

USTRUCT(DisplayName="Gameplay Tag Comparison")
struct RECALLINTERACTMODULE_API FRecallInteractGameplayTagCountCondition : public FRecallInteractCondition
{
	GENERATED_BODY()
	
public:
	virtual bool EvaluateCondition(const FRecallInteractContext& Context, FText& OutFailedText) const override;

protected:
	/**
	 * Target to evaluate.
	 */
	UPROPERTY(EditAnywhere)
	ERecallInteractTarget Target = ERecallInteractTarget::Instigator;

	/**
	 * Tag condition for the interactor entity (the one who instigate the interaction).
	 */
	UPROPERTY(EditAnywhere)
	FGameplayTag GameplayTag;

	/**
	 * Will test against all the children tags as well.
	 */
	UPROPERTY(EditAnywhere)
	bool bUseChildrenTags = false;
	
	/**
	 * Comparison operator to use for the gameplay tag.
	 */
	UPROPERTY(EditAnywhere)
	ERecallInteractGameplayTagCountComparison Comparison = ERecallInteractGameplayTagCountComparison::GreaterThan;
	
	/**
	 * Value to compare against.
	 */
	UPROPERTY(EditAnywhere, meta=(ClampMin="0"))
	int32 Value = 0;
	
	/**
	 * Text displayed when this condition fails.
	 */
	UPROPERTY(EditAnywhere)
	FText FailedText;

private:
	bool CompareValue(int32 Count) const;
};

/**
 * Compare the faction of the instigator and the owner of the interaction.
 */
USTRUCT(DisplayName="Faction Comparison")
struct RECALLINTERACTMODULE_API FRecallInteractFactionComparisonCondition : public FRecallInteractCondition
{
	GENERATED_BODY()
	
public:
	virtual bool EvaluateCondition(const FRecallInteractContext& Context, FText& OutFailedText) const override;

protected:
	UPROPERTY(EditAnywhere)
	bool bInvert = false;
};
