// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

#include "RecallMovementTypes.generated.h"

RECALLGAMEPLAY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_IgnoreMovement);
RECALLGAMEPLAY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Grounded);

RECALLGAMEPLAY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_MovementSpeedModifier);

UENUM(meta=(BitFlags, UseEnumValuesAsMaskValuesInEditor="true"))
enum class ERecallMovementRotationAxis : uint8
{
	None		= 0				UMETA(Hidden),
	
	X			= 1 << 0,
	Y			= 1 << 1,

	All			= X | Y
};
ENUM_CLASS_FLAGS(ERecallMovementRotationAxis)

constexpr bool EnumHasAnyFlags(uint8 Flags, ERecallMovementRotationAxis Contains) { return (Flags & static_cast<uint8>(Contains)) != 0; }

USTRUCT()
struct RECALLGAMEPLAY_API FRecallMovementSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	bool bOnlyMoveOnGround = true;
	
	UPROPERTY(EditAnywhere, meta=(Bitmask, BitmaskEnum="/Script/RecallGameplay.ERecallMovementRotationAxis"))
	uint8 RotateAxis = static_cast<uint8>(ERecallMovementRotationAxis::All);
	
	UPROPERTY(EditAnywhere, meta=(EditCondition="RotateAxis != 0", ClampMin=0.0f))
	float RotateInterpSpeed = 10.0f;
	
	UPROPERTY(EditAnywhere, meta=(EditCondition="RotateAxis != 0"))
	bool bFaceControlRotation = true;

	UPROPERTY(EditAnywhere, meta=(ClampMin="0", UIMin="0", Units="CentimetersPerSecond"), DisplayName="Jump Z Velocity")
	float JumpZVelocityCentimetersPerSecond = 420.0f;

protected:
	UPROPERTY(EditAnywhere, meta=(Units="CentimetersPerSecondSquared", ClampMin=0.0f))
	float Acceleration = 100.0f;

	UPROPERTY(EditAnywhere, meta=(Units="CentimetersPerSecondSquared", ClampMin=0.0f))
	float Deceleration = 300.0f;

	UPROPERTY(EditAnywhere, meta=(Units="CentimetersPerSecond", ClampMin=0.0f))
	float Speed = 1000.0f;

public:
	float GetAcceleration() const;
	float GetDeceleration() const;
	float GetSpeed() const;
	float GetSpeedCentimetersPerSecond() const;
};
