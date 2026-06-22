// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"

struct FMassEntityHandle;
struct FMassEntityManager;
struct FMassExecutionContext;
class URecallSignalSubsystem;

/**
 * Define the target that can be used during an interaction to evaluate a command or a condition.
 */
UENUM()
enum class ERecallInteractTarget : uint8
{
	Owner				UMETA(ToolTip="Interactable entity owning this command"),
	Instigator			UMETA(ToolTip="Entity which instigate the interaction (the instigator)"),
};

/**
 * Context for the execution of an interaction.
 */
struct RECALLINTERACTMODULE_API FRecallInteractContext
{
public:
	FMassExecutionContext& ExecutionContext;
	const FMassEntityHandle& InstigatorEntity;
	const FMassEntityHandle& InteractableEntity;
	URecallSignalSubsystem* SignalSystemPtr = nullptr;
	float DeltaTime = 1.0f;

public:
	UWorld* GetWorld() const;
	UGameInstance* GetGameInstance() const;
	FMassEntityManager& GetEntityManagerChecked() const;
	const FMassEntityHandle& GetTargetEntity(ERecallInteractTarget Target) const;
};
