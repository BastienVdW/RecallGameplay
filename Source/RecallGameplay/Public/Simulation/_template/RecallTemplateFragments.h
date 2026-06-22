// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Mass/EntityElementTypes.h"

#include "RecallTemplateFragments.generated.h"

// Tag to identify entities
USTRUCT() struct FRecallTemplateTag : public FMassTag { GENERATED_BODY() };

// Fragment
USTRUCT()
struct RECALLGAMEPLAY_API FRecallTemplateFragment : public FMassFragment
{
	GENERATED_BODY()
};

// Shared fragment
USTRUCT()
struct RECALLGAMEPLAY_API FRecallTemplateSharedFragment : public FMassSharedFragment
{
	GENERATED_BODY()
};

// Const shared Fragment
USTRUCT()
struct RECALLGAMEPLAY_API FRecallTemplateConstSharedFragment : public FMassConstSharedFragment
{
	GENERATED_BODY()
};
