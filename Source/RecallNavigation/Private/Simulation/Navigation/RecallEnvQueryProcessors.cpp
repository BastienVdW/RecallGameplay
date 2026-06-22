// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallEnvQueryProcessors.h"

#include "MassExtendedExecutionContext.h"
#include "Simulation/StateTree/RecallStateTreeProcessorGroupTypes.h"
#include "System/EQS/RecallEnvQuerySubsystem.h"

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
#include "DrawDebugHelpers.h"
#endif // UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT

//----------------------------------------------------------------------//
// URecallEnvQueryProcessor
//----------------------------------------------------------------------//
URecallEnvQueryProcessor::URecallEnvQueryProcessor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EExtendedProcessorExecutionFlags::All);
	ProcessingPhase = EMassExtendedProcessingPhase::PrePhysics;
	ExecutionOrder.ExecuteBefore.Add(Recall::StateTree::ProcessorGroupNames::StateTreeUpdate);
}

void URecallEnvQueryProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

bool URecallEnvQueryProcessor::ShouldAllowQueryBasedPruning(const bool bRuntimeMode /*= true*/) const
{
	return false;
}

void URecallEnvQueryProcessor::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager)
{
	ProcessorRequirements.AddSubsystemRequirement<URecallEnvQuerySubsystem>(EMassExtendedFragmentAccess::ReadWrite);
}

void URecallEnvQueryProcessor::Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_EnvQuery_Execute);

	URecallEnvQuerySubsystem& EnvQuerySystem = Context.GetMutableSubsystemChecked<URecallEnvQuerySubsystem>();
	EnvQuerySystem.TickEnvQueryQueue();
}

//----------------------------------------------------------------------//
// URecallEnvQueryRepresentationProcessor
//----------------------------------------------------------------------//
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
static bool bDebugShowEnvQueryResult = false;
static FAutoConsoleVariableRef CVarRecallShowEnvQueryResult(
	TEXT("recall.stateTree.ShowEnvQueryResult"),
	bDebugShowEnvQueryResult,
	TEXT("Show Env Query Result")
);
#endif // UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT

URecallEnvQueryRepresentationProcessor::URecallEnvQueryRepresentationProcessor()
{
	ExecutionFlags = static_cast<int32>(EExtendedProcessorExecutionFlags::All);
	ProcessingPhase = EMassExtendedProcessingPhase::Render;
	bRequiresGameThreadExecution = true;
}

void URecallEnvQueryRepresentationProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

bool URecallEnvQueryRepresentationProcessor::ShouldAllowQueryBasedPruning(const bool bRuntimeMode /*= true*/) const
{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	return false;
#else // UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	return true;
#endif
}

void URecallEnvQueryRepresentationProcessor::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager)
{
	Super::ConfigureQueries(EntityManager);

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	ProcessorRequirements.AddSubsystemRequirement<URecallEnvQuerySubsystem>(EMassExtendedFragmentAccess::ReadOnly);
#endif // UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
}

void URecallEnvQueryRepresentationProcessor::Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context)
{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	QUICK_SCOPE_CYCLE_COUNTER(Recall_EnvQueryRepresentation_Execute);

	if (!bDebugShowEnvQueryResult)
	{
		return;
	}

	const URecallEnvQuerySubsystem& EnvQuerySystem = Context.GetSubsystemChecked<URecallEnvQuerySubsystem>();

	for (const TPair<FRecallEnvQueryHandle, FRecallEnvQueryDebugCache>& DebugCache : EnvQuerySystem.GetEnvQueryDebugCache())
	{
		DrawDebugSphere(Context.GetWorld(), DebugCache.Value.QueryResult, 30.f, 12, FColor::Emerald);

		for (const FRecallEnvQueryItem& Item : DebugCache.Value.Items)
		{
			const FColor ItemColor(Item.Score * 255.f, Item.Score * 255.f, Item.Score * 255.f);
			DrawDebugSphere(Context.GetWorld(), Item.Location, 20.f, 3, ItemColor);
		}
	}
#endif // UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
}
