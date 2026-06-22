// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallTrackerRepresentationProcessors.h"

#include "MassExecutionContext.h"
#include "Representation/Tracker/RecallTrackerReactInterface.h"
#include "Representation/Tracker/RecallTrackerRepresentationTypes.h"
#include "Simulation/Representation/RecallTrackerRepresentationFragments.h"
#include "Simulation/Transform/RecallTransformFragments.h"
#include "System/Observer/RecallObserverSubjectSubsystem.h"

//----------------------------------------------------------------------//
// URecallTrackerRepresentationProcessor
//----------------------------------------------------------------------//
URecallTrackerRepresentationProcessor::URecallTrackerRepresentationProcessor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::All);
	ProcessingPhase = EMassProcessingPhase::Render;
	bRequiresGameThreadExecution = true;
}

struct FRecallTrackerRepresentationCacheManager
{
	TArray<FRecallTrackedEntityRepresentation> TrackedEntities;

	FORCEINLINE void ResetCache()
	{
		TrackedEntities.Reset();
	}
};

void URecallTrackerRepresentationProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);

	CacheManager = MakeShared<FRecallTrackerRepresentationCacheManager>();
}

void URecallTrackerRepresentationProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallTransformFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddConstSharedRequirement<FRecallTrackerRepresentationConstSharedFragment>();

	ProcessorRequirements.AddSubsystemRequirement<URecallObserverSubjectSubsystem>(EMassFragmentAccess::ReadOnly);
}

void URecallTrackerRepresentationProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_Tracker_Representation);

	const URecallObserverSubjectSubsystem& ObserverSubjectSystem = Context.GetSubsystemChecked<URecallObserverSubjectSubsystem>();
	const auto Observers = ObserverSubjectSystem.GetObservers<IRecallTrackerReactInterface>();
	if (Observers.Num() == 0)
	{
		return;
	}

	check(CacheManager.IsValid());
	CacheManager->ResetCache();
	
	TArray<FRecallTrackedEntityRepresentation>& TrackedEntities = CacheManager->TrackedEntities;
	
	EntityQuery.ForEachEntityChunk(Context, [&TrackedEntities](FMassExecutionContext& Context)
	{
		const auto& TrackerRepresentationConstSharedFragment = Context.GetConstSharedFragment<FRecallTrackerRepresentationConstSharedFragment>();

		const TConstArrayView<FRecallTransformFragment> TransformList = Context.GetFragmentView<FRecallTransformFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			const FRecallTransformFragment& TransformFragment = TransformList[EntityIndex];
	
			FRecallTrackedEntityRepresentation& NewTrackedEntity = TrackedEntities.AddDefaulted_GetRef();
			NewTrackedEntity.Entity = Context.GetEntity(EntityIndex);
			NewTrackedEntity.Location = TransformFragment.Position;
			NewTrackedEntity.TrackerWidgetClass = TrackerRepresentationConstSharedFragment.TrackerWidgetClass;
		}
	});

	for (const auto& Observer : Observers)
	{
		Observer.Interface.SetTrackedEntities(TrackedEntities);
	}
}
