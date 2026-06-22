// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallInventoryTasks.h"

#include "MassCommandBuffer.h"
#include "MassEntityView.h"
#include "MassExecutionContext.h"
#include "RecallSignalSubsystem.h"
#include "Data/Inventory/RecallInventoryItemAsset.h"
#include "Data/Inventory/RecallInventoryItemCommandTypes.h"
#include "Simulation/Inventory/RecallInventoryFragments.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "Kismet/KismetMathLibrary.h"
#include "Simulation/Collect/RecallCollectFragments.h"
#include "Simulation/Inventory/RecallInventorySignalTypes.h"
#include "Simulation/Physics/RecallPhysicsBodyFragment.h"
#include "Simulation/Transform/RecallTransformFragments.h"
#include "StateTree/RecallStateTreeExecutionContext.h"
#include "System/Entity/RecallEntitySubsystem.h"
#include "System/Inventory/RecallItemSubsystem.h"
#include "Utility/Math/RecallMathUtils.h"

//----------------------------------------------------------------------//
// FRecallUseEquipItemTask
//----------------------------------------------------------------------//
bool FRecallUseInventoryItemTask::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(EquipmentFragmentHandle);
	return true;
}

const FRecallInventoryItemCommand* FRecallUseInventoryItemTask::GetItemCommand(
	const FStateTreeExecutionContext& Context, FGameplayTag& OutEquipSlot) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	
	FGameplayTag ItemTag;

	switch (InstanceData.Source)
	{
	case ERecallUseEquipItemSource::Equipment:
		{
			const FRecallEquipmentFragment& EquipmentFragment = Context.GetExternalData(EquipmentFragmentHandle);
			if (!EquipmentFragment.Equipment.GetEquipmentSlot(InstanceData.EquipSlot, ItemTag))
			{
				return nullptr;
			}
			
			OutEquipSlot = InstanceData.EquipSlot;
		}
		break;

	case ERecallUseEquipItemSource::Inventory:
		ItemTag = InstanceData.ItemTag;
		break;

	default:
		unimplemented();
		return nullptr;
	}

	const URecallItemSubsystem& ItemSystem = URecallItemSubsystem::GetRef(Context.GetWorld());
	const TObjectPtr<const URecallInventoryItemAsset> ItemAsset = ItemSystem.GetItemAsset(ItemTag);
	if (!ItemAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("%hs Item asset not found: %s"), __FUNCTION__, *ItemTag.ToString());
		return nullptr;
	}

	return ItemAsset->Command.GetPtr<FRecallInventoryItemCommand>();
}

static FRecallInventoryItemExecutionContext CreateInventoryItemExecutionContext(
	const FStateTreeExecutionContext& Context, const FGameplayTag& EquipSlot)
{
	const FRecallStateTreeExecutionContext& MassExecutionContext = static_cast<const FRecallStateTreeExecutionContext&>(Context);
	return FRecallInventoryItemExecutionContext{
		MassExecutionContext.GetMassExecutionContext(),
		MassExecutionContext.GetEntity(),
		MassExecutionContext.GetSignalSystem(),
		MassExecutionContext.GetRandomStream(),
		EquipSlot,
	};
}

EStateTreeRunStatus FRecallUseInventoryItemTask::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	FGameplayTag EquipSlot;
	const FRecallInventoryItemCommand* ItemCommandPtr = GetItemCommand(Context, EquipSlot);
	if (ItemCommandPtr == nullptr)
	{
		return EStateTreeRunStatus::Failed;
	}

	ItemCommandPtr->OnEnter(CreateInventoryItemExecutionContext(Context, InstanceData.EquipSlot));

	return Super::EnterState(Context, Transition);
}

void FRecallUseInventoryItemTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FGameplayTag EquipSlot;
	const FRecallInventoryItemCommand* ItemCommandPtr = GetItemCommand(Context, EquipSlot);
	if (ItemCommandPtr != nullptr)
	{
		ItemCommandPtr->OnExit(CreateInventoryItemExecutionContext(Context, EquipSlot));
	}

	return Super::ExitState(Context, Transition);
}

