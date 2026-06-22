// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"

#include "RecallCarryTypes.generated.h"

/**
 * Set of parameters that define a carryable entity.
 */
USTRUCT()
struct RECALLINTERACTMODULE_API FRecallCarryableSettings
{
	GENERATED_BODY()

	/**
	 * Range of allowed carrier count for this carryable.
	 * Min: Minimum carrier count needed for the object to be considered "carried"
	 * Max: Maximum carrier count allowed; additional players cannot pick up once this limit is reached
	 */
	UPROPERTY(EditAnywhere)
	FInt32Range CarrierRange = FInt32Range(1, 4);

	/**
	 * The state tree will be started once this entity starts being carried.
	 */
	UPROPERTY(EditAnywhere)
	bool bRunStateTreeWhileCarried = true;

	/**
	 * If true, physics constraints will be created between carriers and the carryable entity.
	 * If false, no constraints are created, allowing for custom positioning logic.
	 */
	UPROPERTY(EditAnywhere)
	bool bUsePhysicsConstraints = true;
};
