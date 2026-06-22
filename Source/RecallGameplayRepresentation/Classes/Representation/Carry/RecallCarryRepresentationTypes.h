// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"

#include "RecallCarryRepresentationTypes.generated.h"

USTRUCT(BlueprintType)
struct RECALLGAMEPLAYREPRESENTATION_API FRecallCarryableRepresentation
{
	GENERATED_BODY()

public:
	/**
	 * Range of allowed carrier count for this carryable.
	 * Min: Minimum carrier count needed for the object to be considered "carried"
	 * Max: Maximum carrier count allowed
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FInt32Range CarrierRange = FInt32Range(1, 4);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 CarrierCount = 0;

	FORCEINLINE bool IsCarried() const { return CarrierCount >= CarrierRange.GetLowerBound().GetValue(); }
};
