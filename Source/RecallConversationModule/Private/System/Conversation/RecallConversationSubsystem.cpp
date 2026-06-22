// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "System/Conversation/RecallConversationSubsystem.h"

#include "Actor/Conversation/RecallConversationParticipantActor.h"
#include "Conversation/RecallConversationHandle.h"
#include "Conversation/RecallConversationTypes.h"
#include "ConversationContext.h"
#include "ConversationDatabase.h"
#include "ConversationInstance.h"
#include "ConversationRegistry.h"
#include "ConversationSettings.h"
#include "ConversationTaskNode.h"
#include "Desync/RecallDesyncLog.h"
#include "RecallSignalSubsystem.h"
#include "Simulation/Conversation/RecallConversationSignalTypes.h"
#include "System/Conversation/RecallConversationRegistryTypes.h"
#include "System/Random/RecallRandomNumberSubsystem.h"
#include "Utility/Simulation/RecallSimulationUtils.h"

void URecallConversationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	Collection.InitializeDependency<URecallRandomNumberSubsystem>();
	Collection.InitializeDependency<URecallSignalSubsystem>();

	RandomNumberSystem = UWorld::GetSubsystem<URecallRandomNumberSubsystem>(GetWorld());
	SignalSystem = UWorld::GetSubsystem<URecallSignalSubsystem>(GetWorld());
	
	UClass* InstanceClass = GetDefault<UConversationSettings>()->GetConversationInstanceClass();
	if (InstanceClass == nullptr)
	{
		InstanceClass = UConversationInstance::StaticClass();
	}

	ConversationRegistry.FreeInstanceIndices.Reserve(RECALL_CONVERSATION_POOL_SIZE);
	ConversationRegistry.Instances.SetNum(RECALL_CONVERSATION_POOL_SIZE);
	
	for (int32 InstanceIndex = 0; InstanceIndex < ConversationRegistry.Instances.Num(); InstanceIndex++)
	{
		ConversationRegistry.Instances[InstanceIndex].ConversationInstance = NewObject<UConversationInstance>(this, InstanceClass);
	}
	
	FWorldDelegates::OnWorldInitializedActors.AddUObject(this, &ThisClass::OnActorsInitialized);
}

void URecallConversationSubsystem::Deinitialize()
{
	Super::Deinitialize();
	
	for (int32 InstanceIndex = 0; InstanceIndex < ConversationRegistry.InstanceCount; InstanceIndex++)
	{
		ConversationRegistry.Instances[InstanceIndex].ReleaseConversation(ConversationRegistry.ParticipanActors);
	}
	ConversationRegistry.Instances.Empty();
	ConversationRegistry.ParticipanActors.Empty();
	
	FWorldDelegates::OnWorldInitializedActors.RemoveAll(this);
}

void URecallConversationSubsystem::OnActorsInitialized(const FActorsInitializedParams& Params)
{
	ConversationRegistry.ParticipanActors.SetNum(RECALL_CONVERSATION_PARTICIPANT_POOL_SIZE);
	
	for (int32 ActorIndex = 0; ActorIndex < RECALL_CONVERSATION_PARTICIPANT_POOL_SIZE; ActorIndex++)
	{
		ConversationRegistry.ParticipanActors[ActorIndex] = Params.World->SpawnActor<ARecallConversationParticipantActor>();
	}
}

void URecallConversationSubsystem::Reset()
{
	ConversationRegistry.SerialNumberGenerator = 0;

	for (int32 InstanceIndex = 0; InstanceIndex < ConversationRegistry.InstanceCount; InstanceIndex++)
	{
		ConversationRegistry.Instances[InstanceIndex].ReleaseConversation(ConversationRegistry.ParticipanActors);
	}
	
	ConversationRegistry.InstanceCount = 0;
	ConversationRegistry.FreeInstanceIndices.Reset();
}

void URecallConversationSubsystem::Save(const FRecallSnapshotContext& Context, FInstancedStruct& OutSnapshot)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR(TEXT("URecallConversationSubsystem::Save"));
	QUICK_SCOPE_CYCLE_COUNTER(Recall_Conversation_Save);

	FScopeLock Lock(&DataGuard);
	
	OutSnapshot.InitializeAs<FRecallConversationSnapshot>(ConversationRegistry);
}

