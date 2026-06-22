// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "MassExtendedEntityTypes.h"

#include "RecallNavLinkFragments.generated.h"

USTRUCT()
struct RECALLNAVIGATION_API FRecallNavLinkTraversalFragment : public FMassExtendedFragment
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere)
	uint32 TraversalDuration = 0;
	
	UPROPERTY(VisibleAnywhere)
	uint64 CustomNavLinkId = 0;
	
	UPROPERTY(VisibleAnywhere)
	FVector StartLocation = FVector::ZeroVector;
	
	UPROPERTY(VisibleAnywhere)
	FVector EndLocation = FVector::ZeroVector;
};
