// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "RecallTrackerReactInterface.generated.h"

struct FRecallTrackedEntityRepresentation;

UINTERFACE()
class RECALLGAMEPLAYCORE_API URecallTrackerReactInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 * Observer interface to react to tracked entities.
 * UI can display a widget to follow these entities more easily.
 */
class RECALLGAMEPLAYCORE_API IRecallTrackerReactInterface
{
	GENERATED_IINTERFACE_BODY()
	
public:
	virtual void SetTrackedEntities(const TArray<FRecallTrackedEntityRepresentation>& TrackedEntityRepresentations) {}
};
