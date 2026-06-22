// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallEquipmentProcessors.h"

#include "Actor/RecallEquipmentHolderActorInterface.h"
#include "Data/Inventory/RecallInventoryItemAsset.h"
#include "MassExtendedExecutionContext.h"
#include "RecallSignalSubsystem.h"
#include "Simulation/GameplayTag/RecallGameplayTagFragments.h"
#include "Simulation/Inventory/RecallInventoryFragments.h"
#include "Simulation/Inventory/RecallInventorySignalTypes.h"
#include "Simulation/Representation/RecallActorRepresentationFragments.h"
#include "System/Actor/RecallActorSubsystem.h"
#include "System/Inventory/RecallItemSubsystem.h"
#include "Utility/Inventory/RecallInventoryUtils.h"

//----------------------------------------------------------------------//
// URecallEquipmentConstructor
//----------------------------------------------------------------------//
URecallEquipmentConstructor::URecallEquipmentConstructor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EExtendedProcessorExecutionFlags::All);
	ObservedType = FRecallEquipmentFragment::StaticStruct();
	Operation = EMassExtendedObservedOperation::Add;
}

void URecallEquipmentConstructor::InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallEquipmentConstructor::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallEquipmentFragment>(EMassExtendedFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FRecallGameplayTagFragment>(EMassExtendedFragmentAccess::ReadWrite, EMassExtendedFragmentPresence::Optional);
	EntityQuery.AddConstSharedRequirement<FRecallEquipmentConstSharedFragment>();
	EntityQuery.AddSubsystemRequirement<URecallActorSubsystem>(EMassExtendedFragmentAccess::ReadWrite);
	EntityQuery.AddSubsystemRequirement<URecallSignalSubsystem>(EMassExtendedFragmentAccess::ReadWrite);
}

void URecallEquipmentConstructor::Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_Equipment_Constructor);

	EntityQuery.ForEachEntityChunk(Context, [](FMassExtendedExecutionContext& Context)
	{
		const URecallItemSubsystem& ItemSystem = URecallItemSubsystem::GetRef(Context.GetWorld());
		URecallActorSubsystem& ActorSystem = Context.GetMutableSubsystemChecked<URecallActorSubsystem>();
		URecallSignalSubsystem& SignalSystem = Context.GetMutableSubsystemChecked<URecallSignalSubsystem>();

		const FRecallEquipmentConstSharedFragment& EquipmentConstSharedFragment = Context.GetConstSharedFragment<FRecallEquipmentConstSharedFragment>();

		const TArrayView<FRecallEquipmentFragment> EquipmentList = Context.GetMutableFragmentView<FRecallEquipmentFragment>();
		const TArrayView<FRecallGameplayTagFragment> TagsList = Context.GetMutableFragmentView<FRecallGameplayTagFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			const FMassExtendedEntityHandle Entity = Context.GetEntity(EntityIndex);
			
			FRecallEquipmentFragment& EquipmentFragment = EquipmentList[EntityIndex];
			
			FRecallGameplayTagFragment* const TagsFragmentPtr = TagsList.IsValidIndex(EntityIndex) ? &TagsList[EntityIndex] : nullptr;
			
			const FRecallEquipmentContext EquipmentContext{
				ItemSystem, ActorSystem, SignalSystem,
				Entity, EquipmentConstSharedFragment, EquipmentFragment, TagsFragmentPtr
			};			
			Recall::Inventory::Utils::UpdateEquipmentEffects(EquipmentContext);
			Recall::Inventory::Utils::UpdateEquipmentRepresentation(EquipmentContext);
		}
	});
}

//----------------------------------------------------------------------//
// URecallEquipmentDestructor
//----------------------------------------------------------------------//
URecallEquipmentDestructor::URecallEquipmentDestructor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EExtendedProcessorExecutionFlags::All);
	ObservedType = FRecallEquipmentFragment::StaticStruct();
	Operation = EMassExtendedObservedOperation::Remove;
}

void URecallEquipmentDestructor::InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallEquipmentDestructor::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallEquipmentFragment>(EMassExtendedFragmentAccess::ReadWrite);
	EntityQuery.AddSubsystemRequirement<URecallActorSubsystem>(EMassExtendedFragmentAccess::ReadWrite);
}

