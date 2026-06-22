// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallInteractProcessors.h"

#include "Desync/RecallDesyncLog.h"
#include "Interact/RecallInteractProgressReactInterface.h"
#include "Interact/RecallInteractReactInterface.h"
#include "MassExtendedExecutionContext.h"
#include "RecallSignalSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Simulation/Interact/RecallInteractFragments.h"
#include "Simulation/Interact/RecallInteractSignalTypes.h"
#include "Simulation/Physics/RecallPhysicsSignalTypes.h"
#include "Simulation/Player/RecallPlayerProcessorGroupTypes.h"
#include "Simulation/Controller/RecallControllerFragments.h"
#include "Simulation/Physics/RecallPhysicsSensorFragment.h"
#include "Simulation/StateTree/RecallStateTreeFragments.h"
#include "Simulation/StateTree/RecallStateTreeProcessorGroupTypes.h"
#include "Simulation/StateTree/RecallStateTreeSignalTypes.h"
#include "Simulation/Transform/RecallTransformFragments.h"
#include "System/Actor/RecallActorSubsystem.h"
#include "System/AI/RecallStateTreeSubsystem.h"
#include "System/Input/RecallInputQueueSubsystem.h"
#include "System/Representation/RecallRepresentationEventSubsystem.h"
#include "Utility/Interact/RecallInteractSearchUtils.h"
#include "Utility/Interact/RecallInteractRepresentationUtils.h"
#include "Utility/Player/RecallPlayerUtils.h"

//----------------------------------------------------------------------//
// URecallInteractorSignalProcessor
//----------------------------------------------------------------------//
URecallInteractorSignalProcessor::URecallInteractorSignalProcessor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ProcessingPhase = EMassExtendedProcessingPhase::PrePhysics;
	ExecutionOrder.ExecuteBefore.Add(Recall::StateTree::ProcessorGroupNames::StateTreeUpdate);
	bRequiresGameThreadExecution = true;
}

void URecallInteractorSignalProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);

	SubscribeToSignal(Recall::Physics::Signals::OverlapBegin);
	SubscribeToSignal(Recall::Physics::Signals::OverlapEnd);
	SubscribeToSignal(Recall::Interact::Signals::OverlapTick);
}

void URecallInteractorSignalProcessor::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallTransformFragment>(EMassExtendedFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FRecallInteractorFragment>(EMassExtendedFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FRecallPhysicsSensorFragment>(EMassExtendedFragmentAccess::ReadOnly);
	EntityQuery.AddConstSharedRequirement<FRecallInteractorSharedFragment>();
	EntityQuery.AddSubsystemRequirement<URecallSignalSubsystem>(EMassExtendedFragmentAccess::ReadWrite);
}

void URecallInteractorSignalProcessor::SignalEntities(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context, FRecallSignalNameLookup& EntitySignals)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_Interactor_Signal);

	EntityQuery.ForEachEntityChunk(Context, [&EntitySignals](FMassExtendedExecutionContext& Context)
	{
		URecallSignalSubsystem& SignalSystem = Context.GetMutableSubsystemChecked<URecallSignalSubsystem>();

		const FRecallInteractorSharedFragment& InteractorSharedFragment = Context.GetConstSharedFragment<FRecallInteractorSharedFragment>();

		const TConstArrayView<FRecallPhysicsSensorFragment> SensorList = Context.GetFragmentView<FRecallPhysicsSensorFragment>();
		const TConstArrayView<FRecallTransformFragment> TransformList = Context.GetFragmentView<FRecallTransformFragment>();

		const TArrayView<FRecallInteractorFragment> InteractorList = Context.GetMutableFragmentView<FRecallInteractorFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			const FMassExtendedEntityHandle Entity = Context.GetEntity(EntityIndex);

			const FRecallTransformFragment& TransformFragment = TransformList[EntityIndex];
			const FRecallPhysicsSensorFragment& SensorFragment = SensorList[EntityIndex];

			FRecallInteractorFragment& InteractorFragment = InteractorList[EntityIndex];
			InteractorFragment.ContextualInteractEntity = Entity; // Use self as contextual interactable entity

			TArray<FName> Signals;
			EntitySignals.GetSignalsForEntity(Entity, Signals);

			if (Signals.Contains(Recall::Physics::Signals::OverlapEnd))
			{
				InteractorFragment.ClosestInteractableEntity.Reset();
			}
			else if (Signals.Contains(Recall::Interact::Signals::OverlapTick))
			{
				const FRecallUpdateClosestInteractableEntityContext UpdateClosestInteractableEntityContext
				{
					Context, Entity, TransformFragment, SensorFragment, InteractorFragment, InteractorSharedFragment
				};

				Recall::Interact::Search::Utils::UpdateClosestInteractableEntity(UpdateClosestInteractableEntityContext);

				SignalSystem.DelaySignalEntity(Recall::Interact::Signals::OverlapTick, Entity, RECALL_INTERACT_OVERLAP_UPDATE_RATE);
			}
			else if (Signals.Contains(Recall::Physics::Signals::OverlapBegin))
			{
				SignalSystem.SignalEntity(Recall::Interact::Signals::OverlapTick, Entity);
			}
		}
	});
}

