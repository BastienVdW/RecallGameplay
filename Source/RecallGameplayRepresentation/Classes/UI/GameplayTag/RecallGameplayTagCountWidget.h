// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "ExtendedCommonUserWidget.h"
#include "Observer/Representation/RecallRepresentationReactInterface.h"
#include "GameplayTagContainer.h"
#include "Mass/EntityHandle.h"

#include "RecallGameplayTagCountWidget.generated.h"

UENUM()
enum class ERecallGameplayTagCountOwner : uint8
{
	Controller,
	Faction,
};

/**
 * Widget to keep track of the count of a specific gameplay tap owned by the player.
 */
UCLASS(Abstract)
class RECALLGAMEPLAYREPRESENTATION_API URecallGameplayTagCountWidget :
	public UExtendedCommonUserWidget,
	public IRecallRepresentationReactInterface
{
	GENERATED_BODY()

public:
	void SetEntity(const FMassEntityHandle& InEntity);
	void SetGameplayTag(const FGameplayTag& Tag);

	// IRecallRepresentationReactInterface implementation Begin
public:
	virtual void OnRender() override;
	// IRecallRepresentationReactInterface implementation End
	
	// UUserWidget implementation Begin
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	// UUserWidget implementation End

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnGameplayEffectTagCountChanged(const FGameplayTag& Tag, int32 Count);

protected:
	/**
	 * Define the owner of the gameplay tag to display.
	 * Controller: Can be a player or an AI
	 * Faction: Inventory of a whole faction
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Gameplay Tag Count")
	ERecallGameplayTagCountOwner Owner = ERecallGameplayTagCountOwner::Controller;

	/**
	 * Gameplay tags used to filter the entity owning the gameplay tag of this widget.
	 * Default to the owner of this widget is empty.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gameplay Tag Count", meta=(GameplayTagFilter="Controller", EditCondition="Owner == ERecallGameplayTagCountOwner::Controller", EditConditionHides))
	FGameplayTag ControllerGameplayTag;

	/**
	 * Use the faction assigned to the controller.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Gameplay Tag Count", meta=(EditCondition="Owner == ERecallGameplayTagCountOwner::Faction", EditConditionHides))
	bool bUseControllerFaction = true;

	/**
	 * Display the gameplay tag owned by this faction.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gameplay Tag Count", meta=(GameplayTagFilter="Faction", EditCondition="Owner == ERecallGameplayTagCountOwner::Faction && !bUseControllerFaction", EditConditionHides))
	FGameplayTagContainer FactionGameplayTags;
	
	/**
	 * Gameplay tag to display the count for.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Gameplay Tag Count")
	FGameplayTag GameplayTag;

	/**
	 * Count for this gameplay tag.
	 */
	UPROPERTY(Transient, BlueprintReadOnly)
	int32 GameplayTagCount = 0;

	/**
	 * Entity which own the gameplay tag.
	 */
	UPROPERTY(Transient, BlueprintReadOnly, meta=(ExposeOnSpawn))
	FMassEntityHandle Entity;

private:
	void SetGameplayTagCountFromController();
	void SetGameplayTagCountFromFaction();
	void SetGameplayTagCount(int32 Count);
};