void URecallConversationSubsystem::Restore(const FRecallSnapshotContext& Context, const FInstancedStruct& InSnapshot)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR(TEXT("URecallConversationSubsystem::Restore"));
	QUICK_SCOPE_CYCLE_COUNTER(Recall_Conversation_Restore);

	FScopeLock Lock(&DataGuard);
	
	if (const FRecallConversationSnapshot* DataPtr = InSnapshot.GetPtr<FRecallConversationSnapshot>())
	{
		DataPtr->Restore(ConversationRegistry);
	}
}

FRecallConversationHandle URecallConversationSubsystem::CreateConversationInstance(const FMassEntityHandle& Entity)
{	
	Recall::Simulation::Utils::CheckSimulationProcessingPhase(this);
	
	FScopeLock Lock(&DataGuard);
	
	if (!ensure(ConversationRegistry.FreeInstanceIndices.Num()
		|| ConversationRegistry.InstanceCount < RECALL_CONVERSATION_POOL_SIZE))
	{
		UE_LOG(LogRecallConversation, Error,
			TEXT("%hs Please increase the size of the conversation pool, or release existing handles"), __FUNCTION__);
		return FRecallConversationHandle::Invalid();
	}
	
	int32 Index = INDEX_NONE;
	if (ConversationRegistry.FreeInstanceIndices.Num())
	{
		Index = ConversationRegistry.FreeInstanceIndices.Pop(EAllowShrinking::No);
	}
	else
	{
		Index = ConversationRegistry.InstanceCount++;
	}
	
	ConversationRegistry.Instances[Index].MutableData.Entity = Entity;

	const FRecallConversationHandle NewHandle(++ConversationRegistry.SerialNumberGenerator, Index);
	return NewHandle;
}

void URecallConversationSubsystem::ReleaseConversationInstance(FRecallConversationHandle& Handle)
{
	Recall::Simulation::Utils::CheckSimulationProcessingPhase(this);
	
	if (!Handle.IsValid())
	{
		return;
	}
	
	{
		FScopeLock Lock(&DataGuard);
	
		CheckIfHandleIsValid(Handle);		

		FRecallConversationInstanceEntry& Instance = ConversationRegistry.Instances[Handle.Index];

		if (Instance.MutableData.bStarted)
		{
			const TArray<FMassEntityHandle> ParticipantEntities = Instance.MutableData.GetParticipantEntities();
			GetSignalSystemChecked().SignalEntities(
				Recall::Conversation::Signals::Callback::OnConversationEnd, ParticipantEntities);
		}
		
		Instance.ReleaseConversation(ConversationRegistry.ParticipanActors);
		
		ConversationRegistry.FreeInstanceIndices.Add(Handle.Index);
	}

	Handle.Reset();
}

bool URecallConversationSubsystem::CanStartConversation(const FMassEntityHandle& ConversationEntity,
	const FGameplayTag& EntryPoint, const FString& EntryIdentifier, const TObjectPtr<const UConversationDatabase>& Graph,
	const FGameplayTag& ParticipantID, const FMassEntityHandle& ParticipantEntity, FString ParticipantPlayerID)
{
	FRecallConversationHandle Handle = CreateConversationInstance(ConversationEntity);
	if (!Handle.IsValid())
	{
		return false;
	}

	AddConversationParticipant(Handle, ParticipantID, ParticipantEntity, ParticipantPlayerID);

	auto EvaluateEntryNodesRequirement = [this](const FRecallConversationHandle& Handle,
		const FGameplayTag& EntryPoint, const FString& EntryIdentifier, const TObjectPtr<const UConversationDatabase>& Graph,
		EConversationRequirementResult MaximumRequirementResult = EConversationRequirementResult::Passed)
	{
		FScopeLock Lock(&DataGuard);
	
		CheckIfHandleIsValid(Handle);
		
		const FRecallConversationInstanceEntry& Instance = ConversationRegistry.Instances[Handle.Index];
		const TObjectPtr<UConversationInstance>& ConversationInstance = Instance.ConversationInstance;
		if (!ConversationInstance)
		{
			return false;
		}
		
		const UConversationRegistry* ConversationRegistryPtr = UConversationRegistry::GetFromWorld(GetWorld());
		if (ConversationRegistryPtr == nullptr)
		{
			return false;
		}
			
		const TArray<FGuid> PotentialStartingPoints = ConversationRegistryPtr->GetOutputLinkGUIDs(Graph, EntryPoint, EntryIdentifier);
		const FConversationContext Context = FConversationContext::CreateServerContext(ConversationInstance, nullptr);
		
		for (const FGuid& TestGUID : PotentialStartingPoints)
		{
			const UConversationNode* TestNode = Context.GetConversationRegistry().GetRuntimeNodeFromGUID(TestGUID, Graph.Get());
			if (const UConversationTaskNode* TaskNode = Cast<UConversationTaskNode>(TestNode))
			{
				const EConversationRequirementResult RequirementResult = TaskNode->CheckRequirements(Context);

				if (static_cast<int64>(RequirementResult) <= static_cast<int64>(MaximumRequirementResult))
				{
					UE_LOG(LogRecallConversation, Verbose, TEXT("\t%s is legal"), *TestGUID.ToString());
					return true;
				}
			}
		}
		return false;
	};	
	const bool bCanStart = EvaluateEntryNodesRequirement(Handle, EntryPoint, EntryIdentifier, Graph);

	RemoveConversationParticipant(Handle, ParticipantID);

	ReleaseConversationInstance(Handle);

	return bCanStart;
}

