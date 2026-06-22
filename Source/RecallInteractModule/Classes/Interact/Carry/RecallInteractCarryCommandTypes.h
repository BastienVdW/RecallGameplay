// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Interact/RecallInteractCommandTypes.h"

#include "RecallInteractCarryCommandTypes.generated.h"

/**
 * Carry this entity when interacting.
 * The entity must have the "RE Carryable" trait.
 */
USTRUCT(DisplayName="Carry")
struct RECALLINTERACTMODULE_API FRecallInteractCarryCommand : public FRecallInteractCommand
{
	GENERATED_BODY()

public:
	virtual bool Validate(const FRecallInteractContext& Context) const override;
	virtual void OnBegin(const FRecallInteractContext& Context) const override;
	virtual void OnEnd(const FRecallInteractContext& Context) const override;
};
