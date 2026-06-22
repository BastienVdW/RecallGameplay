// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallGameplayEffectAreaProcessors.h"

#include "MassEntityView.h"
#include "MassExecutionContext.h"
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

void URecallGameplayEffectAreaSignalProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
	
	SubscribeToSignal(Recall::Physics::Signals::NewOverlap);
	SubscribeToSignal(Recall::Physics::Signals::MinusOverlap);
}

void URecallGameplayEffectAreaSignalProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallGameplayEffectAreaFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FRecallPhysicsSensorFragment>(EMassFragmentAccess::ReadOnly);
}

static void UpdateAreaGameplayEffects(const FMassExecutionContext& Context, const FMassEntityHandle& OwnerEntity,
	FRecallGameplayEffectAreaFragment& GameplayEffectAreaFragment, const TArray<FMassEntityHandle>& OverlappingEntities)
{	
	const FMassEntityManager& EntityManager = Context.GetEntityManagerChecked();

	TArray<FMassEntityHandle> OldOverlappingEntities;
	GameplayEffectAreaFragment.InstanceMap.GetKeys(OldOverlappingEntities);

	// Remove effect from entities that do not overlap anymore
	for (const FMassEntityHandle& OldOverlappingEntity : OldOverlappingEntities)
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

		const FMassEntityView OldOverlappingView(EntityManager, OldOverlappingEntity);
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
		for (const FMassEntityHandle& NewOverlappingEntity : OverlappingEntities)
		{
			if (GameplayEffectAreaFragment.InstanceMap.Contains(NewOverlappingEntity) ||
				!EntityManager.IsEntityValid(NewOverlappingEntity))
			{
				continue;
			}
			
			const FMassEntityView NewOverlappingView(EntityManager, NewOverlappingEntity);
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

void URecallGameplayEffectAreaSignalProcessor::SignalEntities(FMassEntityManager& EntityManager,
	FMassExecutionContext& Context, FRecallSignalNameLookup& EntitySignals)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_GameplayEffectArea_Signal);

	EntityQuery.ForEachEntityChunk(Context, [](FMassExecutionContext& Context)
	{		
		const TConstArrayView<FRecallPhysicsSensorFragment> SensorList = Context.GetFragmentView<FRecallPhysicsSensorFragment>();
		const TArrayView<FRecallGameplayEffectAreaFragment> GameplayEffectAreaList = Context.GetMutableFragmentView<FRecallGameplayEffectAreaFragment>();
		
		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			const FMassEntityHandle Entity = Context.GetEntity(EntityIndex);
			FRecallGameplayEffectAreaFragment& GameplayEffectAreaFragment = GameplayEffectAreaList[EntityIndex];
			
			const FRecallPhysicsSensorFragment& SensorFragment = SensorList[EntityIndex];
			const TArray<FMassEntityHandle> OverlappingEntities = SensorFragment.GetOverlappingEntities();

			UpdateAreaGameplayEffects(Context, Entity, GameplayEffectAreaFragment, OverlappingEntities);
		}
	});
}
