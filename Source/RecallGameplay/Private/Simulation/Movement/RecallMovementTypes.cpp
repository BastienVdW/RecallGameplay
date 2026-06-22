// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "Simulation/Movement/RecallMovementTypes.h"

#include "Utility/Math/RecallMathUtils.h"

UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_IgnoreMovement, 				"State.IgnoreMovement",					"Skip Movement Processor");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Grounded, 						"State.Grounded",						"Tag applied by the Movement Processor when grounded");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_MovementSpeedModifier,		"Attribute.MovementSpeedModifier",		"Modify movement speed");

float FRecallMovementSettings::GetAcceleration() const
{
	return Recall::Math::Utils::UnitsPerSecondToPerFrame(Acceleration);
}

float FRecallMovementSettings::GetDeceleration() const
{
	return Recall::Math::Utils::UnitsPerSecondToPerFrame(Deceleration);
}

float FRecallMovementSettings::GetSpeed() const
{
	return Recall::Math::Utils::UnitsPerSecondToPerFrame(Speed);
}

float FRecallMovementSettings::GetSpeedCentimetersPerSecond() const
{
	return Speed;
}
