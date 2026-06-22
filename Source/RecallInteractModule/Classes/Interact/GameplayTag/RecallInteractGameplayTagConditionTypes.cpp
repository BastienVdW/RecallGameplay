// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallInteractGameplayTagConditionTypes.h"

#include "GameplayTagsManager.h"
#include "MassEntityView.h"
#include "Simulation/GameplayTag/RecallGameplayTagFragments.h"
#include "Utility/GameplayTag/RecallGameplayTagUtils.h"

//----------------------------------------------------------------------//
// FRecallInteractGameplayTagCondition
//----------------------------------------------------------------------//
bool FRecallInteractGameplayTagCondition::EvaluateCondition(const FRecallInteractContext& Context, FText& OutFailedText) const
{
	// Instigator tags check
	const FMassEntityView InstigatorView(Context.GetEntityManagerChecked(), Context.InstigatorEntity);
	const FRecallGameplayTagFragment* InstigatorGameplayTagFragmentPtr = InstigatorView.GetFragmentDataPtr<FRecallGameplayTagFragment>();
	if (InstigatorGameplayTagFragmentPtr != nullptr
		&& !Recall::GameplayTag::Utils::EvaluateCondition(GameplayTagCondition, InstigatorGameplayTagFragmentPtr->GameplayTagCountMap))
	{
		OutFailedText = FailedText;
		return false;
	}

	// Interactable tags check
	const FMassEntityView InteractableView(Context.GetEntityManagerChecked(), Context.InteractableEntity);
	const FRecallGameplayTagFragment* InteractableGameplayTagFragmentPtr = InteractableView.GetFragmentDataPtr<FRecallGameplayTagFragment>();
	if (InteractableGameplayTagFragmentPtr != nullptr
		&& !Recall::GameplayTag::Utils::EvaluateCondition(OwnerGameplayTagCondition, InteractableGameplayTagFragmentPtr->GameplayTagCountMap))
	{
		OutFailedText = FailedText;
		return false;
	}
	
	return true;
}

//----------------------------------------------------------------------//
// FRecallInteractGameplayTagCountCondition
//----------------------------------------------------------------------//
bool FRecallInteractGameplayTagCountCondition::EvaluateCondition(const FRecallInteractContext& Context,
	FText& OutFailedText) const
{
	const FMassEntityHandle TargetEntity = Context.GetTargetEntity(Target);
	const FMassEntityView TargetView(Context.GetEntityManagerChecked(), TargetEntity);

	const FRecallGameplayTagFragment* TargetGameplayTagFragmentPtr = TargetView.GetFragmentDataPtr<FRecallGameplayTagFragment>();
	if (TargetGameplayTagFragmentPtr == nullptr)
	{
		return CompareValue(0);
	}

	TArray<FGameplayTag> Tags;

	if (bUseChildrenTags)
	{
		const FGameplayTagContainer TagContainer = UGameplayTagsManager::Get().RequestGameplayTagChildren(GameplayTag);
		TagContainer.GetGameplayTagArray(Tags);
	}
	else
	{
		Tags.Add(GameplayTag);
	}

	for (const FGameplayTag& Tag : Tags)
	{
		const int32 Count = TargetGameplayTagFragmentPtr->GameplayTagCountMap.GetTagCount(Tag);
		if (CompareValue(Count))
		{
			return true;
		}
	}

	return false;
}

bool FRecallInteractGameplayTagCountCondition::CompareValue(int32 Count) const
{
	switch (Comparison)
	{
	case ERecallInteractGameplayTagCountComparison::None:
	case ERecallInteractGameplayTagCountComparison::MAX:
		checkNoEntry();
		return false;

	case ERecallInteractGameplayTagCountComparison::Equal:
		return Count == Value;

	case ERecallInteractGameplayTagCountComparison::NotEqual:
		return Count != Value;

	case ERecallInteractGameplayTagCountComparison::GreaterThan:
		return Count > Value;

	case ERecallInteractGameplayTagCountComparison::GreaterThanOrEqual:
		return Count >= Value;

	case ERecallInteractGameplayTagCountComparison::LessThan:
		return Count < Value;

	case ERecallInteractGameplayTagCountComparison::LessThanOrEqual:
		return Count <= Value;

	default:
		unimplemented();
		return false;
	}
}

//----------------------------------------------------------------------//
// FRecallInteractFactionComparisonCondition
//----------------------------------------------------------------------//
bool FRecallInteractFactionComparisonCondition::EvaluateCondition(const FRecallInteractContext& Context,
	FText& OutFailedText) const
{
	const FMassEntityManager& EntityManager = Context.GetEntityManagerChecked();
	
	auto GetFactionTags = [&EntityManager](const FMassEntityHandle& Entity)
	{
		const FMassEntityView EntityView(EntityManager, Entity);
		const auto* GameplayTagFragmentPtr = EntityView.GetFragmentDataPtr<FRecallGameplayTagFragment>();

		if (GameplayTagFragmentPtr != nullptr)
		{
			const FGameplayTagContainer GameplayTags = GameplayTagFragmentPtr->GameplayTagCountMap.GetTags();
			return Recall::GameplayTag::Utils::GetFactionTags(GameplayTags);
		}
		else
		{
			return FGameplayTagContainer();
		}
	};
	
	
	const FGameplayTagContainer InstigatorFactionTags = GetFactionTags(Context.InstigatorEntity);
	const FGameplayTagContainer InteractableFactionTags = GetFactionTags(Context.InteractableEntity);

	return InstigatorFactionTags.HasAll(InteractableFactionTags) != bInvert;
}
