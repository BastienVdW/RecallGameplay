// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallStateTreeProcessors.h"

#include "Desync/RecallDesyncLog.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "MassExecutionContext.h"
#include "RecallSignalSubsystem.h"
#include "Navigation/RecallNavigationSignalTypes.h"
#include "ProfilingDebugging/CsvProfiler.h"
#include "Simulation/Player/RecallPlayerProcessorGroupTypes.h"
#include "Simulation/Player/RecallPlayerSignalTypes.h"
#include "Simulation/StateTree/RecallStateTreeFragments.h"
#include "Simulation/StateTree/RecallStateTreeProcessorGroupTypes.h"
#include "Simulation/StateTree/RecallStateTreeSignalTypes.h"
#include "Simulation/StateTree/RecallStateTreeUtils.h"
#include "Simulation/Transform/RecallTransformFragments.h"
#include "StateTree/RecallStateTreeExecutionContext.h"
#include "StateTree.h"
#include "Simulation/Entity/RecallEntityProcessorGroupTypes.h"
#include "Simulation/Physics/RecallPhysicsSignalTypes.h"
#include "System/AI/RecallStateTreeSubsystem.h"
#include "System/Random/RecallRandomNumberSubsystem.h"
#include "Utility/Simulation/RecallSimulationUtils.h"
#include "VisualLogger/VisualLogger.h"

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
#include "DrawDebugHelpers.h"

static bool bDebugPauseStateTreeExecution = false;
static FAutoConsoleVariableRef CVarRecallPauseStateTree(
	TEXT("recall.AI.PauseBehaviors"),
	bDebugPauseStateTreeExecution,
	TEXT("Enables/Disables The AI Behaviors")
);
#endif // UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT

CSV_DEFINE_CATEGORY(RecallStateTreeProcessor, true);

//----------------------------------------------------------------------//
// URecallStateTreeFragmentDestructor
//----------------------------------------------------------------------//
URecallStateTreeFragmentDestructor::URecallStateTreeFragmentDestructor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::All);
	ObservedTypes.Add(FRecallStateTreeInstanceFragment::StaticStruct());
	ObservedOperations = EMassObservedOperationFlags::Remove;
}

void URecallStateTreeFragmentDestructor::InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallStateTreeFragmentDestructor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallStateTreeInstanceFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddSubsystemRequirement<URecallStateTreeSubsystem>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddConstSharedRequirement<FRecallStateTreeSharedFragment>();
	EntityQuery.AddSubsystemRequirement<URecallSignalSubsystem>(EMassFragmentAccess::ReadWrite);
}

void URecallStateTreeFragmentDestructor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_StateTree_Destructor);

	EntityQuery.ForEachEntityChunk(Context,
		[&EntityManager](FMassExecutionContext& Context)
	{
		URecallSignalSubsystem& SignalSystem = Context.GetMutableSubsystemChecked<URecallSignalSubsystem>();
		URecallStateTreeSubsystem& StateTreeSystem = Context.GetMutableSubsystemChecked<URecallStateTreeSubsystem>();
		const TArrayView<FRecallStateTreeInstanceFragment> StateTreeInstanceList = Context.GetMutableFragmentView<FRecallStateTreeInstanceFragment>();

		Recall::StateTree::Behavior::ForEachEntityInChunk(Context, EntityManager, SignalSystem, StateTreeSystem,
			[](FRecallStateTreeExecutionContext& StateTreeExecutionContext, FRecallStateTreeInstanceFragment& StateTreeFragment)
			{
				// Stop the tree instance
				StateTreeExecutionContext.Stop();
			}
		);

		// Free the StateTree instance memory
		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			FRecallStateTreeInstanceFragment& StateTreeInstanceFragment = StateTreeInstanceList[EntityIndex];
			if (StateTreeInstanceFragment.InstanceHandle.IsValid())
			{
				StateTreeSystem.FreeInstanceData(StateTreeInstanceFragment.InstanceHandle);
			}

			checkf(StateTreeInstanceFragment.SubInstanceHandleMap.IsEmpty(), TEXT("Should have been emptied when state tree was stopped"));
			checkf(StateTreeInstanceFragment.SubInstanceHandles.IsEmpty(), TEXT("Should have been emptied when state tree was stopped"));
		}
	});
}

