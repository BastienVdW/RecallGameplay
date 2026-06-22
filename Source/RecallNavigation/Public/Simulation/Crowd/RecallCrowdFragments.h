// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Mass/EntityElementTypes.h"
#include "System/Crowd/RecallCrowdAgentHandle.h"

#include "RecallCrowdFragments.generated.h"

USTRUCT()
struct RECALLNAVIGATION_API FRecallCrowdAgentFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	FRecallCrowdAgentHandle CrowdAgentHandle;
};
