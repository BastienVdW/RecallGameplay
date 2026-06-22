// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallTemplateProcessors.h"

#include "MassExecutionContext.h"
#include "RecallSignalSubsystem.h"
#include "Simulation/_template/RecallTemplateFragments.h"

//----------------------------------------------------------------------//
// URecallTemplateConstructor
//----------------------------------------------------------------------//
URecallTemplateConstructor::URecallTemplateConstructor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::All);
	ObservedType = FRecallTemplateFragment::StaticStruct();
	Operation = EMassObservedOperation::Add;
}

void URecallTemplateConstructor::InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallTemplateConstructor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallTemplateFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddConstSharedRequirement<FRecallTemplateConstSharedFragment>();
}

void URecallTemplateConstructor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_Template_Constructor);

	EntityQuery.ForEachEntityChunk(Context, [](FMassExecutionContext& Context)
	{
		const FRecallTemplateConstSharedFragment& TemplateSharedFragment = Context.GetConstSharedFragment<FRecallTemplateConstSharedFragment>();

		const TArrayView<FRecallTemplateFragment> TemplateList = Context.GetMutableFragmentView<FRecallTemplateFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			FRecallTemplateFragment& TemplateFragment = TemplateList[EntityIndex];
		}
	});
}

//----------------------------------------------------------------------//
// URecallTemplateDestructor
//----------------------------------------------------------------------//
URecallTemplateDestructor::URecallTemplateDestructor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::All);
	ObservedType = FRecallTemplateFragment::StaticStruct();
	Operation = EMassObservedOperation::Remove;
}

void URecallTemplateDestructor::InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallTemplateDestructor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallTemplateFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddConstSharedRequirement<FRecallTemplateConstSharedFragment>();

}

void URecallTemplateDestructor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_Template_Destructor);

	EntityQuery.ForEachEntityChunk(Context, [](FMassExecutionContext& Context)
	{
		const FRecallTemplateConstSharedFragment& TemplateSharedFragment = Context.GetConstSharedFragment<FRecallTemplateConstSharedFragment>();

		const TArrayView<FRecallTemplateFragment> TemplateList = Context.GetMutableFragmentView<FRecallTemplateFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			FRecallTemplateFragment& TemplateFragment = TemplateList[EntityIndex];
		}
	});
}

//----------------------------------------------------------------------//
// URecallTemplateSignalProcessor
//----------------------------------------------------------------------//
URecallTemplateSignalProcessor::URecallTemplateSignalProcessor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void URecallTemplateSignalProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);

	// SubscribeToSignal(Recall::Template::Signals::DummySignal);
}

void URecallTemplateSignalProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallTemplateFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddConstSharedRequirement<FRecallTemplateConstSharedFragment>();
	EntityQuery.AddSubsystemRequirement<URecallSignalSubsystem>(EMassFragmentAccess::ReadWrite);
}

void URecallTemplateSignalProcessor::SignalEntities(FMassEntityManager& EntityManager, FMassExecutionContext& Context, FRecallSignalNameLookup& EntitySignals)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_Template_Signal);

	EntityQuery.ForEachEntityChunk(Context, [&EntitySignals](FMassExecutionContext& Context)
	{
		const FMassEntityManager& EntityManager = Context.GetEntityManagerChecked();
		URecallSignalSubsystem& SignalSystem = Context.GetMutableSubsystemChecked<URecallSignalSubsystem>();

		const FRecallTemplateConstSharedFragment& TemplateConstSharedFragment = Context.GetConstSharedFragment<FRecallTemplateConstSharedFragment>();

		const TArrayView<FRecallTemplateFragment> TemplateList = Context.GetMutableFragmentView<FRecallTemplateFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			const FMassEntityHandle Entity = Context.GetEntity(EntityIndex);

			FRecallTemplateFragment& TemplateFragment = TemplateList[EntityIndex];
		}
	});
}

//----------------------------------------------------------------------//
// URecallTemplateProcessor
//----------------------------------------------------------------------//
URecallTemplateProcessor::URecallTemplateProcessor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::All);
	ProcessingPhase = EMassProcessingPhase::FrameEnd;
}

void URecallTemplateProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

bool URecallTemplateProcessor::ShouldAllowQueryBasedPruning(const bool bRuntimeMode /*= true*/) const
{
	return Super::ShouldAllowQueryBasedPruning(bRuntimeMode);
}

void URecallTemplateProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallTemplateFragment>(EMassFragmentAccess::ReadOnly);
}

void URecallTemplateProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_Template_Execute);

	EntityQuery.ForEachEntityChunk(Context, [](FMassExecutionContext& Context)
	{
		const FMassEntityManager& EntityManager = Context.GetEntityManagerChecked();

		const TConstArrayView<FRecallTemplateFragment> TemplateList = Context.GetFragmentView<FRecallTemplateFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			const FRecallTemplateFragment& TemplateFragment = TemplateList[EntityIndex];
		}
	});
}