//----------------------------------------------------------------------//
// URecallStateTreeActivationProcessor
//----------------------------------------------------------------------//
URecallStateTreeActivationProcessor::URecallStateTreeActivationProcessor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::All);
	ProcessingPhase = EMassProcessingPhase::PostPhysics;
	ExecutionOrder.ExecuteInGroup = Recall::StateTree::ProcessorGroupNames::StateTreeActivation;
}

void URecallStateTreeActivationProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallStateTreeInstanceFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddTagRequirement<FRecallStateTreeActivatedTag>(EMassFragmentPresence::None);
	EntityQuery.AddConstSharedRequirement<FRecallStateTreeSharedFragment>();
	EntityQuery.AddSubsystemRequirement<URecallSignalSubsystem>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddSubsystemRequirement<URecallStateTreeSubsystem>(EMassFragmentAccess::ReadWrite);
}

void URecallStateTreeActivationProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_StateTree_Activation);

	EntityQuery.ForEachEntityChunk(Context, [](FMassExecutionContext& Context)
	{
		URecallStateTreeSubsystem& StateTreeSystem = Context.GetMutableSubsystemChecked<URecallStateTreeSubsystem>();
		URecallSignalSubsystem& SignalSystem = Context.GetMutableSubsystemChecked<URecallSignalSubsystem>();

		const FRecallStateTreeSharedFragment& SharedStateTree = Context.GetConstSharedFragment<FRecallStateTreeSharedFragment>();

		const TArrayView<FRecallStateTreeInstanceFragment> StateTreeInstanceList = Context.GetMutableFragmentView<FRecallStateTreeInstanceFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			// Allocate and initialize the StateTree instance memory
			FRecallStateTreeInstanceFragment& StateTreeInstance = StateTreeInstanceList[EntityIndex];
			StateTreeInstance.OwnerEntityHandle = Context.GetEntity(EntityIndex);
			StateTreeInstance.InstanceHandle = StateTreeSystem.AllocateInstanceData();

			Context.Defer().AddTag<FRecallStateTreeActivatedTag>(Context.GetEntity(EntityIndex));
		}

		if (SharedStateTree.bAutoStart)
		{
			SignalSystem.SignalEntities(Recall::StateTree::Signals::StateTreeStart, Context.GetEntities());
		}
	});
}

//----------------------------------------------------------------------//
// URecallStateTreeStartProcessor
//----------------------------------------------------------------------//
URecallStateTreeStartProcessor::URecallStateTreeStartProcessor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ProcessingPhase = EMassProcessingPhase::FrameEnd;
	ExecutionOrder.ExecuteInGroup = Recall::StateTree::ProcessorGroupNames::StateTreeStart;
}

struct FRecallStateTreeStartCacheManager
{
	TArray<FMassEntityHandle> EntitiesToSignal;

	void ResetCache()
	{
		EntitiesToSignal.Reset();
	}
};

void URecallStateTreeStartProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);

	CacheManager = MakeShared<FRecallStateTreeStartCacheManager>();

	SubscribeToSignal(Recall::StateTree::Signals::StateTreeStart);
}

void URecallStateTreeStartProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	FMassTagBitSet RequiredTags;
	RequiredTags.Add(FRecallStateTreeActivatedTag::StaticStruct());

	FMassTagBitSet InvalidTags;
	InvalidTags.Add(FRecallStateTreeRunningTag::StaticStruct());
	
	EntityQuery.AddRequirement<FRecallStateTreeInstanceFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddSubsystemRequirement<URecallStateTreeSubsystem>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddConstSharedRequirement<FRecallStateTreeSharedFragment>();
	EntityQuery.AddTagRequirements<EMassFragmentPresence::All>(RequiredTags);
	EntityQuery.AddTagRequirements<EMassFragmentPresence::None>(InvalidTags);
	EntityQuery.AddSubsystemRequirement<URecallSignalSubsystem>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddSubsystemRequirement<URecallRandomNumberSubsystem>(EMassFragmentAccess::ReadWrite);

	ProcessorRequirements.AddSubsystemRequirement<URecallSignalSubsystem>(EMassFragmentAccess::ReadWrite);
}