//----------------------------------------------------------------------//
// URecallInteractorInputProcessor
//----------------------------------------------------------------------//
URecallInteractorInputProcessor::URecallInteractorInputProcessor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EExtendedProcessorExecutionFlags::All);
	ProcessingPhase = EMassExtendedProcessingPhase::PrePhysics;
	ExecutionOrder.ExecuteAfter.Add(Recall::Player::ProcessorGroupNames::Input);
	ExecutionOrder.ExecuteBefore.Add(Recall::StateTree::ProcessorGroupNames::StateTreeUpdate);
}

void URecallInteractorInputProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallInteractorInputProcessor::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallInteractorFragment>(EMassExtendedFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FRecallStateTreeInstanceFragment>(EMassExtendedFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FRecallControllerFragment>(EMassExtendedFragmentAccess::ReadOnly);
	EntityQuery.AddSubsystemRequirement<URecallInputQueueSubsystem>(EMassExtendedFragmentAccess::ReadOnly);
	EntityQuery.AddSubsystemRequirement<URecallStateTreeSubsystem>(EMassExtendedFragmentAccess::ReadWrite);
	EntityQuery.AddSubsystemRequirement<URecallSignalSubsystem>(EMassExtendedFragmentAccess::ReadWrite);
	EntityQuery.RegisterWithProcessor(*this);	
}

void URecallInteractorInputProcessor::Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_InteractorInput_Execute);

	EntityQuery.ForEachEntityChunk(Context, [](FMassExtendedExecutionContext& Context)
	{
		const URecallInputQueueSubsystem& InputQueueSystem = Context.GetSubsystemChecked<URecallInputQueueSubsystem>();
		
		URecallStateTreeSubsystem& StateTreeSystem = Context.GetMutableSubsystemChecked<URecallStateTreeSubsystem>();
		URecallSignalSubsystem& SignalSystem = Context.GetMutableSubsystemChecked<URecallSignalSubsystem>();

		const TConstArrayView<FRecallInteractorFragment> InteractorList = Context.GetFragmentView<FRecallInteractorFragment>();
		const TConstArrayView<FRecallControllerFragment> PlayerControllerList = Context.GetFragmentView<FRecallControllerFragment>();
		const TConstArrayView<FRecallStateTreeInstanceFragment> StateTreeInstanceList = Context.GetFragmentView<FRecallStateTreeInstanceFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			const FMassExtendedEntityHandle Entity = Context.GetEntity(EntityIndex);

			const FRecallInteractorFragment& InteractorFragment = InteractorList[EntityIndex];
			const FRecallControllerFragment& PlayerControllerFragment = PlayerControllerList[EntityIndex];
			const FRecallStateTreeInstanceFragment& StateTreeInstanceFragment = StateTreeInstanceList[EntityIndex];

			FRecallInput Input;
			if (!InputQueueSystem.GetFrameInput(PlayerControllerFragment.ControllerID, Input))
			{
				continue;
			}

			auto SendStateTreeEvent = [&SignalSystem, &Entity, &InteractorFragment,
				&StateTreeSystem, &StateTreeInstanceFragment](const FGameplayTag& Tag)
			{
				FStateTreeEvent Event;
				Event.Payload = FInstancedStruct::Make(InteractorFragment.ContextualInteractEntity);
				Event.Tag = Tag;
				Event.Origin = TEXT("RecallInteractorInputProcessor");

				StateTreeSystem.SendStateTreeEvent(StateTreeInstanceFragment.InstanceHandle, Event);
				
				SignalSystem.SignalEntity(Recall::StateTree::Signals::TickRequired, Entity);
				
#if RECALL_DESYNC_LOG
				RECALL_DESYNC_LOG_STR(SignalSystem.GetWorld(), ContextualInteractEntity,
					InteractorFragment.ContextualInteractEntity.DebugGetDescription());
				RECALL_DESYNC_LOG_STR(SignalSystem.GetWorld(), Entity, Entity.DebugGetDescription());
				RECALL_DESYNC_LOG_STR(SignalSystem.GetWorld(), Tag, Tag.ToString());
#endif // RECALL_DESYNC_LOG
			};
			
			if (UGameplayStatics::HasOption(Input.Options, Recall::Input::Option::InteractContextualPrimary))
			{				
				SendStateTreeEvent(StateTreeEvent_Interact_Contextual_Primary);
			}
			
			if (UGameplayStatics::HasOption(Input.Options, Recall::Input::Option::InteractContextualSecondary))
			{				
				SendStateTreeEvent(StateTreeEvent_Interact_Contextual_Secondary);
			}
			
			if (UGameplayStatics::HasOption(Input.Options, Recall::Input::Option::InteractContextualTertiary))
			{				
				SendStateTreeEvent(StateTreeEvent_Interact_Contextual_Tertiary);
			}
			
			if (UGameplayStatics::HasOption(Input.Options, Recall::Input::Option::InteractContextualQuaternary))
			{				
				SendStateTreeEvent(StateTreeEvent_Interact_Contextual_Quaternary);
			}
		}
	});
}

