// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallStateTreeEntityFilterTypes.h"

#include "MassExtendedEntityManager.h"
#include "MassExtendedEntityView.h"
#include "Simulation/GameplayTag/RecallGameplayTagFragments.h"
#include "Simulation/Transform/RecallTransformFragments.h"
#include "StateTree/RecallStateTreeExecutionContext.h"
#include "Utility/GameplayTag/RecallGameplayTagUtils.h"

static void RemoveInvalidEntities(const FRecallStateTreeExecutionContext& Context,
	TArray<FMassExtendedEntityHandle>& Entities)
{
	const FMassExtendedEntityManager& EntityManager = Context.GetEntityManager();
	for (int32 EntityIndex = Entities.Num() - 1; EntityIndex >= 0; --EntityIndex)
	{
		const FMassExtendedEntityHandle& Entity = Entities[EntityIndex];
		if (!EntityManager.IsEntityValid(Entity))
		{
			Entities.RemoveAtSwap(EntityIndex);
		}
	}
}

//----------------------------------------------------------------------//
// FRecallStateTreeEntityFilterGameplayTag
//----------------------------------------------------------------------//
void FRecallStateTreeEntityFilterGameplayTag::FilterEntities(const FRecallStateTreeExecutionContext& Context,
	TArray<FMassExtendedEntityHandle>& Entities) const
{
	RemoveInvalidEntities(Context, Entities);
		
	TArray<bool> Results;
	Results.SetNum(Entities.Num());

	const FMassExtendedEntityManager& EntityManager = Context.GetEntityManager();
	ParallelFor(Entities.Num(),
		[this, &Entities, &EntityManager, &Results](int32 Index)
	{
		const FMassExtendedEntityHandle& Entity = Entities[Index];
		const FMassExtendedEntityView EntityView(EntityManager, Entity);

		const auto* SlaveGameplayTagFragmentPtr = EntityView.GetFragmentDataPtr<FRecallGameplayTagFragment>();
		Results[Index] = SlaveGameplayTagFragmentPtr != nullptr ? Recall::GameplayTag::Utils::EvaluateCondition(
			GameplayTagCondition, SlaveGameplayTagFragmentPtr->GameplayTagCountMap) : false;
	});

	for (int32 EntityIndex = Entities.Num() - 1; EntityIndex >= 0; --EntityIndex)
	{
		if (Results[EntityIndex])
		{
			continue;
		}
		
		Entities.RemoveAtSwap(EntityIndex);
	}
}

//----------------------------------------------------------------------//
// FRecallStateTreeEntityFilterSelect
//----------------------------------------------------------------------//
void FRecallStateTreeEntityFilterSelect::FilterEntities(const FRecallStateTreeExecutionContext& Context,
	TArray<FMassExtendedEntityHandle>& Entities) const
{
	RemoveInvalidEntities(Context, Entities);
	
	const int32 RemoveCount = Entities.Num() - MaxCount;	
	if (RemoveCount > 0)
	{
		Sort(Context, Entities);
		Entities.RemoveAt(MaxCount, RemoveCount);
	}
}

void FRecallStateTreeEntityFilterSelect::Sort(const FRecallStateTreeExecutionContext& Context,
	TArray<FMassExtendedEntityHandle>& Entities) const
{
	switch (Selection)
	{
	case EFRecallStateTreeEntityFilterSelectType::Distance:
		SortByDistance(Context, Entities);
		break;

	case EFRecallStateTreeEntityFilterSelectType::Random:
		SortShuffle(Context, Entities);
		break;

	default:
		unimplemented();
		break;
	}
}

void FRecallStateTreeEntityFilterSelect::SortByDistance(const FRecallStateTreeExecutionContext& Context,
	TArray<FMassExtendedEntityHandle>& Entities)
{
	const FMassExtendedEntityManager& EntityManager = Context.GetEntityManager();
	const FMassExtendedEntityHandle& OwnerEntity = Context.GetEntity();

	auto GetEntityLocation = [&EntityManager](const FMassExtendedEntityHandle& Entity)
	{
		const FMassExtendedEntityView EntityView(EntityManager, Entity);
		const auto& TransformFragment = EntityView.GetFragmentData<FRecallTransformFragment>();
		return TransformFragment.Position;
	};

	const FVector OwnerLocation = GetEntityLocation(OwnerEntity);

	TMap<FMassExtendedEntityHandle, float> Distances;
	
	for (const FMassExtendedEntityHandle& Entity : Entities)
	{
		const FVector Location = GetEntityLocation(OwnerEntity);
		const float DistanceSquared = FVector::DistSquared(Location, OwnerLocation);
		
		Distances.Add(Entity, DistanceSquared);
	}
	
	Entities.Sort(
		[&Distances](const FMassExtendedEntityHandle& lEntity, const FMassExtendedEntityHandle& rEntity)
	{
		return Distances[lEntity] < Distances[rEntity];
	});
}

void FRecallStateTreeEntityFilterSelect::SortShuffle(const FRecallStateTreeExecutionContext& Context,
	TArray<FMassExtendedEntityHandle>& Entities)
{
	const FRandomStream& RandomStream = Context.GetRandomStream();
	const int32 LastIndex = Entities.Num() - 1;
	
	for (int32 Index = 0; Index < LastIndex; ++Index)
	{
		const int32 ShuffleIndex = RandomStream.RandRange(Index, LastIndex);
		Entities.Swap(Index, ShuffleIndex);
	}
}