void URecallStateTreeStartProcessor::SignalEntities(FMassEntityManager& EntityManager, FMassExecutionContext& Context, FRecallSignalNameLookup& EntitySignals)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_StateTree_Tick);

	check(CacheManager.IsValid());
	CacheManager->ResetCache();

	TArray<FMassEntityHandle>& EntitiesToSignal = CacheManager->EntitiesToSignal;

	EntityQuery.ForEachEntityChunk(Context,
		[&EntitiesToSignal](FMassExecutionContext& Context)
	{
		// Keep stats regarding the amount of tree instances ticked per frame
		CSV_CUSTOM_STAT(RecallStateTreeProcessor, NumStartedStateTree, Context.GetNumEntities(), ECsvCustomStatOp::Accumulate);

		const double TimeInSeconds = Recall::Simulation::Utils::GetTimeSeconds(Context.GetWorld());

		FMassEntityManager& EntityManager = Context.GetEntityManagerChecked();
		URecallSignalSubsystem& SignalSystem = Context.GetMutableSubsystemChecked<URecallSignalSubsystem>();
		URecallStateTreeSubsystem& StateTreeSystem = Context.GetMutableSubsystemChecked<URecallStateTreeSubsystem>();
		URecallRandomNumberSubsystem& RandomNumberSystem = Context.GetMutableSubsystemChecked<URecallRandomNumberSubsystem>();

		Recall::StateTree::Behavior::ForEachEntityInChunk(Context, EntityManager, SignalSystem, StateTreeSystem, [TimeInSeconds, &RandomNumberSystem]
		(FRecallStateTreeExecutionContext& StateTreeExecutionContext, FRecallStateTreeInstanceFragment& StateTreeFragment)
		{
			const FRandomStream& RandomStream = RandomNumberSystem.GetRandomStream();
			const int32 RandomSeed = FMath::RoundToInt(static_cast<float>(RAND_MAX) * RandomStream.FRand());;

			StateTreeExecutionContext.Start(FStateTreeExecutionContext::FStartParameters{ .RandomSeed = RandomSeed });
			StateTreeFragment.LastUpdateTimeInSeconds = TimeInSeconds;
		});

		EntitiesToSignal.Append(Context.GetEntities());
	});

	if (EntitiesToSignal.Num() > 0)
	{
		URecallSignalSubsystem& SignalSystem = Context.GetMutableSubsystemChecked<URecallSignalSubsystem>();
		SignalSystem.SignalEntities(Recall::StateTree::Signals::StateTreeActivate, EntitiesToSignal);
		
		Context.Defer().PushCommand<FMassCommandAddTag<FRecallStateTreeRunningTag>>(EntitiesToSignal);
	}
}

//----------------------------------------------------------------------//
// URecallStateTreeUpdateProcessor
//----------------------------------------------------------------------//
URecallStateTreeUpdateProcessor::URecallStateTreeUpdateProcessor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ProcessingPhase = EMassProcessingPhase::PrePhysics;
	ExecutionOrder.ExecuteInGroup = Recall::StateTree::ProcessorGroupNames::StateTreeUpdate;
	ExecutionOrder.ExecuteAfter.Add(Recall::Player::ProcessorGroupNames::Input);
	ExecutionOrder.ExecuteBefore.Add(Recall::Entity::ProcessorGroupNames::AsyncSpawn);
	bRequiresGameThreadExecution = true;
}

struct FRecallStateTreeUpdateCacheManager
{
	TArray<FMassEntityHandle> EntitiesToSignal;

	void ResetCache()
	{
		EntitiesToSignal.Reset();
	}
};

void URecallStateTreeUpdateProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);

	CacheManager = MakeShared<FRecallStateTreeUpdateCacheManager>();

	SubscribeToSignal(Recall::StateTree::Signals::StateTreeStop);
	SubscribeToSignal(Recall::StateTree::Signals::StateTreeActivate);
	SubscribeToSignal(Recall::StateTree::Signals::NewStateTreeTaskRequired);
	SubscribeToSignal(Recall::StateTree::Signals::EventReceived);
	SubscribeToSignal(Recall::StateTree::Signals::TickRequired);

	// Transitions
	SubscribeToSignal(Recall::StateTree::Signals::DelayedTransitionWakeup);

	// Input
	SubscribeToSignal(Recall::Player::Signals::InputPressed);

	// Physics
	SubscribeToSignal(Recall::Physics::Signals::NewOverlap);
	
	// Navigation
	SubscribeToSignal(Recall::Navigation::Signals::EnvQueryDone);
	SubscribeToSignal(Recall::Navigation::Signals::MoveAtAbort);
	SubscribeToSignal(Recall::Navigation::Signals::MoveAtDone);
}

void URecallStateTreeUpdateProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	FMassTagBitSet RequiredTags;
	RequiredTags.Add(FRecallStateTreeRunningTag::StaticStruct());
	RequiredTags.Add(FRecallStateTreeActivatedTag::StaticStruct());
	
	EntityQuery.AddRequirement<FRecallStateTreeInstanceFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddSubsystemRequirement<URecallStateTreeSubsystem>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddConstSharedRequirement<FRecallStateTreeSharedFragment>();
	EntityQuery.AddTagRequirements<EMassFragmentPresence::All>(RequiredTags);
	EntityQuery.AddSubsystemRequirement<URecallSignalSubsystem>(EMassFragmentAccess::ReadWrite);

	ProcessorRequirements.AddSubsystemRequirement<URecallSignalSubsystem>(EMassFragmentAccess::ReadWrite);
}

void URecallStateTreeUpdateProcessor::SignalEntities(FMassEntityManager& EntityManager, FMassExecutionContext& Context, FRecallSignalNameLookup& EntitySignals)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_StateTree_Tick);

	check(CacheManager.IsValid());
	CacheManager->ResetCache();

	TArray<FMassEntityHandle>& EntitiesToSignal = CacheManager->EntitiesToSignal;

	static bool bIsPaused = false;
	const bool bPreviousPause = bIsPaused;

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	// TODO: Move this setting to URecallStateTreeSubsystem so it can be rollback
	// + trigger it from input debug option so it can be replicated between clients
	bIsPaused = bDebugPauseStateTreeExecution;
