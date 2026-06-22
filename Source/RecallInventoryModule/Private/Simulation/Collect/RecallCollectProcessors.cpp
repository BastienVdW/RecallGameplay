// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallCollectProcessors.h"

#include "MassExecutionContext.h"
#include "MassEntityView.h"
#include "RecallSignalSubsystem.h"
#include "Data/Inventory/RecallInventoryItemAsset.h"
#include "Simulation/Collect/RecallCollectFragments.h"
#include "Simulation/Collect/RecallCollectSignalTypes.h"
#include "Simulation/GameplayTag/RecallGameplayTagFragments.h"
#include "Simulation/Interact/RecallInteractFragments.h"
#include "Simulation/Inventory/RecallInventorySignalTypes.h"
#include "Simulation/Physics/RecallPhysicsSensorFragment.h"
#include "Simulation/Physics/RecallPhysicsSignalTypes.h"
#include "Simulation/Transform/RecallTransformFragments.h"
#include "System/Inventory/RecallInventorySubsystem.h"
#include "System/Inventory/RecallItemSubsystem.h"
#include "System/Representation/RecallRepresentationEventSubsystem.h"
#include "Utility/GameplayTag/RecallGameplayTagUtils.h"
#include "Utility/Representation/RecallRepresentationUtils.h"

//----------------------------------------------------------------------//
// URecallCollectPointSignalProcessor
//----------------------------------------------------------------------//
URecallCollectPointSignalProcessor::URecallCollectPointSignalProcessor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void URecallCollectPointSignalProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);

	SubscribeToSignal(Recall::Physics::Signals::OverlapBegin);
	SubscribeToSignal(Recall::Physics::Signals::OverlapUpdate);
}

void URecallCollectPointSignalProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallCollectPointFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FRecallPhysicsSensorFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddSubsystemRequirement<URecallInventorySubsystem>(EMassFragmentAccess::ReadWrite);
}

void URecallCollectPointSignalProcessor::SignalEntities(FMassEntityManager& EntityManager, FMassExecutionContext& Context, FRecallSignalNameLookup& EntitySignals)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_CollectPoint_Signal);

	EntityQuery.ForEachEntityChunk(Context, [&EntitySignals](FMassExecutionContext& Context)
	{
		const FMassEntityManager& EntityManager = Context.GetEntityManagerChecked();
		URecallInventorySubsystem& InventorySystem = Context.GetMutableSubsystemChecked<URecallInventorySubsystem>();

		const TConstArrayView<FRecallCollectPointFragment> CollectPointList = Context.GetFragmentView<FRecallCollectPointFragment>();
		const TConstArrayView<FRecallPhysicsSensorFragment> SensorList = Context.GetFragmentView<FRecallPhysicsSensorFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			const FRecallCollectPointFragment& CollectPointFragment = CollectPointList[EntityIndex];
			const FRecallPhysicsSensorFragment& SensorFragment = SensorList[EntityIndex];

			FRecallGameplayTagCountMap& Inventory = InventorySystem.GetMutableInventory(CollectPointFragment.OwnerInventoryTag);
			
			// Collect overlapping collectable entities
			const TArray<FMassEntityHandle> OverlappingEntities = SensorFragment.GetOverlappingEntities();
			for (const FMassEntityHandle& OverlappingEntity : OverlappingEntities)
			{
				if (!EntityManager.IsEntityValid(OverlappingEntity))
				{
					continue;
				}
				
				const FMassEntityView CollectableView(EntityManager, OverlappingEntity);
				const FRecallCollectableFragment* CollectableFragmentPtr = CollectableView.GetFragmentDataPtr<FRecallCollectableFragment>();
				if (CollectableFragmentPtr == nullptr)
				{
					continue;
				}

				const FRecallCollectableConstSharedFragment& CollectableConstSharedFragment = CollectableView.GetConstSharedFragmentData<FRecallCollectableConstSharedFragment>();
				
				// Add collected item
				for (int32 ItemIndex = 0; ItemIndex < CollectableFragmentPtr->ItemCount; ItemIndex++)
				{
					Inventory.AddTag(CollectableFragmentPtr->InventoryItem);
				}

				if (CollectableConstSharedFragment.CollectableSettings.bDestroyOnCollect)
				{
					Context.Defer().DestroyEntity(OverlappingEntity);
				}
			}
		}
	});
}

