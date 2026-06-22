// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Interact/RecallInteractCommandTypes.h"
#include "StructUtils/InstancedStruct.h"

#include "RecallInteractEntityCommandTypes.generated.h"

/**
 * Send an event to the Entity of the interactable entity.
 * The payload is made of the interactor entity.
 */
USTRUCT(DisplayName="Spawn Entity")
struct RECALLINTERACTMODULE_API FRecallInteractSpawnEntityEventCommand : public FRecallInteractCommand
{
	GENERATED_BODY()

public:
	virtual void OnExecute(const FRecallInteractContext& Context) const override;

protected:
	/**
	 * Entity config to spawn.
	*/
	UPROPERTY(EditAnywhere, meta=(AllowedClasses="/Script/MassSpawner.MassEntityConfigAsset"))
	FSoftObjectPath EntityConfigAsset;
	
	/**
	 * Attach the new entity to the target entity.
	 */
	UPROPERTY(EditAnywhere)
	bool bAttach = false;
	
	/**
	 * Target to entity to use as parent for the new entity.
	 */
	UPROPERTY(EditAnywhere, meta=(EditCondition=bAttach))
	ERecallInteractTarget ParentTarget = ERecallInteractTarget::Owner;
	
	UPROPERTY(EditAnywhere, meta=(BaseStruct="/Script/RecallSimulation.RecallEntityAsyncSpawnCommand", ExcludeBaseStruct))
	FInstancedStruct SpawnCommand;
};