#endif // UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT

	EntityQuery.ForEachEntityChunk(Context,
		[&EntitySignals, &EntitiesToSignal, bPreviousPause](FMassExecutionContext& Context)
	{
		// Keep stats regarding the amount of tree instances ticked per frame
		CSV_CUSTOM_STAT(RecallStateTreeProcessor, NumTickedStateTree, Context.GetNumEntities(), ECsvCustomStatOp::Accumulate);

		const double TimeInSeconds = Recall::Simulation::Utils::GetTimeSeconds(Context.GetWorld());

		FMassEntityManager& EntityManager = Context.GetEntityManagerChecked();
		URecallSignalSubsystem& SignalSystem = Context.GetMutableSubsystemChecked<URecallSignalSubsystem>();
		URecallStateTreeSubsystem& StateTreeSystem = Context.GetMutableSubsystemChecked<URecallStateTreeSubsystem>();

		const float DeltaTime = Recall::Simulation::Utils::GetDilatedFixedDeltaTime(Context.GetWorld());

		Recall::StateTree::Behavior::ForEachEntityInChunk(Context, EntityManager, SignalSystem, StateTreeSystem, [&DeltaTime, &SignalSystem, TimeInSeconds, &EntitySignals, &EntitiesToSignal, bPreviousPause]
		(FRecallStateTreeExecutionContext& StateTreeExecutionContext, FRecallStateTreeInstanceFragment& StateTreeFragment)
		{
			const FMassEntityHandle Entity = StateTreeExecutionContext.GetEntity();

			// Compute adjusted delta time
			const float AdjustedDeltaTime = FloatCastChecked<float>(TimeInSeconds - StateTreeFragment.LastUpdateTimeInSeconds, /* Precision */ 1. / 256.);
			StateTreeFragment.LastUpdateTimeInSeconds = TimeInSeconds;

			TArray<FName> Signals;
			EntitySignals.GetSignalsForEntity(Entity, Signals);

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
			if (bPreviousPause != bIsPaused)
			{
				if (bIsPaused)
				{
					Signals.Add(Recall::StateTree::Signals::StateTreeStop);
				}
				else if (bPreviousPause != bIsPaused)
				{
					SignalSystem.SignalEntity(Recall::StateTree::Signals::StateTreeStart, Entity);
					return;
				}
			}
#endif // UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT

			if (Signals.Contains(Recall::StateTree::Signals::StateTreeStop)) // Stop
			{
				StateTreeExecutionContext.Stop();
				StateTreeExecutionContext.GetMassExecutionContext().Defer().RemoveTag<FRecallStateTreeRunningTag>(Entity);
				return;
			}
			else
			{
				// Tick the tree instance
				StateTreeExecutionContext.Tick(AdjustedDeltaTime);

				// When last tick status is different than "Running", the state tree need to be tick again
				// For performance reason, tick again to see if we could find a new state right away instead of waiting the next frame.
				if (StateTreeExecutionContext.GetLastTickStatus() != EStateTreeRunStatus::Running)
				{
					StateTreeExecutionContext.Tick(0.0f);

					// Could not find new state yet, try again next frame
					if (StateTreeExecutionContext.GetLastTickStatus() != EStateTreeRunStatus::Running)
					{
						EntitiesToSignal.Add(StateTreeExecutionContext.GetEntity());
					}
				}
			}

#if RECALL_DESYNC_LOG
			RECALL_DESYNC_LOG_INT(StateTreeExecutionContext.GetWorld(), StateTreeEntity, Entity.SerialNumber);

#if WITH_STATETREE_DEBUG
			RECALL_DESYNC_LOG_INT(StateTreeExecutionContext.GetWorld(), StateChangeCount, StateTreeExecutionContext.GetStateChangeCount());
#endif // WITH_STATETREE_DEBUG

			RECALL_DESYNC_LOG_FLOAT(StateTreeExecutionContext.GetWorld(), StateTreeTimeInSeconds, TimeInSeconds);
			RECALL_DESYNC_LOG_FLOAT(StateTreeExecutionContext.GetWorld(), StateTreeAdjustedDeltaTime, AdjustedDeltaTime);
			RECALL_DESYNC_LOG_FLOAT(StateTreeExecutionContext.GetWorld(), StateTreeLastUpdateTimeInSeconds, StateTreeFragment.LastUpdateTimeInSeconds);
			RECALL_DESYNC_LOG_CONTEXT(StateTreeExecutionContext.GetWorld(), StateTreeExecutionContext.GetActiveStateName());
#endif // RECALL_DESYNC_LOG
		});
	});

	if (EntitiesToSignal.Num() > 0)
	{
		URecallSignalSubsystem& SignalSystem = Context.GetMutableSubsystemChecked<URecallSignalSubsystem>();
		SignalSystem.SignalEntities(Recall::StateTree::Signals::NewStateTreeTaskRequired, EntitiesToSignal);
	}
}

//----------------------------------------------------------------------//
// URecallStateTreeDebugRepresentationProcessor
//----------------------------------------------------------------------//
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
static bool bDebugRecallStateTreeOnScreenLog = false;
static FAutoConsoleVariableRef CVarRecallStateTreeOnScreenLog(
	TEXT("recall.AI.DebugStateTreeOnScreen"),
	bDebugRecallStateTreeOnScreenLog,
	TEXT("Display State Tree Log")
);

