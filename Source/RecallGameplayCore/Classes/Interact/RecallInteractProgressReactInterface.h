// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "UObject/Interface.h"
#include "Data/Interact/RecallInteractTypes.h"

#include "RecallInteractProgressReactInterface.generated.h"

UINTERFACE()
class RECALLGAMEPLAYCORE_API URecallInteractProgressReactInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 * Interface to react to the progress of interactions.
 */
class RECALLGAMEPLAYCORE_API IRecallInteractProgressReactInterface
{
	GENERATED_IINTERFACE_BODY()
	
public:
	virtual void SetInteractions(const TArray<FRecallInteractProgress>& Interactions) {}

};