void URecallConversationSubsystem::CheckIfHandleIsValid(const FRecallConversationHandle& Handle) const
{
	check(Handle.IsValid());
	check(ConversationRegistry.Instances.IsValidIndex(Handle.Index)
		&& !ConversationRegistry.FreeInstanceIndices.Contains(Handle.Index));
}

int32 URecallConversationSubsystem::GetNextSeedChecked() const
{
	check(RandomNumberSystem.IsValid());
	return FMath::RoundToInt(static_cast<float>(RAND_MAX) * RandomNumberSystem->GetRandomStream().FRand());
}

class URecallSignalSubsystem& URecallConversationSubsystem::GetSignalSystemChecked() const
{
	check(SignalSystem.IsValid());
	return *SignalSystem.Get();
}

void URecallConversationSubsystem::AddConversationParticipant(const FRecallConversationHandle& Handle,
                                                                const FGameplayTag& ParticipantID, const FMassEntityHandle& Entity, FString PlayerID)
{
	Recall::Simulation::Utils::CheckSimulationProcessingPhase(this);
	
	if (!Handle.IsValid())
	{
		return;
	}
	
	FScopeLock Lock(&DataGuard);
	
	CheckIfHandleIsValid(Handle);

	FRecallConversationInstanceEntry& Instance = ConversationRegistry.Instances[Handle.Index];
	if (!ensureAlwaysMsgf(!Instance.MutableData.Participants.Contains(ParticipantID),
		TEXT("%hs Participant was already added to conversation"), __FUNCTION__))
	{
		return;
	}
	
	const TWeakObjectPtr<ARecallConversationParticipantActor> ParticipantActor = Cast<ARecallConversationParticipantActor>(
		ConversationRegistry.ParticipanActors.Pop(EAllowShrinking::No));
	if (!ParticipantActor.IsValid())
	{
		UE_LOG(LogRecallConversation, Error,
			TEXT("%hs Increase participant pool size or release conversations"), __FUNCTION__)
		return;
	}

	FRecallConversationParticipant& NewParticipant = Instance.MutableData.Participants.Add(ParticipantID);
	NewParticipant.Entity = Entity;
	NewParticipant.PlayerID = PlayerID;
	
	ParticipantActor->SetConversationParticipant(Instance.MutableData.Entity, ParticipantID, Entity, PlayerID);
	
	if (const TObjectPtr<UConversationInstance>& ConversationInstance = Instance.ConversationInstance)
	{
		ConversationInstance->ServerAssignParticipant(ParticipantID, ParticipantActor.Get());

		if (Instance.MutableData.bStarted)
		{
			ConversationInstance->ServerRefreshCurrentConversationNode();

			GetSignalSystemChecked().SignalEntity(
				Recall::Conversation::Signals::Callback::OnConversationStart, Entity);
			
			// TODO: Participant enter event
		}
		
#if RECALL_DESYNC_LOG
		RECALL_DESYNC_LOG_STR(this, AddConversationParticipant, ParticipantID.ToString());
#endif // RECALL_DESYNC_LOG
	}
}