EStateTreeRunStatus FRecallUseInventoryItemTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FGameplayTag EquipSlot;
	const FRecallInventoryItemCommand* ItemCommandPtr = GetItemCommand(Context, EquipSlot);
	if (ItemCommandPtr == nullptr)
	{
		return EStateTreeRunStatus::Failed;
	}
	
	if (ItemCommandPtr->OnTick(CreateInventoryItemExecutionContext(Context, EquipSlot)))
	{
		return Super::Tick(Context, DeltaTime);
	}
	else
	{
		return EStateTreeRunStatus::Succeeded;
	}
}

//----------------------------------------------------------------------//
// FRecallDropEquipItemTask
//----------------------------------------------------------------------//
bool FRecallDropEquipItemTask::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(EntitySystemHandle);
	Linker.LinkExternalData(EquipmentFragmentHandle);
	Linker.LinkExternalData(TransformFragmentHandle);
	return true;
}

static void SpawnDropItemEntity(const FMassExecutionContext& MassExecutionContext,
	URecallEntitySubsystem& EntitySystem, const TObjectPtr<UMassEntityConfigAsset>& EntityConfigAsset,
	const FVector& DropLocation, const FVector& Velocity, const FGameplayTag& ItemTag)
{
	if (!ensureAlwaysMsgf(EntityConfigAsset, TEXT("Invalid drop item entity config")) ||
		!ensureAlwaysMsgf(ItemTag.IsValid(), TEXT("Invalid drop item tag")))
	{
		return;
	}
	
	MassExecutionContext.Defer().PushCommand<FMassDeferredCreateCommand>(
		[&EntitySystem, EntityConfigAsset, DropLocation, Velocity, ItemTag](FMassEntityManager& System)
		{
			TArray<FMassEntityHandle> Entities;		
			EntitySystem.CreateEntities(EntityConfigAsset, 1, Entities);

			const FMassEntityView EntityView(System, Entities[0]);
			if (FRecallTransformFragment* TransformFragmentPtr = EntityView.GetFragmentDataPtr<FRecallTransformFragment>())
			{
				TransformFragmentPtr->Position = DropLocation;
			}

			if (FRecallPhysicsBodyFragment* BodyFragmentPtr = EntityView.GetFragmentDataPtr<FRecallPhysicsBodyFragment>())
			{
				BodyFragmentPtr->StartVelocity = Velocity;
			}
			
			if (FRecallCollectableFragment* CollectableFragmentPtr = EntityView.GetFragmentDataPtr<FRecallCollectableFragment>())
			{
				CollectableFragmentPtr->InventoryItem = ItemTag;
			}
		}
	);
}

EStateTreeRunStatus FRecallDropEquipItemTask::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.EquipSlot.IsValid())
	{
		return EStateTreeRunStatus::Failed;
	}
	
	const FRecallStateTreeExecutionContext& MassContext = static_cast<const FRecallStateTreeExecutionContext&>(Context);
	FRecallEquipmentFragment& EquipmentFragment = Context.GetExternalData(EquipmentFragmentHandle);
	if (EquipmentFragment.Equipment.IsEmptySlot(InstanceData.EquipSlot))
	{
		return Super::EnterState(Context, Transition);
	}

	const FGameplayTag ItemTag = EquipmentFragment.Equipment.ClearEquipmentSlot(InstanceData.EquipSlot);
	
	if (DropItemEntity)
	{
		const FRecallTransformFragment& TransformFragment = Context.GetExternalData(TransformFragmentHandle);

		// Randomize a bit our vector so not all items are dropped in the same direction.
		const FRotator RandomDirection(
			MassContext.GetRandomStream().FRandRange(-RandomRotationRange, RandomRotationRange),
			MassContext.GetRandomStream().FRandRange(-RandomRotationRange, RandomRotationRange),
			MassContext.GetRandomStream().FRandRange(-RandomRotationRange, RandomRotationRange)
		);
		const FVector RandomizedDropDirection = UKismetMathLibrary::Quat_RotateVector(RandomDirection.Quaternion(), InstanceData.Direction);
		const float RandomizedSpeed = MassContext.GetRandomStream().FRandRange(Speed.GetLowerBoundValue(), Speed.GetUpperBoundValue());
		
		const FVector DropLocation = InstanceData.Location.IsZero() ? TransformFragment.Position : InstanceData.Location;
		const FVector DropVelocity = RandomizedDropDirection * Recall::Math::Utils::UnitsPerSecondToPerFrame(RandomizedSpeed);

		URecallEntitySubsystem& EntitySystem = Context.GetExternalData(EntitySystemHandle);
		SpawnDropItemEntity(MassContext.GetMassExecutionContext(), EntitySystem, DropItemEntity, DropLocation, DropVelocity, ItemTag);
	}
	
	MassContext.GetSignalSystem().SignalEntity(Recall::Inventory::Signals::Callback::OnUnequip, MassContext.GetEntity());

	if (bSucceedOnDone)
	{
		return EStateTreeRunStatus::Succeeded;
	}
	
	return Super::EnterState(Context, Transition);
}

void FRecallDropEquipItemTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	return Super::ExitState(Context, Transition);
}

EStateTreeRunStatus FRecallDropEquipItemTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	return Super::Tick(Context, DeltaTime);
}

//----------------------------------------------------------------------//
// FRecallSelectEquipSlotTask
//----------------------------------------------------------------------//
bool FRecallSelectEquipSlotTask::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(EquipmentFragmentHandle);
	return true;
}

EStateTreeRunStatus FRecallSelectEquipSlotTask::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	FRecallEquipmentFragment& EquipmentFragment = Context.GetExternalData(EquipmentFragmentHandle);

	if (bFailIfEmptyEquipSlot && EquipmentFragment.Equipment.IsEmptySlot(InstanceData.EquipSlot))
	{
		return EStateTreeRunStatus::Failed;
	}

	if (EquipmentFragment.SelectedEquipSlot != InstanceData.EquipSlot)
	{
		EquipmentFragment.SelectedEquipSlot = InstanceData.EquipSlot;
		Context.BroadcastDelegate(InstanceData.OnEquipSlotChanged);
		return Super::EnterState(Context, Transition);
	}
	else
	{		
		return EStateTreeRunStatus::Failed;
	}
}

EStateTreeRunStatus FRecallSelectEquipSlotTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	return EStateTreeRunStatus::Succeeded;
}

//----------------------------------------------------------------------//
// FRecallCycleEquipSlotTask
//----------------------------------------------------------------------//
bool FRecallCycleEquipSlotTask::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(EquipmentFragmentHandle);
	return true;
}

EStateTreeRunStatus FRecallCycleEquipSlotTask::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	
	TArray<FGameplayTag> EquipSlotTags;
	InstanceData.EquipSlots.GetGameplayTagArray(EquipSlotTags);

	if (EquipSlotTags.Num() == 0)
	{
		return EStateTreeRunStatus::Failed;
	}
	
	FRecallEquipmentFragment& EquipmentFragment = Context.GetExternalData(EquipmentFragmentHandle);
	const int32 StartEquipSlotIndex = EquipSlotTags.IndexOfByKey(EquipmentFragment.SelectedEquipSlot);
	int32 EquipSlotIndex = StartEquipSlotIndex;

	do
	{
		if (bCycleRight)
		{
			EquipSlotIndex = (EquipSlotIndex + 1) % EquipSlotTags.Num();
		}
		else
		{
			EquipSlotIndex = EquipSlotIndex > 0 ? EquipSlotIndex - 1 : EquipSlotTags.Num() - 1;
		}

		if (!InstanceData.bSkipEmptyEquipSlot || !EquipmentFragment.Equipment.IsEmptySlot(EquipSlotTags[EquipSlotIndex]))
		{
			break;
		}
	} while (EquipSlotIndex != FMath::Max(StartEquipSlotIndex, 0));

	if (EquipSlotIndex != StartEquipSlotIndex)
	{
		EquipmentFragment.SelectedEquipSlot = EquipSlotTags[EquipSlotIndex];
		Context.BroadcastDelegate(InstanceData.OnEquipSlotChanged);
		return Super::EnterState(Context, Transition);
	}
	else
	{		
		return EStateTreeRunStatus::Failed;
	}
}

EStateTreeRunStatus FRecallCycleEquipSlotTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	return EStateTreeRunStatus::Succeeded;
}