//----------------------------------------------------------------------//
// URecallInteractorRepresentationEventProcessor
//----------------------------------------------------------------------//
URecallInteractorRepresentationEventProcessor::URecallInteractorRepresentationEventProcessor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EExtendedProcessorExecutionFlags::All);
	ProcessingPhase = EMassExtendedProcessingPhase::FrameEnd;
}

struct FRecallInteractorRepresentationCacheManager
{
	TArray<FRecallInteractProgress> Interactions;

	void ResetCache()
	{
		Interactions.Reset();
	}
};

void URecallInteractorRepresentationEventProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);

	CacheManager = MakeShared<FRecallInteractorRepresentationCacheManager>();
}

void URecallInteractorRepresentationEventProcessor::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallInteractorFragment>(EMassExtendedFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FRecallControllerFragment>(EMassExtendedFragmentAccess::ReadOnly, EMassExtendedFragmentPresence::Optional);
	EntityQuery.AddSubsystemRequirement<URecallRepresentationEventSubsystem>(EMassExtendedFragmentAccess::ReadWrite);
	EntityQuery.AddSubsystemRequirement<URecallActorSubsystem>(EMassExtendedFragmentAccess::ReadOnly);
	
	ProcessorRequirements.AddSubsystemRequirement<URecallRepresentationEventSubsystem>(EMassExtendedFragmentAccess::ReadWrite);
}

void URecallInteractorRepresentationEventProcessor::Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_InteractorRepresentation_Execute);

	check(CacheManager.IsValid());
	CacheManager->ResetCache();
	
	TArray<FRecallInteractProgress>& Interactions = CacheManager->Interactions;
	
	EntityQuery.ForEachEntityChunk(Context, [&Interactions](FMassExtendedExecutionContext& Context)
	{
		URecallRepresentationEventSubsystem& RepresentationEventSystem = Context.GetMutableSubsystemChecked<URecallRepresentationEventSubsystem>();
		const URecallActorSubsystem& ActorSubsystem = Context.GetSubsystemChecked<URecallActorSubsystem>();

		const TConstArrayView<FRecallControllerFragment> PlayerControllerList = Context.GetFragmentView<FRecallControllerFragment>();
		const TConstArrayView<FRecallInteractorFragment> InteractorList = Context.GetFragmentView<FRecallInteractorFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			const FMassExtendedEntityHandle Entity = Context.GetEntity(EntityIndex);
			const FRecallControllerFragment* PlayerControllerFragmentPtr =
				PlayerControllerList.IsValidIndex(EntityIndex) ? &PlayerControllerList[EntityIndex] : nullptr;
			const FRecallInteractorFragment& InteractorFragment = InteractorList[EntityIndex];

			const FRecallInteractRepresentationContext RepresentationContext{ Context, ActorSubsystem };
			const FRecallInteractState State = Recall::Interact::Representation::Utils::GetInteractInfo(RepresentationContext, Entity, InteractorFragment);

			int32 LocalPlayerIndex = INDEX_NONE;
			
			// Update local interact button
			if (PlayerControllerFragmentPtr != nullptr)
			{
				if (Recall::Player::Utils::FindLocalPlayerIndex(Context.GetWorld(), PlayerControllerFragmentPtr->ControllerID, LocalPlayerIndex))
				{
					RepresentationEventSystem.PushObserverEvent<IRecallInteractReactInterface>([State, LocalPlayerIndex](auto& Observer)
					{
						Observer.Interface.SetInteraction(State, LocalPlayerIndex);
					});
				}
			}
			// Show progress for non player entities
			else if (State.Progress.IsInProgress() && !State.Progress.bIsContextual && !State.Progress.bHideProgress)
			{
				FRecallInteractProgress& Interaction = Interactions.AddDefaulted_GetRef();
				Interaction.ID = Entity.SerialNumber;
				Interaction.Location = State.InteractableLocation;
				Interaction.Progress = InteractorFragment.InteractionProgress;
			}
		}
	});

	if (Interactions.Num() > 0)
	{
		URecallRepresentationEventSubsystem& RepresentationEventSystem = Context.GetMutableSubsystemChecked<URecallRepresentationEventSubsystem>();
		RepresentationEventSystem.PushObserverEvent<IRecallInteractProgressReactInterface>([Interactions](auto& Observer)
		{
			Observer.Interface.SetInteractions(Interactions);
		});
	}		
}