void URecallEquipmentDestructor::Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_Equipment_Destructor);

	EntityQuery.ForEachEntityChunk(Context, [](FMassExtendedExecutionContext& Context)
	{
		URecallActorSubsystem& ActorSystem = Context.GetMutableSubsystemChecked<URecallActorSubsystem>();

		const TArrayView<FRecallEquipmentFragment> EquipmentList = Context.GetMutableFragmentView<FRecallEquipmentFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			FRecallEquipmentFragment& EquipmentFragment = EquipmentList[EntityIndex];

			for (TPair<FGameplayTag, FRecallEquipSlotRepresentation>& EquipSlotRepresentation : EquipmentFragment.EquipSlotRepresentationMap)
			{
				ActorSystem.ReleaseActor(EquipSlotRepresentation.Value.ActorHandle);
			}
			EquipmentFragment.EquipSlotRepresentationMap.Empty();
		}
	});
}

//----------------------------------------------------------------------//
// URecallEquipmentSignalProcessor
//----------------------------------------------------------------------//
URecallEquipmentSignalProcessor::URecallEquipmentSignalProcessor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void URecallEquipmentSignalProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);

	SubscribeToSignal(Recall::Inventory::Signals::Callback::OnLoot);
	SubscribeToSignal(Recall::Inventory::Signals::Callback::OnEquip);
	SubscribeToSignal(Recall::Inventory::Signals::Callback::OnUnequip);
}

void URecallEquipmentSignalProcessor::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallEquipmentFragment>(EMassExtendedFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FRecallGameplayTagFragment>(EMassExtendedFragmentAccess::ReadWrite, EMassExtendedFragmentPresence::Optional);
	EntityQuery.AddConstSharedRequirement<FRecallEquipmentConstSharedFragment>();
	EntityQuery.AddSubsystemRequirement<URecallSignalSubsystem>(EMassExtendedFragmentAccess::ReadWrite);
	EntityQuery.AddSubsystemRequirement<URecallActorSubsystem>(EMassExtendedFragmentAccess::ReadWrite);
}

void URecallEquipmentSignalProcessor::SignalEntities(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context, FRecallSignalNameLookup& EntitySignals)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_Equipment_Signal);

	EntityQuery.ForEachEntityChunk(Context, [&EntitySignals](FMassExtendedExecutionContext& Context)
	{
		const URecallItemSubsystem& ItemSystem = URecallItemSubsystem::GetRef(Context.GetWorld());
		URecallActorSubsystem& ActorSystem = Context.GetMutableSubsystemChecked<URecallActorSubsystem>();
		URecallSignalSubsystem& SignalSystem = Context.GetMutableSubsystemChecked<URecallSignalSubsystem>();
		
		const FRecallEquipmentConstSharedFragment& EquipmentConstSharedFragment = Context.GetConstSharedFragment<FRecallEquipmentConstSharedFragment>();
		
		const TArrayView<FRecallEquipmentFragment> EquipmentList = Context.GetMutableFragmentView<FRecallEquipmentFragment>();
		const TArrayView<FRecallGameplayTagFragment> TagsList = Context.GetMutableFragmentView<FRecallGameplayTagFragment>();
		
		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			const FMassExtendedEntityHandle Entity = Context.GetEntity(EntityIndex);

			FRecallEquipmentFragment& EquipmentFragment = EquipmentList[EntityIndex];
			
			FRecallGameplayTagFragment* const TagsFragmentPtr = TagsList.IsValidIndex(EntityIndex) ? &TagsList[EntityIndex] : nullptr;
			
			const FRecallEquipmentContext EquipmentContext{
				ItemSystem, ActorSystem, SignalSystem,
				Entity, EquipmentConstSharedFragment, EquipmentFragment, TagsFragmentPtr
			};
			
			TArray<FName> Signals;
			EntitySignals.GetSignalsForEntity(Entity, Signals);
			
			if (Signals.Contains(Recall::Inventory::Signals::Callback::OnLoot))
			{
				if (EquipmentConstSharedFragment.Settings.bAutoEquip && TagsFragmentPtr != nullptr)
				{
					Recall::Inventory::Utils::AutoEquipItem(EquipmentContext);				
				}
			}
			else if (Signals.Contains(Recall::Inventory::Signals::Callback::OnEquip)
				|| Signals.Contains(Recall::Inventory::Signals::Callback::OnUnequip))
			{
				Recall::Inventory::Utils::UpdateEquipmentEffects(EquipmentContext);
				Recall::Inventory::Utils::UpdateEquipmentRepresentation(EquipmentContext);
			}
		}
	});
}

