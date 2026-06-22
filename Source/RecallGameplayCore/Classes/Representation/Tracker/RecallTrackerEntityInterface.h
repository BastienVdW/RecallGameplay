// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "RecallTrackerEntityInterface.generated.h"

struct FMassEntityHandle;

UINTERFACE()
class RECALLGAMEPLAYCORE_API URecallTrackerEntityInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 * Interface for widgets used to track entities.
 */
class RECALLGAMEPLAYCORE_API IRecallTrackerEntityInterface
{
	GENERATED_IINTERFACE_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category=Tracker)
	void SetTrackedEntity(const FMassEntityHandle& Entity);
};