static bool bDebugRecallStateTreeInWorldLog = false;
static FAutoConsoleVariableRef CVarRecallStateTreeInWorldLog(
	TEXT("recall.AI.DebugStateTreeInWorld"),
	bDebugRecallStateTreeInWorldLog,
	TEXT("Display State Tree Log")
);
#endif // UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT

URecallStateTreeDebugRepresentationProcessor::URecallStateTreeDebugRepresentationProcessor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::All);
	ProcessingPhase = EMassProcessingPhase::Render;
	bRequiresGameThreadExecution = true;
}

void URecallStateTreeDebugRepresentationProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	EntityQuery.AddRequirement<FRecallTransformFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::Optional);
	EntityQuery.AddRequirement<FRecallStateTreeInstanceFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddConstSharedRequirement<FRecallStateTreeSharedFragment>();
	EntityQuery.AddTagRequirement<FRecallStateTreeActivatedTag>(EMassFragmentPresence::All);
	EntityQuery.AddSubsystemRequirement<URecallStateTreeSubsystem>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddSubsystemRequirement<URecallSignalSubsystem>(EMassFragmentAccess::ReadWrite);
#endif // UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
}

void URecallStateTreeDebugRepresentationProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	QUICK_SCOPE_CYCLE_COUNTER(Recall_StateTree_DebugRender);

	if (!bDebugRecallStateTreeOnScreenLog && !bDebugRecallStateTreeInWorldLog)
	{
		return;
	}

	EntityQuery.ForEachEntityChunk(Context, [](FMassExecutionContext& Context)
	{
		const FRecallStateTreeSharedFragment& SharedStateTree = Context.GetConstSharedFragment<FRecallStateTreeSharedFragment>();
		const TObjectPtr<const UStateTree>& StateTree = SharedStateTree.StateTree;
		if (!StateTree)
		{
			return;
		}

		FMassEntityManager& EntityManager = Context.GetEntityManagerChecked();
		URecallSignalSubsystem& SignalSystem = Context.GetMutableSubsystemChecked<URecallSignalSubsystem>();
		URecallStateTreeSubsystem& StateTreeSystem = Context.GetMutableSubsystemChecked<URecallStateTreeSubsystem>();

		const TConstArrayView<FRecallStateTreeInstanceFragment> StateTreeInstanceList = Context.GetFragmentView<FRecallStateTreeInstanceFragment>();
		const TConstArrayView<FRecallTransformFragment> TransformList = Context.GetFragmentView<FRecallTransformFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			const FRecallStateTreeInstanceFragment& StateTreeFragment = StateTreeInstanceList[EntityIndex];
			const FRecallTransformFragment* const TransformFragmentPtr = TransformList.IsValidIndex(EntityIndex) ?
				&TransformList[EntityIndex] : nullptr;

			FString Status;

			FStateTreeInstanceData* InstanceData = StateTreeSystem.GetInstanceData(StateTreeFragment.InstanceHandle);
			if (InstanceData)
			{
				const FRecallStateTreeExecutionContext StateTreeContext(StateTreeSystem, *StateTree, *InstanceData, EntityManager, SignalSystem, Context, Context.GetEntity(EntityIndex));

				Status += FString::Printf(TEXT("  %s\n"), *StateTreeContext.GetStateTree()->GetName());

				Status += FString::Printf(TEXT("  %s\n"), *StateTreeContext.GetActiveStateName());

#if WITH_STATETREE_DEBUG
				Status += FString::Printf(TEXT("  %d\n"), StateTreeContext.GetStateChangeCount());
#endif // WITH_STATETREE_DEBUG
			}
			else
			{
				Status += TEXT("{red}<No StateTree instance>{white}\n");
			}

			if (bDebugRecallStateTreeInWorldLog && TransformFragmentPtr != nullptr)
			{
				DrawDebugString(Context.GetWorld(), TransformFragmentPtr->Position, Status, nullptr, FColor::Green, -1);
			}

			if (bDebugRecallStateTreeOnScreenLog)
			{
				GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Green, Status);
			}
		}
	});
#endif // UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
}