//----------------------------------------------------------------------//
// URecallCollectableSignalProcessor
//----------------------------------------------------------------------//
URecallCollectableSignalProcessor::URecallCollectableSignalProcessor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void URecallCollectableSignalProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);

	SubscribeToSignal(Recall::Collect::Signals::Collect);
	SubscribeToSignal(Recall::Physics::Signals::OverlapBegin);
	SubscribeToSignal(Recall::Physics::Signals::OverlapUpdate);
}

void URecallCollectableSignalProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallTransformFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FRecallCollectableFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FRecallInteractableFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::Optional);
	EntityQuery.AddRequirement<FRecallPhysicsSensorFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::Optional);
	EntityQuery.AddConstSharedRequirement<FRecallCollectableConstSharedFragment>();
	EntityQuery.AddSubsystemRequirement<URecallSignalSubsystem>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddSubsystemRequirement<URecallInventorySubsystem>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddSubsystemRequirement<URecallRepresentationEventSubsystem>(EMassFragmentAccess::ReadWrite);
}

static bool CollectByInstigatorEntity(
	FMassExecutionContext& Context,
	const FMassEntityHandle& InstigatorEntity,
	const FMassEntityHandle& CollectableEntity,
	const FRecallCollectableSettings& CollectableSettings,
	const FRecallCollectableFragment& CollectableFragment,
	const FRecallTransformFragment& CollectableTransformFragment)
{
	const FMassEntityManager& EntityManager = Context.GetEntityManagerChecked();	
	if (!EntityManager.IsEntityValid(InstigatorEntity))
	{
		return false;
	}
	
	const FMassEntityView InstigatorView(EntityManager, InstigatorEntity);

	// Add collected item
	if (FRecallGameplayTagFragment* GameplayTagFragmentPtr = InstigatorView.GetFragmentDataPtr<FRecallGameplayTagFragment>())
	{
		if (CollectableSettings.bCollectByFaction)
		{
			const FGameplayTagContainer FactionTags = Recall::GameplayTag::Utils::GetFactionTags(
				GameplayTagFragmentPtr->GameplayTagCountMap.GetTags());
			
			URecallInventorySubsystem& InventorySystem = Context.GetMutableSubsystemChecked<URecallInventorySubsystem>();

			TArray<FGameplayTag> FactionGameplayTagArray;
			FactionTags.GetGameplayTagArray(FactionGameplayTagArray);

			for (const FGameplayTag& FactionTag : FactionGameplayTagArray)
			{
				FRecallGameplayTagCountMap& Inventory = InventorySystem.GetMutableInventory(FactionTag);				
				Inventory.AddTag(CollectableFragment.InventoryItem, CollectableFragment.ItemCount);
			}
		}
		else
		{
			GameplayTagFragmentPtr->GameplayTagCountMap.AddTag(
				CollectableFragment.InventoryItem, CollectableFragment.ItemCount);
		}

		if (CollectableSettings.bDisplayPopup)
		{
			const URecallItemSubsystem& ItemSystem = URecallItemSubsystem::GetRef(Context.GetWorld());
			if (const TObjectPtr<const URecallInventoryItemAsset> ItemAsset = ItemSystem.GetItemAsset(
					CollectableFragment.InventoryItem))
			{
				auto& RepresentationEventSystem = Context.GetMutableSubsystemChecked<URecallRepresentationEventSubsystem>();

				FFormatNamedArguments Args;
				Args.Add("IconName", FText::FromString(ItemAsset->IconName));
				Args.Add("ItemCount", CollectableFragment.ItemCount);

				const FText PopupText = FText::Format(CollectableSettings.PopupText, Args);
				const FVector& PopupPosition = CollectableTransformFragment.Position;
				Recall::Representation::Utils::PushUIPopup(RepresentationEventSystem, PopupText, PopupPosition);
			}
		}

		URecallSignalSubsystem& SignalSystem = Context.GetMutableSubsystemChecked<URecallSignalSubsystem>();
		SignalSystem.SignalEntity(Recall::Inventory::Signals::Callback::OnLoot, InstigatorEntity);

		if (CollectableSettings.bDestroyOnCollect)
		{
			Context.Defer().DestroyEntity(CollectableEntity);
		}
	
		return true;
	}

	return false;
}

