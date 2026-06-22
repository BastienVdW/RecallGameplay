// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "MassExtendedEntityElementTypes.h"
#include "MassExtendedEntityHandle.h"
#include "RecallCarryTypes.h"

#include "RecallCarryFragments.generated.h"

// Tag to identify entities
USTRUCT() struct FRecallCarryableTag : public FMassExtendedTag { GENERATED_BODY() };

// Tag to indicate entity is currently being carried
USTRUCT() struct FRecallCarriedTag : public FMassExtendedTag { GENERATED_BODY() };

// Fragment
USTRUCT()
struct RECALLINTERACTMODULE_API FRecallCarryableFragment : public FMassExtendedFragment
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	TArray<FMassExtendedEntityHandle> CarrierEntities;

	FORCEINLINE int32 GetCarrierCount() const { return CarrierEntities.Num(); }
};

USTRUCT()
struct RECALLINTERACTMODULE_API FRecallCarryableConstSharedFragment : public FMassExtendedConstSharedFragment
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	FRecallCarryableSettings Settings;

	/**
	 * Check if the entity is considered "carried" (minimum carrier count reached).
	 * @param CarrierCount The current number of carriers
	 * @return True if carrier count >= minimum, false otherwise
	 */
	FORCEINLINE bool IsCarried(int32 CarrierCount) const
	{
		return CarrierCount >= Settings.CarrierRange.GetLowerBound().GetValue();
	}

	/**
	 * Check if more carriers can be added (maximum carrier limit not reached).
	 * @param CarrierCount The current number of carriers
	 * @return True if carrier count < maximum, false if limit is reached
	 */
	FORCEINLINE bool CanAddCarrier(int32 CarrierCount) const
	{
		return CarrierCount < Settings.CarrierRange.GetUpperBound().GetValue();
	}
};
