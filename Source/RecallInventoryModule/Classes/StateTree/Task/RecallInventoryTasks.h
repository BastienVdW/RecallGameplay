// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "StateTree/RecallStateTreeTaskBase.h"

#include "RecallInventoryTasks.generated.h"

struct FRecallInventoryItemCommand;

UENUM()
enum class ERecallUseEquipItemSource
{
	Equipment,
	Inventory,
};

USTRUCT()
struct RECALLINVENTORYMODULE_API FRecallUseEquipItemTaskInstanceData
{
	GENERATED_BODY()

	/**
	 * Where does the item to use come from.
	 */
	UPROPERTY(EditAnywhere, Category=Parameter)
	ERecallUseEquipItemSource Source = ERecallUseEquipItemSource::Equipment;
	
	/**
	 * Equip slot of the item to use.
	 */
	UPROPERTY(EditAnywhere, Category=Parameter, meta=(GameplayTagFilter="EquipSlot", EditCondition="Source == ERecallUseEquipItemSource::Equipment", EditConditionHides))
	FGameplayTag EquipSlot;

	/**
	 * Tag of the inventory item to use.
	 */
	UPROPERTY(EditAnywhere, Category=Parameter, meta=(GameplayTagFilter="Inventory", EditCondition="Source == ERecallUseEquipItemSource::Inventory", EditConditionHides))
	FGameplayTag ItemTag;
};

/**
* Try to use an equipped item.
*/
USTRUCT(meta=(DisplayName="Use Equip Item"))
struct RECALLINVENTORYMODULE_API FRecallUseInventoryItemTask : public FRecallStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallUseEquipItemTaskInstanceData;

protected:
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

private:
	const FRecallInventoryItemCommand* GetItemCommand(
		const FStateTreeExecutionContext& Context, FGameplayTag& OutEquipSlot) const;
protected:
	TStateTreeExternalDataHandle<struct FRecallEquipmentFragment> EquipmentFragmentHandle;
};

USTRUCT()
struct RECALLINVENTORYMODULE_API FRecallDropEquipItemTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category=Parameter)
	FVector Location = FVector::ZeroVector;
	
	UPROPERTY(EditAnywhere, Category=Parameter)
	FVector Direction = FVector::ForwardVector;
	
	/**
	 * Equip slot of the item to drop.
	 */
	UPROPERTY(EditAnywhere, Category=Parameter, meta=(GameplayTagFilter="EquipSlot"))
	FGameplayTag EquipSlot;
};

/**
* Drop an equipped item.
*/
USTRUCT(meta=(DisplayName="Drop Equip Item"))
struct RECALLINVENTORYMODULE_API FRecallDropEquipItemTask : public FRecallStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallDropEquipItemTaskInstanceData;

protected:
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

	UPROPERTY(EditAnywhere, Category=Parameter)
	bool bSucceedOnDone = true;
	
	UPROPERTY(EditAnywhere, Category=Parameter, meta=(ClampMin="0.0", ClampMax="180.0", Units="Degrees"))
	float RandomRotationRange = 15.0f;
	
	UPROPERTY(EditAnywhere, Category=Parameter, meta=(Units="CentimetersPerSecond"))
	FFloatRange Speed{ 5.0f, 10.0f };
	
	UPROPERTY(EditAnywhere, Category=Parameter)
	TObjectPtr<class UMassEntityConfigAsset> DropItemEntity;
	
protected:
	TStateTreeExternalDataHandle<class URecallEntitySubsystem> EntitySystemHandle;
	TStateTreeExternalDataHandle<struct FRecallEquipmentFragment> EquipmentFragmentHandle;
	TStateTreeExternalDataHandle<struct FRecallTransformFragment> TransformFragmentHandle;
};

USTRUCT()
struct RECALLINVENTORYMODULE_API FRecallSelectEquipSlotTaskInstanceData
{
	GENERATED_BODY()

	/**
	 * Equip slot to select.
	 */
	UPROPERTY(EditAnywhere, Category=Parameter, meta=(GameplayTagFilter="EquipSlot"))
	FGameplayTag EquipSlot;

	UPROPERTY(EditAnywhere, Category=Parameter)
	FStateTreeDelegateDispatcher OnEquipSlotChanged;
};

/**
 * Select an equip slot.
 */
USTRUCT(meta=(DisplayName="Select Equip Slot"))
struct RECALLINVENTORYMODULE_API FRecallSelectEquipSlotTask : public FRecallStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallSelectEquipSlotTaskInstanceData;

protected:
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

	UPROPERTY(EditAnywhere, Category=Parameter)
	bool bFailIfEmptyEquipSlot = true;
	
protected:
	TStateTreeExternalDataHandle<struct FRecallEquipmentFragment> EquipmentFragmentHandle;
};
USTRUCT()
struct RECALLINVENTORYMODULE_API FRecallCycleEquipSlotTaskInstanceData
{
	GENERATED_BODY()
	
	/**
	 * Do not select empty equip slots.
	 */
	UPROPERTY(EditAnywhere, Category=Parameter)
	bool bSkipEmptyEquipSlot = true;
	
	/**
	 * Equip slots to cycle between.
	 */
	UPROPERTY(EditAnywhere, Category=Parameter, meta=(GameplayTagFilter="EquipSlot"))
	FGameplayTagContainer EquipSlots;

	UPROPERTY(EditAnywhere, Category=Parameter)
	FStateTreeDelegateDispatcher OnEquipSlotChanged;
};

/**
 * Cycle through a list of equipment slots.
 */
USTRUCT(meta=(DisplayName="Cycle Equip Slot"))
struct RECALLINVENTORYMODULE_API FRecallCycleEquipSlotTask : public FRecallStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallCycleEquipSlotTaskInstanceData;

protected:
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

	/**
	 * Cycle to the right, if false then cycle to the left.
	 */
	UPROPERTY(EditAnywhere, Category=Parameter)
	bool bCycleRight = true;
	
protected:
	TStateTreeExternalDataHandle<struct FRecallEquipmentFragment> EquipmentFragmentHandle;
};
