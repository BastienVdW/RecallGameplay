// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "GameplayTagContainer.h"
#include "StructUtils/InstancedStruct.h"

#include "RecallInventoryItemCommandTypes.generated.h"

struct FMassEntityHandle;
struct FMassExecutionContext;
class URecallSignalSubsystem;

struct FRecallInventoryItemExecutionContext
{
	FMassExecutionContext& MassExecutionContext;
	const FMassEntityHandle& Entity;
	URecallSignalSubsystem& SignalSystem;
	const FRandomStream& RandomStream;
	const FGameplayTag EquipSlot;
};

USTRUCT()
struct RECALLGAMEPLAYCORE_API FRecallInventoryItemCommand
{
	GENERATED_BODY()
	
	virtual ~FRecallInventoryItemCommand() = default;
	virtual void OnEnter(const FRecallInventoryItemExecutionContext& Context) const {}
	virtual bool OnTick(const FRecallInventoryItemExecutionContext& Context) const { return false; }
	virtual void OnExit(const FRecallInventoryItemExecutionContext& Context) const {}
};
