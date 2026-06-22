// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "RecallCollectTypes.generated.h"

USTRUCT()
struct RECALLINVENTORYMODULE_API FRecallCollectableSettings
{
	GENERATED_BODY()

	FRecallCollectableSettings();

	/**
	 * Collect by the faction of the collector entity.
	 */
	UPROPERTY(EditAnywhere)
	bool bCollectByFaction = false;

	/**
	 * Automatically collect the item when in range.
	 */
	UPROPERTY(EditAnywhere)
	bool bAutoCollect = false;
	
	/**
	 * Name of the sensor used by the auto-collect.
	 */
	UPROPERTY(EditAnywhere, meta=(GetOptions="GetSensorNames", EditCondition="bAutoCollect"))
	FName AutoCollectSensorName = NAME_None;
	
	/**
	 * Destroy the collectable when collected.
	 */
	UPROPERTY(EditAnywhere)
	bool bDestroyOnCollect = true;

	/**
	 * Display a UI popup on collect.
	 */
	UPROPERTY(EditAnywhere)
	bool bDisplayPopup = true;

	/**
	 * Text to display on the popup. Some arguments can be used.
	 * {IconName}: Name of the icon for the item.
	 * {ItemCount}: Number of items collected.
	 */
	UPROPERTY(EditAnywhere, meta=(EditCondition="bDisplayPopup"))
	 FText PopupText;
};
