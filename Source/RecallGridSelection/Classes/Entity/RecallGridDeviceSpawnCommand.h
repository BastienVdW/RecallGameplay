// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "System/Entity/RecallEntityAsyncSpawnTypes.h"
#include "GameplayTagContainer.h"

#include "RecallGridDeviceSpawnCommand.generated.h"

USTRUCT(DisplayName="Grid Device Spawn Command")
struct RECALLGRIDSELECTION_API FRecallGridDeviceSpawnCommand : public FRecallEntityAsyncSpawnCommand
{
	GENERATED_BODY()

public:
	/**
	 * Entity owning the spawned device.
	 */
	UPROPERTY(EditAnywhere, Category=Parameter)
	FMassExtendedEntityHandle OwnerEntity;

	/**
	 * Destroy the spawned entities if the owned is invalid.
	 */
	UPROPERTY(EditAnywhere, Category=Parameter)
	bool bDestroyIfInvalidOwner = true;

	/**
	 * Add the owner faction tags to the spawned device.
	 */
	UPROPERTY(EditAnywhere, Category=Parameter)
	bool bAddOwnerFactionTags = true;

	UPROPERTY(EditAnywhere, Category=Parameter)
	bool bAddDeviceTag = true;

	UPROPERTY()
	FGameplayTag DeviceTag;
	
	UPROPERTY()
	int32 GridCellIndex = INDEX_NONE;
	
	UPROPERTY()
	uint32 GridCellReservationNumber = 0;
	
	// FRecallEntityAsyncSpawnCommand implementation Begin
public:	
	virtual void OnSpawn(FMassExtendedEntityManager& System, const TArray<FMassExtendedEntityHandle>& Entities) const override;
	// FRecallEntityAsyncSpawnCommand implementation End
};
