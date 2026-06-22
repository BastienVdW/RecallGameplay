// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallConversationProcessors.h"

#include "Conversation/RecallConversationInputTypes.h"
#include "Conversation/RecallConversationTypes.h"
#include "MassExecutionContext.h"
#include "Simulation/Conversation/RecallConversationFragments.h"
#include "Simulation/Conversation/RecallConversationSignalTypes.h"
#include "Simulation/GameplayTag/RecallGameplayTagFragments.h"
#include "System/Conversation/RecallConversationSubsystem.h"
#include "System/Entity/RecallEntitySubsystem.h"
#include "System/Input/RecallInputQueueSubsystem.h"
#include "Utility/Conversation/RecallConversationUtils.h"
#include "Utility/Simulation/RecallSimulationUtils.h"

//----------------------------------------------------------------------//
// URecallConversationParticipantSignalProcessor
//----------------------------------------------------------------------//
URecallConversationParticipantSignalProcessor::URecallConversationParticipantSignalProcessor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void URecallConversationParticipantSignalProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);

	SubscribeToSignal(Recall::Conversation::Signals::Callback::OnConversationStart);
	SubscribeToSignal(Recall::Conversation::Signals::Callback::OnConversationEnd);
}

void URecallConversationParticipantSignalProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallConversationParticipantFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FRecallGameplayTagFragment>(EMassFragmentAccess::ReadWrite, EMassFragmentPresence::Optional);
}

void URecallConversationParticipantSignalProcessor::SignalEntities(FMassEntityManager& EntityManager, FMassExecutionContext& Context, FRecallSignalNameLookup& EntitySignals)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_ConversationParticipant_Signal);

	EntityQuery.ForEachEntityChunk(Context, [&EntitySignals](FMassExecutionContext& Context)
	{
		const TArrayView<FRecallConversationParticipantFragment> ParticipantList = Context.GetMutableFragmentView<FRecallConversationParticipantFragment>();
		const TArrayView<FRecallGameplayTagFragment> GameplayTagList = Context.GetMutableFragmentView<FRecallGameplayTagFragment>();

		const bool bAlreadyParticipate = Context.DoesArchetypeHaveTag<FRecallConversationParticipantInsideTag>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			const FMassEntityHandle Entity = Context.GetEntity(EntityIndex);

			FRecallConversationParticipantFragment& ParticipantFragment = ParticipantList[EntityIndex];
			FRecallGameplayTagFragment* const GameplayTagFragmentPtr = GameplayTagList.IsValidIndex(EntityIndex) ?
				&GameplayTagList[EntityIndex] : nullptr;
			
			TArray<FName> Signals;
			EntitySignals.GetSignalsForEntity(Entity, Signals);
						
			if (!bAlreadyParticipate && Signals.Contains(Recall::Conversation::Signals::Callback::OnConversationStart))
			{
				if (GameplayTagFragmentPtr != nullptr)
				{
					GameplayTagFragmentPtr->GameplayTagCountMap.AddTag(Conversation_Participate);	
					GameplayTagFragmentPtr->GameplayTagCountMap.AddTag(State_BlockControl);	
				}

				Context.Defer().AddTag<FRecallConversationParticipantInsideTag>(Entity);
			}
			else if (bAlreadyParticipate && Signals.Contains(Recall::Conversation::Signals::Callback::OnConversationEnd))
			{
				if (GameplayTagFragmentPtr != nullptr)
				{
					GameplayTagFragmentPtr->GameplayTagCountMap.RemoveTag(Conversation_Participate);	
					GameplayTagFragmentPtr->GameplayTagCountMap.RemoveTag(State_BlockControl);	
				}
				
				ParticipantFragment.ConversationHandle.Reset();

				Context.Defer().RemoveTag<FRecallConversationParticipantInsideTag>(Entity);
			}
		}
	});
}

//----------------------------------------------------------------------//
// URecallConversationProcessor
//----------------------------------------------------------------------//
URecallConversationProcessor::URecallConversationProcessor()
	: ActiveEntityQuery(*this)
	, InactiveEntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::All);
	ProcessingPhase = EMassProcessingPhase::FrameEnd;
}

void URecallConversationProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallConversationProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	FMassTagBitSet ActiveRequiredTags;
	ActiveRequiredTags.Add(*FRecallConversationActiveTag::StaticStruct());
	
	ProcessorRequirements.AddSubsystemRequirement<URecallConversationSubsystem>(EMassFragmentAccess::ReadWrite);
	
	ActiveEntityQuery.AddRequirement<FRecallConversationFragment>(EMassFragmentAccess::ReadWrite);
	ActiveEntityQuery.AddTagRequirements<EMassFragmentPresence::All>(ActiveRequiredTags);
	ActiveEntityQuery.AddSubsystemRequirement<URecallInputQueueSubsystem>(EMassFragmentAccess::ReadOnly);
	ActiveEntityQuery.RegisterWithProcessor(*this);
	
	InactiveEntityQuery.AddRequirement<FRecallConversationFragment>(EMassFragmentAccess::ReadOnly);
	InactiveEntityQuery.AddConstSharedRequirement<FRecallConversationConstSharedFragment>();
	InactiveEntityQuery.AddTagRequirements<EMassFragmentPresence::None>(ActiveRequiredTags);
	InactiveEntityQuery.AddSubsystemRequirement<URecallEntitySubsystem>(EMassFragmentAccess::ReadOnly);
	InactiveEntityQuery.RegisterWithProcessor(*this);
}

void URecallConversationProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_Conversation_Execute);

	URecallConversationSubsystem& ConversationSystem = Context.GetMutableSubsystemChecked<URecallConversationSubsystem>();

	ActiveEntityQuery.ForEachEntityChunk(Context, [&ConversationSystem](FMassExecutionContext& Context)
	{
		const uint32 Frame = Recall::Simulation::Utils::GetFrame(Context.GetWorld());
		const int32 FramesPerSeconds = Recall::Simulation::Utils::GetFramesPerSeconds(Context.GetWorld());
		
		const URecallInputQueueSubsystem& InputQueueSystem = Context.GetSubsystemChecked<URecallInputQueueSubsystem>();

		const TArrayView<FRecallConversationFragment> ConversationList = Context.GetMutableFragmentView<FRecallConversationFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			const FMassEntityHandle Entity = Context.GetEntity(EntityIndex);
			
			FRecallConversationFragment& ConversationFragment = ConversationList[EntityIndex];
			if (!ensure(ConversationFragment.ConversationHandle.IsValid()))
			{
				continue;
			}

			// Update the conversation (once per second) to make sure that the UI is synced
			if (Frame % FramesPerSeconds == 0)
			{
				ConversationSystem.RefreshConversation(ConversationFragment.ConversationHandle);
			}

			// Advance conversation based on player choices
			for (const FString& PlayerID : ConversationSystem.GetConversationParticipantPlayers(
					ConversationFragment.ConversationHandle))
			{
				FRecallInput Input;
				if (!InputQueueSystem.GetFrameInput(PlayerID, Input))
				{
					continue;
				}

				const FRecallConversationInputCommand ConversationInput(Input.Options);
				if (!ConversationInput.IsValid())
				{
					continue;
				}
				
				// TODO: Make sure that the player is allowed to input something

				ConversationSystem.AdvanceConversation(ConversationFragment.ConversationHandle,
					ConversationInput.ToChoiceReference());
				if (!ConversationFragment.ConversationHandle.IsValid())
				{
					Context.Defer().RemoveTag<FRecallConversationActiveTag>(Entity);
					break;
				}
			}
		}
	});

	const uint32 Frame = Recall::Simulation::Utils::GetFrame(Context.GetWorld());
	if (Frame == 0)
	{
		ConversationSystem.PushConversationTrigger(Conversation_Trigger_StartGame);
	}
	
	FGameplayTagContainer ConversationTriggers;
	ConversationSystem.PopConversationTriggers(ConversationTriggers);

	if (ConversationTriggers.IsEmpty())
	{
		return;
	}
	
	InactiveEntityQuery.ForEachEntityChunk(Context,
		[&ConversationTriggers](FMassExecutionContext& Context)
	{
		const FRecallConversationConstSharedFragment& ConversationConstSharedFragment = Context.GetConstSharedFragment<FRecallConversationConstSharedFragment>();
		if (!ConversationTriggers.HasAny(ConversationConstSharedFragment.TriggerSettings.ConversationTriggerTags))
		{
			return;
		}
			
		const URecallEntitySubsystem& EntitySystem = Context.GetSubsystemChecked<URecallEntitySubsystem>();
		const TArray<FMassEntityHandle> PlayerEntities = EntitySystem.GetControllerEntities();
			
		const TConstArrayView<FRecallConversationFragment> ConversationList = Context.GetFragmentView<FRecallConversationFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			const FMassEntityHandle ConversationEntity = Context.GetEntity(EntityIndex);
			const FRecallConversationFragment& ConversationFragment = ConversationList[EntityIndex];

			Recall::Conversation::Utils::StartConversation(Context, PlayerEntities,
				ConversationEntity, ConversationFragment.ConversationEntryPoint, ConversationFragment.ConversationEntryIdentifier);
		}
	});
}

//----------------------------------------------------------------------//
// URecallConversationDeinitializer
//----------------------------------------------------------------------//
URecallConversationDeinitializer::URecallConversationDeinitializer()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::All);
	ObservedType = FRecallConversationFragment::StaticStruct();
	Operation = EMassObservedOperation::Remove;
}

void URecallConversationDeinitializer::InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallConversationDeinitializer::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallConversationFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddSubsystemRequirement<URecallConversationSubsystem>(EMassFragmentAccess::ReadWrite);
}

void URecallConversationDeinitializer::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_Conversation_Deinitialize);
	
	EntityQuery.ForEachEntityChunk(Context, [](FMassExecutionContext& Context)
	{
		URecallConversationSubsystem& ConversationSystem = Context.GetMutableSubsystemChecked<URecallConversationSubsystem>();

		const TArrayView<FRecallConversationFragment> ConversationList = Context.GetMutableFragmentView<FRecallConversationFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			FRecallConversationFragment& ConversationFragment = ConversationList[EntityIndex];			
			ConversationSystem.ReleaseConversationInstance(ConversationFragment.ConversationHandle);
		}
	});
}
