// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Mass/EntityElementTypes.h"
#include "MassEntityTypes.h"
#include "Attribute/RecallAttributeSetTypes.h"

#include "RecallAttributeFragments.generated.h"

class URecallAttributeSetAsset;

/**
 * Fragment to hold entity attributes
 */
USTRUCT()
struct RECALLATTRIBUTEMODULE_API FRecallAttributeFragment : public FMassFragment
{
	GENERATED_BODY()

	/**
	 * Container for the entity attributes.
	 */
	UPROPERTY(VisibleAnywhere)
	FRecallAttributeSet AttributeSet;
};

template <>
struct TMassFragmentTraits<FRecallAttributeFragment> final
{ enum { AuthorAcceptsItsNotTriviallyCopyable = true }; };

/**
 * Fragment to hold entity attributes
 */
USTRUCT()
struct RECALLATTRIBUTEMODULE_API FRecallAttributeConstSharedFragment : public FMassConstSharedFragment
{
	GENERATED_BODY()

	/**
	 * Define the entity base attributes.
	 */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<const URecallAttributeSetAsset> AttributeSet;
};
