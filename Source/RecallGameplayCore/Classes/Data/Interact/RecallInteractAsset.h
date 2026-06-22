// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Engine/DataAsset.h"
#include "RecallInteractTypes.h"

#include "RecallInteractAsset.generated.h"

/**
 * Data asset to define an interaction.
 */
UCLASS()
class RECALLGAMEPLAYCORE_API URecallInteractAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	/**
	 * Parent asset to inherit interactions from.
	 * Forms a hierarchy where this asset's events are checked first, then parent's events.
	 */
	UPROPERTY(EditAnywhere)
	TObjectPtr<const URecallInteractAsset> ParentAsset;
	
	/**
	 * Parameters that define an interaction.
	 */
	UPROPERTY(EditAnywhere, meta=(ShowOnlyInnerProperties))
	FRecallInteractionParameters Interaction;
};
