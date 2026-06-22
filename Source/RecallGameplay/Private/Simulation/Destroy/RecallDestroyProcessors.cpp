// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallDestroyProcessors.h"

#include "MassExtendedExecutionContext.h"
#include "Simulation/Destroy/RecallDestroySignalTypes.h"
#include "Simulation/Transform/RecallTransformFragments.h"

//----------------------------------------------------------------------//
// URecallDestroySignalProcessor
//----------------------------------------------------------------------//
URecallDestroySignalProcessor::URecallDestroySignalProcessor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ProcessingPhase = EMassExtendedProcessingPhase::FrameEnd;
}

void URecallDestroySignalProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);

	SubscribeToSignal(Recall::Destroy::Signals::Destroy);
}

void URecallDestroySignalProcessor::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallTransformFragment>(EMassExtendedFragmentAccess::ReadOnly);
}

void URecallDestroySignalProcessor::SignalEntities(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context, FRecallSignalNameLookup& EntitySignals)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_Destroy_Signal);

	EntityQuery.ForEachEntityChunk(Context, [&EntitySignals](FMassExtendedExecutionContext& Context)
	{
		Context.Defer().DestroyEntities(Context.GetEntities());
	});
}