static void CollectByInteractor(
	FMassExecutionContext& Context,
	const FMassEntityHandle& CollectableEntity,
	const FRecallInteractableFragment& InteractableFragment,
	const FRecallCollectableSettings& CollectableSettings,
	const FRecallCollectableFragment& CollectableFragment,
	const FRecallTransformFragment& CollectableTransformFragment)
{	
	for (const FMassEntityHandle& InstigatorEntity : InteractableFragment.ExecuteInstigators)
	{
		if (CollectByInstigatorEntity(Context, InstigatorEntity, CollectableEntity,
			CollectableSettings, CollectableFragment, CollectableTransformFragment))
		{
			return;
		}
	}
}

static void AutoCollectByOverlappingEntity(
	FMassExecutionContext& Context,
	const FMassEntityHandle& CollectableEntity,
	const FRecallPhysicsSensorFragment& SensorFragment,
	const FRecallCollectableSettings& CollectableSettings,
	const FRecallCollectableFragment& CollectableFragment,
	const FRecallTransformFragment& CollectableTransformFragment)
{
	for (const FMassEntityHandle& InstigatorEntity : SensorFragment.GetOverlappingEntities(CollectableSettings.AutoCollectSensorName))
	{
		if (CollectByInstigatorEntity(Context, InstigatorEntity, CollectableEntity,
			CollectableSettings, CollectableFragment, CollectableTransformFragment))
		{
			return;
		}
	}
}

void URecallCollectableSignalProcessor::SignalEntities(FMassEntityManager& EntityManager, FMassExecutionContext& Context, FRecallSignalNameLookup& EntitySignals)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_Collectable_Signal);

	EntityQuery.ForEachEntityChunk(Context, [&EntitySignals](FMassExecutionContext& Context)
	{
		const FRecallCollectableConstSharedFragment& CollectableConstSharedFragment = Context.GetConstSharedFragment<FRecallCollectableConstSharedFragment>();
		const FRecallCollectableSettings& CollectableSettings = CollectableConstSharedFragment.CollectableSettings;

		const TConstArrayView<FRecallTransformFragment> TransformList = Context.GetFragmentView<FRecallTransformFragment>();
		const TConstArrayView<FRecallInteractableFragment> InteractableList = Context.GetFragmentView<FRecallInteractableFragment>();
		const TConstArrayView<FRecallCollectableFragment> CollectableList = Context.GetFragmentView<FRecallCollectableFragment>();
		const TConstArrayView<FRecallPhysicsSensorFragment> SensorList = Context.GetFragmentView<FRecallPhysicsSensorFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			const FMassEntityHandle Entity = Context.GetEntity(EntityIndex);

			const FRecallTransformFragment& TransformFragment = TransformList[EntityIndex];
			const FRecallCollectableFragment& CollectableFragment = CollectableList[EntityIndex];
			
			TArray<FName> Signals;
			EntitySignals.GetSignalsForEntity(Entity, Signals);

			if (Signals.Contains(Recall::Collect::Signals::Collect))
			{
				if (ensureMsgf(InteractableList.IsValidIndex(EntityIndex),
					TEXT("%hs To be collected, a collectable must be interactable"), __FUNCTION__))
				{					
					const FRecallInteractableFragment& InteractableFragment = InteractableList[EntityIndex];
					CollectByInteractor(Context, Entity, InteractableFragment,
						CollectableSettings, CollectableFragment, TransformFragment);
				}
			}
			else if (CollectableSettings.bAutoCollect)
			{
				if (ensureMsgf(SensorList.IsValidIndex(EntityIndex),
					TEXT("%hs Collectable with auto-collect must have a sensor"), __FUNCTION__))
				{
					const FRecallPhysicsSensorFragment& SensorFragment = SensorList[EntityIndex];
					AutoCollectByOverlappingEntity(Context, Entity, SensorFragment,
						CollectableSettings, CollectableFragment, TransformFragment);
				}
			}
		}
	});
}
