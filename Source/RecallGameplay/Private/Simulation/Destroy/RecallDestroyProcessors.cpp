// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallDestroyProcessors.h"

#include "MassExecutionContext.h"
#include "Simulation/Destroy/RecallDestroySignalTypes.h"
#include "Simulation/Transform/RecallTransformFragments.h"

//----------------------------------------------------------------------//
// URecallDestroySignalProcessor
//----------------------------------------------------------------------//
URecallDestroySignalProcessor::URecallDestroySignalProcessor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ProcessingPhase = EMassProcessingPhase::FrameEnd;
}

void URecallDestroySignalProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);

	SubscribeToSignal(Recall::Destroy::Signals::Destroy);
}

void URecallDestroySignalProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallTransformFragment>(EMassFragmentAccess::ReadOnly);
}

void URecallDestroySignalProcessor::SignalEntities(FMassEntityManager& EntityManager, FMassExecutionContext& Context, FRecallSignalNameLookup& EntitySignals)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_Destroy_Signal);

	EntityQuery.ForEachEntityChunk(Context, [&EntitySignals](FMassExecutionContext& Context)
	{
		Context.Defer().DestroyEntities(Context.GetEntities());
	});
}
