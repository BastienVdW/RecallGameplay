// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Mass/EntityElementTypes.h"
#include "Mass/EntityHandle.h"
#include "MassEntityTypes.h"
#include "RecallCarryTypes.h"

#include "RecallCarryFragments.generated.h"

// Tag to identify entities
USTRUCT() struct FRecallCarryableTag : public FMassTag { GENERATED_BODY() };

// Tag to indicate entity is currently being carried
USTRUCT() struct FRecallCarriedTag : public FMassTag { GENERATED_BODY() };

// Fragment
USTRUCT()
struct RECALLINTERACTMODULE_API FRecallCarryableFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	TArray<FMassEntityHandle> CarrierEntities;

	FORCEINLINE int32 GetCarrierCount() const { return CarrierEntities.Num(); }
};

template <>
struct TMassFragmentTraits<FRecallCarryableFragment> final
{ enum { AuthorAcceptsItsNotTriviallyCopyable = true }; };

USTRUCT()
struct RECALLINTERACTMODULE_API FRecallCarryableConstSharedFragment : public FMassConstSharedFragment
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
