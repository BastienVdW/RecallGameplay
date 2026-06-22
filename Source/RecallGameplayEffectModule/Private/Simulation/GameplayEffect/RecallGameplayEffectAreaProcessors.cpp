// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallGameplayEffectAreaProcessors.h"

#include "MassExtendedEntityView.h"
#include "MassExtendedExecutionContext.h"
#include "Gameplay/RecallGameplayEffect.h"
#include "Simulation/GameplayEffect/RecallGameplayEffectFragments.h"
#include "Simulation/Physics/RecallPhysicsSensorFragment.h"
#include "Simulation/Physics/RecallPhysicsSignalTypes.h"

//----------------------------------------------------------------------//
// URecallGameplayEffectAreaSignalProcessor
//----------------------------------------------------------------------//
URecallGameplayEffectAreaSignalProcessor::URecallGameplayEffectAreaSignalProcessor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void URecallGameplayEffectAreaSignalProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
	
	SubscribeToSignal(Recall::Physics::Signals::NewOverlap);
	SubscribeToSignal(Recall::Physics::Signals::MinusOverlap);
}

void URecallGameplayEffectAreaSignalProcessor::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallGameplayEffectAreaFragment>(EMassExtendedFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FRecallPhysicsSensorFragment>(EMassExtendedFragmentAccess::ReadOnly);
}

static void UpdateAreaGameplayEffects(const FMassExtendedExecutionContext& Context, const FMassExtendedEntityHandle& OwnerEntity,
	FRecallGameplayEffectAreaFragment& GameplayEffectAreaFragment, const TArray<FMassExtendedEntityHandle>& OverlappingEntities)
{	
	const FMassExtendedEntityManager& EntityManager = Context.GetEntityManagerChecked();

	TArray<FMassExtendedEntityHandle> OldOverlappingEntities;
	GameplayEffectAreaFragment.InstanceMap.GetKeys(OldOverlappingEntities);

	// Remove effect from entities that do not overlap anymore
	for (const FMassExtendedEntityHandle& OldOverlappingEntity : OldOverlappingEntities)
	{
		if (OverlappingEntities.Contains(OldOverlappingEntity))
		{
			continue;
		}

		FRecallGameplayEffectAreaInstance OldInstance;
		if (!GameplayEffectAreaFragment.InstanceMap.RemoveAndCopyValue(OldOverlappingEntity, OldInstance))
		{
			continue;
		}

		if (!EntityManager.IsEntityValid(OldOverlappingEntity))
		{
			continue;
		}

		const FMassExtendedEntityView OldOverlappingView(EntityManager, OldOverlappingEntity);
		auto* GameplayEffectFragmentPtr = OldOverlappingView.GetFragmentDataPtr<FRecallGameplayEffectFragment>();
		if (GameplayEffectFragmentPtr == nullptr)
		{
			continue;
		}
		
		const FRecallGameplayEffectContext GameplayEffectContext {
			EntityManager, OwnerEntity
		};
			
		GameplayEffectFragmentPtr->Container.RemoveEffect(
			GameplayEffectContext, OldInstance.EffectHandle);
	}

	// Add effect to newly overlapping entities
	if (GameplayEffectAreaFragment.EffectClass)
	{
		for (const FMassExtendedEntityHandle& NewOverlappingEntity : OverlappingEntities)
		{
			if (GameplayEffectAreaFragment.InstanceMap.Contains(NewOverlappingEntity) ||
				!EntityManager.IsEntityValid(NewOverlappingEntity))
			{
				continue;
			}
			
			const FMassExtendedEntityView NewOverlappingView(EntityManager, NewOverlappingEntity);
			auto* GameplayEffectFragmentPtr = NewOverlappingView.GetFragmentDataPtr<FRecallGameplayEffectFragment>();
			if (GameplayEffectFragmentPtr == nullptr)
			{
				continue;
			}

			const FRecallGameplayEffectContext GameplayEffectContext {
				EntityManager, OwnerEntity
			};
		
			FRecallGameplayEffectAreaInstance& Instance = GameplayEffectAreaFragment.InstanceMap.Add(
				NewOverlappingEntity);
			Instance.EffectHandle = GameplayEffectFragmentPtr->Container.AddEffect(
				GameplayEffectContext, GameplayEffectAreaFragment.EffectClass);
		}
	}
}

void URecallGameplayEffectAreaSignalProcessor::SignalEntities(FMassExtendedEntityManager& EntityManager,
	FMassExtendedExecutionContext& Context, FRecallSignalNameLookup& EntitySignals)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_GameplayEffectArea_Signal);

	EntityQuery.ForEachEntityChunk(Context, [](FMassExtendedExecutionContext& Context)
	{		
		const TConstArrayView<FRecallPhysicsSensorFragment> SensorList = Context.GetFragmentView<FRecallPhysicsSensorFragment>();
		const TArrayView<FRecallGameplayEffectAreaFragment> GameplayEffectAreaList = Context.GetMutableFragmentView<FRecallGameplayEffectAreaFragment>();
		
		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			const FMassExtendedEntityHandle Entity = Context.GetEntity(EntityIndex);
			FRecallGameplayEffectAreaFragment& GameplayEffectAreaFragment = GameplayEffectAreaList[EntityIndex];
			
			const FRecallPhysicsSensorFragment& SensorFragment = SensorList[EntityIndex];
			const TArray<FMassExtendedEntityHandle> OverlappingEntities = SensorFragment.GetOverlappingEntities();

			UpdateAreaGameplayEffects(Context, Entity, GameplayEffectAreaFragment, OverlappingEntities);
		}
	});
}
