// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallGameplayEffectProcessors.h"

#include "Gameplay/RecallGameplayEffectContainer.h"
#include "MassExtendedExecutionContext.h"
#include "Simulation/GameplayEffect/RecallGameplayEffectFragments.h"

//----------------------------------------------------------------------//
// URecallGameplayEffectInitializer
//----------------------------------------------------------------------//
URecallGameplayEffectInitializer::URecallGameplayEffectInitializer()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EExtendedProcessorExecutionFlags::All);
	ObservedType = FRecallGameplayEffectFragment::StaticStruct();
	Operation = EMassExtendedObservedOperation::Add;
}

void URecallGameplayEffectInitializer::InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallGameplayEffectInitializer::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallGameplayEffectFragment>(EMassExtendedFragmentAccess::ReadWrite);
}

void URecallGameplayEffectInitializer::Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(Context, [](FMassExtendedExecutionContext& Context)
	{
		const TArrayView<FRecallGameplayEffectFragment> GameplayEffectList = Context.GetMutableFragmentView<FRecallGameplayEffectFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			const FMassExtendedEntityHandle Entity = Context.GetEntity(EntityIndex);
			
			FRecallGameplayEffectFragment& GameplayEffectFragment = GameplayEffectList[EntityIndex];
			GameplayEffectFragment.Container.Initialize(Entity);
		}
	});
}

//----------------------------------------------------------------------//
// URecallGameplayEffectTickProcessor
//----------------------------------------------------------------------//
URecallGameplayEffectTickProcessor::URecallGameplayEffectTickProcessor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EExtendedProcessorExecutionFlags::All);
	ProcessingPhase = EMassExtendedProcessingPhase::PrePhysics;
}

void URecallGameplayEffectTickProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallGameplayEffectTickProcessor::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallGameplayEffectFragment>(EMassExtendedFragmentAccess::ReadWrite);
}

void URecallGameplayEffectTickProcessor::Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_GameplayEffectTick_Execute);
	
	EntityQuery.ForEachEntityChunk(Context, [](FMassExtendedExecutionContext& Context)
	{
		const float DeltaTime = Context.GetDeltaTimeSeconds();
		
		const FMassExtendedEntityManager& EntityManager = Context.GetEntityManagerChecked();
		
		const TArrayView<FRecallGameplayEffectFragment> GameplayEffectList = Context.GetMutableFragmentView<FRecallGameplayEffectFragment>();
		
		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			const FMassExtendedEntityHandle Entity = Context.GetEntity(EntityIndex);
			const FRecallGameplayEffectContext EffectContext{ EntityManager, Entity };
			
			FRecallGameplayEffectFragment& GameplayEffectFragment = GameplayEffectList[EntityIndex];
			GameplayEffectFragment.Container.TickEffects(EffectContext,
				DeltaTime, ERecallGameplayEffectTickSource::Default);
		}
	});
}