void URecallConversationSubsystem::RemoveConversationParticipant(const FRecallConversationHandle& Handle,
	const FGameplayTag& ParticipantID)
{
	Recall::Simulation::Utils::CheckSimulationProcessingPhase(this);
	
	if (!Handle.IsValid())
	{
		return;
	}
	
	FScopeLock Lock(&DataGuard);
	
	CheckIfHandleIsValid(Handle);

	FRecallConversationInstanceEntry& Instance = ConversationRegistry.Instances[Handle.Index];
	
	FRecallConversationParticipant Participant;
	if (!ensureAlwaysMsgf(Instance.MutableData.Participants.RemoveAndCopyValue(ParticipantID, Participant),
		TEXT("%hs Participant already left the conversation"), __FUNCTION__))
	{
		return;
	}

	if (const TObjectPtr<UConversationInstance>& ConversationInstance = Instance.ConversationInstance)
	{
		const FConversationParticipantEntry* ParticipantEntryPtr = ConversationInstance->GetParticipant(ParticipantID);
		if (!ensureAlwaysMsgf(ParticipantEntryPtr != nullptr,
			TEXT("%hs Participant was already removed or never added"), __FUNCTION__))
		{
			return;
		}

		ConversationRegistry.ParticipanActors.Add(ParticipantEntryPtr->Actor);
		
		ConversationInstance->ServerRemoveParticipant(ParticipantID, ConversationInstance->GetParticipantsCopy());

		if (Instance.MutableData.bStarted)
		{
			GetSignalSystemChecked().SignalEntity(
				Recall::Conversation::Signals::Callback::OnConversationEnd, Participant.Entity);

			// TODO: Participant leave event
		}
		
#if RECALL_DESYNC_LOG
		RECALL_DESYNC_LOG_STR(this, RemoveConversationParticipant, ParticipantID.ToString());
#endif // RECALL_DESYNC_LOG
	}
}

TArray<FString> URecallConversationSubsystem::GetConversationParticipantPlayers(
	const FRecallConversationHandle& Handle) const
{
	TArray<FString> Results;
	
	if (!Handle.IsValid())
	{
		return Results;
	}
	
	FScopeLock Lock(&DataGuard);
	
	CheckIfHandleIsValid(Handle);

	const FRecallConversationInstanceEntry& Instance = ConversationRegistry.Instances[Handle.Index];

	for (const TPair<FGameplayTag, FRecallConversationParticipant>& ParticipantEntry : Instance.MutableData.Participants)
	{
		if (!ParticipantEntry.Value.PlayerID.IsEmpty())
		{
			Results.Add(ParticipantEntry.Value.PlayerID);
		}
	}

	return Results;
}

bool URecallConversationSubsystem::StartConversation(FRecallConversationHandle& Handle,
		const FGameplayTag& EntryPoint, const FString& EntryPointIdentifier, const TObjectPtr<const UConversationDatabase>& Graph)
{
	Recall::Simulation::Utils::CheckSimulationProcessingPhase(this);
	
	if (!Handle.IsValid())
	{
		return false;
	}
	
	if (!ensureAlwaysMsgf(Graph,
		TEXT("%hs Invalid graph"), __FUNCTION__))
	{
		ReleaseConversationInstance(Handle);
		return false;
	}
	
	FScopeLock Lock(&DataGuard);

	CheckIfHandleIsValid(Handle);
	
	FRecallConversationInstanceEntry& Instance = ConversationRegistry.Instances[Handle.Index];
	if (!ensureAlwaysMsgf(!Instance.MutableData.bStarted,
		TEXT("%hs Conversation already started"), __FUNCTION__))
	{
		return false;
	}

	Instance.MutableData.bStarted = true;
	Instance.MutableData.ActiveConversationGraph = Graph;
	Instance.MutableData.EntryPoint = EntryPoint;
	Instance.MutableData.EntryPointIdentifier = EntryPointIdentifier;
	Instance.MutableData.InitialSeed = GetNextSeedChecked();

	if (const TObjectPtr<UConversationInstance>& ConversationInstance = Instance.ConversationInstance)
	{
		const TArray<FConversationParticipantEntry> ParticipantEntries = ConversationInstance->GetParticipantListCopy();
		
		Instance.SetConversationSeed(Instance.MutableData.InitialSeed);
		ConversationInstance->ServerStartConversation(EntryPoint, Graph, EntryPointIdentifier);
		
		if (ConversationInstance->GetCurrentNodeHandle().IsValid())
		{
			const TArray<FMassEntityHandle> ParticipantEntities = Instance.MutableData.GetParticipantEntities();
			
			GetSignalSystemChecked().SignalEntities(
				Recall::Conversation::Signals::Callback::OnConversationStart, ParticipantEntities);
					
#if RECALL_DESYNC_LOG
			RECALL_DESYNC_LOG_STR(this, StartConversation, Graph->GetFName().ToString());
#endif // RECALL_DESYNC_LOG
			
			return true;
		}
		else
		{
			UE_LOG(LogRecallConversation, Log,
				TEXT("%hs Conversation ended right away (this may be intentional)"), __FUNCTION__);
			
			check(ConversationInstance->GetParticipantListCopy().IsEmpty());
			for (const FConversationParticipantEntry& ParticipantEntry : ParticipantEntries)
			{
				ConversationRegistry.ParticipanActors.Add(ParticipantEntry.Actor);
			}
			
			ReleaseConversationInstance(Handle);
		}
	}
	
	return false;
}

