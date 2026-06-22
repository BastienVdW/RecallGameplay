// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Animation/AnimInstance.h"

#include "RecallAnimInstance.generated.h"

UCLASS(Blueprintable, BlueprintType, Within=SkeletalMeshComponent, MinimalAPI)
class URecallAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bGrounded = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2f MovementDirection = FVector2f(0.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="Velocity")
	FVector PhysicsVelocity = FVector::ZeroVector;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator ControlRotation = FRotator::ZeroRotator;
};
