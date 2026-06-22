// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Interact/RecallInteractCommandTypes.h"

#include "RecallInteractGenericCommandTypes.generated.h"

/**
 * Destroy the interactable entity.
 */
USTRUCT(DisplayName="Destroy")
struct RECALLINTERACTMODULE_API FRecallInteractDestroyCommand : public FRecallInteractCommand
{
	GENERATED_BODY()

public:
	virtual void OnExecute(const FRecallInteractContext& Context) const override;
};
