// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "System/Interface/RecallSimulationReactSystemInterface.h"
#include "MassExtendedExternalSubsystemTraits.h"
#include "RecallInventoryTypes.h"

#include "RecallInventorySubsystem.generated.h"

UCLASS()
class RECALLINVENTORYMODULE_API URecallInventorySubsystem :
	public UWorldSubsystem,
	public IRecallSimulationReactSystemInterface
{
	GENERATED_BODY()

public:
	FRecallGameplayTagCountMap& GetMutableInventory(const FGameplayTag& OwnerTag);
	const FRecallGameplayTagCountMap& GetInventory(const FGameplayTag& OwnerTag) const;

protected:
	// UWorldSubsystem implementation Begin
	void Initialize(FSubsystemCollectionBase& Collection) override final;
	void Deinitialize() override final;
	// UWorldSubsystem implementation End

	// IRecallSimulationReactSystemInterface implementation Begin
	void Reset() override final;
	void Save(const FRecallSnapshotContext& Context, FInstancedStruct& OutSnapshot) override final;
	void Restore(const FRecallSnapshotContext& Context, const FInstancedStruct& InSnapshot) override final;
	// IRecallSimulationReactSystemInterface implementation End

protected:
	UPROPERTY(VisibleAnywhere)
	FRecallInventoryRegistry InventoryRegistry;
};

template<>
struct TMassExtendedExternalSubsystemTraits<URecallInventorySubsystem> final
{
	enum
	{
		GameThreadOnly = false
	};
};
