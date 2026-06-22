// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "AI/Navigation/NavAgentInterface.h"

class FRecallNavAgent : public INavAgentInterface
{
public:
	FRecallNavAgent(const FVector& InPosition) : Position(InPosition) {}

	//~ Begin INavAgentInterface Interface
public:
	FVector GetNavAgentLocation() const override { return Position; }
	//~ End INavAgentInterface Interface

private:
	FVector Position = FVector::ZeroVector;
};