void URecallConversationSubsystem::AdvanceConversation(FRecallConversationHandle& Handle,
	const FConversationChoiceReference& ChoiceReference)
{
	Recall::Simulation::Utils::CheckSimulationProcessingPhase(this);
	
	if (!Handle.IsValid())
	{
		return;
	}

	FScopeLock Lock(&DataGuard);
	
	CheckIfHandleIsValid(Handle);

	const int32 Seed = GetNextSeedChecked();
	
	FRecallConversationInstanceEntry& Instance = ConversationRegistry.Instances[Handle.Index];
	Instance.MutableData.Choices.Add(FRecallConversationChoice{ Seed, ChoiceReference });

	if (const TObjectPtr<UConversationInstance>& ConversationInstance = Instance.ConversationInstance)
	{
		const TArray<FMassEntityHandle> ParticipantEntities = Instance.MutableData.GetParticipantEntities();
		const TArray<FConversationParticipantEntry> ParticipantEntries = ConversationInstance->GetParticipantListCopy();
		
		Instance.SetConversationSeed(Seed);
		ConversationInstance->ServerAdvanceConversation(FAdvanceConversationRequest(ChoiceReference));

		// End conversation
		if (!Instance.ConversationInstance->GetCurrentNodeHandle().IsValid())
		{
			check(ConversationInstance->GetParticipantListCopy().IsEmpty());
			for (const FConversationParticipantEntry& ParticipantEntry : ParticipantEntries)
			{
				ConversationRegistry.ParticipanActors.Add(ParticipantEntry.Actor);
			}
			
			ReleaseConversationInstance(Handle);
		}
	}
}

void URecallConversationSubsystem::RefreshConversation(const FRecallConversationHandle& Handle) const
{
	Recall::Simulation::Utils::CheckSimulationProcessingPhase(this);
	
	if (!Handle.IsValid())
	{
		return;
	}
	
	FScopeLock Lock(&DataGuard);
	
	CheckIfHandleIsValid(Handle);

	const FRecallConversationInstanceEntry& Instance = ConversationRegistry.Instances[Handle.Index];
	if (!Instance.MutableData.bStarted)
	{
		return;
	}

	if (const TObjectPtr<UConversationInstance>& ConversationInstance = Instance.ConversationInstance)
	{
		ConversationInstance->ServerRefreshCurrentConversationNode();
	}
}

void URecallConversationSubsystem::PushConversationTrigger(const FGameplayTag& TriggerTag)
{
	PushConversationTriggers(TriggerTag.GetSingleTagContainer());
}

void URecallConversationSubsystem::PushConversationTriggers(const FGameplayTagContainer& TriggerTags)
{
	ConversationRegistry.ConversationTriggerQueue.AppendTags(TriggerTags);
}

void URecallConversationSubsystem::PopConversationTriggers(FGameplayTagContainer& OutTriggerTags)
{
	OutTriggerTags.AppendTags(ConversationRegistry.ConversationTriggerQueue);
	ConversationRegistry.ConversationTriggerQueue.Reset();
}