//----------------------------------------------------------------------//
// URecallEquipmentRepresentationProcessor
//----------------------------------------------------------------------//
URecallEquipmentRepresentationProcessor::URecallEquipmentRepresentationProcessor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EExtendedProcessorExecutionFlags::All);
	ProcessingPhase = EMassExtendedProcessingPhase::Render;
	bRequiresGameThreadExecution = true;
}

void URecallEquipmentRepresentationProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallEquipmentRepresentationProcessor::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallEquipmentFragment>(EMassExtendedFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FRecallActorRepresentationFragment>(EMassExtendedFragmentAccess::ReadOnly);
	EntityQuery.AddConstSharedRequirement<FRecallEquipmentConstSharedFragment>();
	EntityQuery.AddSubsystemRequirement<URecallActorSubsystem>(EMassExtendedFragmentAccess::ReadOnly);
}

void URecallEquipmentRepresentationProcessor::Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_Equipment_Representation);

	EntityQuery.ForEachEntityChunk(Context, [](FMassExtendedExecutionContext& Context)
	{
		const URecallItemSubsystem& ItemSystem = URecallItemSubsystem::GetRef(Context.GetWorld());
		const URecallActorSubsystem& ActorSystem = Context.GetSubsystemChecked<URecallActorSubsystem>();
		
		const TConstArrayView<FRecallEquipmentFragment> EquipmentList = Context.GetFragmentView<FRecallEquipmentFragment>();
		const TConstArrayView<FRecallActorRepresentationFragment> ActorRepresentationList = Context.GetFragmentView<FRecallActorRepresentationFragment>();
		
		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			const FRecallEquipmentFragment& EquipmentFragment = EquipmentList[EntityIndex];
			const FRecallActorRepresentationFragment& ActorRepresentationFragment = ActorRepresentationList[EntityIndex];

			const TWeakObjectPtr<AActor> ParentActor = ActorSystem.GetActor(ActorRepresentationFragment.ActorHandle);
			if (!ParentActor.IsValid())
			{
				continue;
			}
			
			for (const TPair<FGameplayTag, FRecallEquipSlotRepresentation>& EquipSlotRepresentation : EquipmentFragment.EquipSlotRepresentationMap)
			{
				const FGameplayTag& EquipSlot = EquipSlotRepresentation.Key;
				const TWeakObjectPtr<AActor> EquipSlotActor = ActorSystem.GetActor(EquipSlotRepresentation.Value.ActorHandle);
				if (!ensureAlwaysMsgf(EquipSlotActor.IsValid(), TEXT("Actor must be value")))
				{
					continue;
				}
				
				FGameplayTag ItemTag;
				if (!EquipmentFragment.Equipment.GetEquipmentSlot(EquipSlot, ItemTag))
				{
					continue;
				}
				
				const TObjectPtr<const URecallInventoryItemAsset> ItemAsset = ItemSystem.GetItemAsset(ItemTag);
				if (!ItemAsset)
				{
					continue;
				}

				bool bHiddenInGame = false;

				if (ItemAsset->bOnlyShowWhenSelected && EquipSlot != EquipmentFragment.SelectedEquipSlot)
				{
					bHiddenInGame = true;
				}

				EquipSlotActor->SetActorHiddenInGame(bHiddenInGame);

				if (EquipSlotActor->GetParentActor() == ParentActor)
				{
					continue;
				}
				
				const FName* OverrideSocketNamePtr = ItemAsset->OverrideEquipSlotSocket.Find(EquipSlotRepresentation.Key);
				const FName SocketName = OverrideSocketNamePtr != nullptr ? *OverrideSocketNamePtr : ItemAsset->SocketName;

				const FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, false);

				if (ParentActor->GetClass()->ImplementsInterface(URecallEquipmentHolderActorInterface::StaticClass()))
				{
					USceneComponent* EquipmentSlotComponent = IRecallEquipmentHolderActorInterface::Execute_GetEquipmentSlotComponent(
						ParentActor.Get(), EquipSlotRepresentation.Key);
					if (IsValid(EquipmentSlotComponent))
					{
						EquipSlotActor->AttachToComponent(EquipmentSlotComponent, AttachmentTransformRules, SocketName);
						EquipSlotActor->SetActorRelativeTransform(ItemAsset->RelativeTransform);
						continue;
					}
				}
				
				EquipSlotActor->AttachToActor(ParentActor.Get(), AttachmentTransformRules, SocketName);
				EquipSlotActor->SetActorRelativeTransform(ItemAsset->RelativeTransform);
			}
		}
	});
}
