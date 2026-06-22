// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "MassExtendedEntityTypes.h"
#include "RecallDropItemTypes.h"

#include "RecallDropItemFragments.generated.h"

USTRUCT()
struct RECALLINVENTORYMODULE_API FRecallDropItemFragment : public FMassExtendedFragment
{
	GENERATED_BODY()
};

USTRUCT()
struct RECALLINVENTORYMODULE_API FRecallDropItemConstSharedFragment : public FMassExtendedConstSharedFragment
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere)
	FRecallDropItemDefinition DropItem;
};
