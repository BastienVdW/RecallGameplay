// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "System/Conversation/RecallConversationRegistryTypes.h"

#include "Actor/Conversation/RecallConversationParticipantActor.h"
#include "Conversation/ConversationInstanceBaitAndSwitch.h"
#include "ConversationInstance.h"

static void ReleaseConversationInstance(const TObjectPtr<UConversationInstance>& ConversationInstance, TArray<TWeakObjectPtr<AActor>>& ParticipanActors)
{
	if (ConversationInstance)
	{
		const FConversationParticipants Participants = ConversationInstance->GetParticipantsCopy();
		for (const FConversationParticipantEntry& Participant : Participants.List)
		{
			ParticipanActors.Add(Participant.Actor);
		}
		
		ConversationInstance->ServerAbortConversation();	
	}
}

void FRecallConversationInstanceEntry::ReleaseConversation(TArray<TWeakObjectPtr<AActor>>& ParticipanActors)
{
	ReleaseConversationInstance(ConversationInstance, ParticipanActors);

	MutableData.Reset();
}

void FRecallConversationInstanceEntry::RestoreConversation(TArray<TWeakObjectPtr<AActor>>& ParticipanActors) const
{
	if (!ensureAlwaysMsgf(ConversationInstance, TEXT("%hs Invalid conversation instance"), __FUNCTION__))
	{
		return;
	}

	ReleaseConversationInstance(ConversationInstance, ParticipanActors);
	
	for (const TPair<FGameplayTag, FRecallConversationParticipant>& ParticipantTuple : MutableData.Participants)
	{
		const TWeakObjectPtr<ARecallConversationParticipantActor> ParticipantActor = Cast<ARecallConversationParticipantActor>(ParticipanActors.Pop(EAllowShrinking::No));
		if (ParticipantActor.IsValid())
		{
			ParticipantActor->SetConversationParticipant(MutableData.Entity, ParticipantTuple.Key,
				ParticipantTuple.Value.Entity, ParticipantTuple.Value.PlayerID);
		}
		ConversationInstance->ServerAssignParticipant(ParticipantTuple.Key, ParticipantActor.Get());
	}

	if (!MutableData.bStarted)
	{
		return;
	}
			
	SetConversationSeed(MutableData.InitialSeed);
	ConversationInstance->ServerStartConversation(MutableData.EntryPoint, MutableData.ActiveConversationGraph,
		MutableData.EntryPointIdentifier);

	for (int32 ChoiceIndex = 0; ChoiceIndex < MutableData.Choices.Num(); ChoiceIndex++)
	{
		const FRecallConversationChoice& Choice = MutableData.Choices[ChoiceIndex];
				
		SetConversationSeed(Choice.Seed);
		ConversationInstance->ServerAdvanceConversation(Choice.Choice);				
	}
}

void FRecallConversationInstanceEntry::SetConversationSeed(int32 Seed) const
{
	static_assert(sizeof(UConversationInstanceBaitAndSwitch) == sizeof(UConversationInstance), "Must have the same size");

	// Dirty hack to set the conversation RNG seed
	if (auto* BaitAndSwitch = reinterpret_cast<UConversationInstanceBaitAndSwitch*>(ConversationInstance.Get()))
	{
		BaitAndSwitch->ConversationRNG.Initialize(Seed);
	}
}

void FRecallConversationInstanceSnapshot::Save(const FRecallConversationInstanceEntry& Instance)
{
	InstanceData = Instance.MutableData;
}

void FRecallConversationInstanceSnapshot::Restore(FRecallConversationInstanceEntry& OutInstance) const
{
	OutInstance.MutableData = InstanceData;
}

FRecallConversationSnapshot::FRecallConversationSnapshot(const FRecallConversationRegistry& Registry)
{
	Save(Registry);
}

void FRecallConversationSnapshot::Save(const FRecallConversationRegistry& Registry)
{
	SerialNumberGenerator = Registry.SerialNumberGenerator;
	FreeInstanceIndices = Registry.FreeInstanceIndices;
	Instances.SetNum(Registry.InstanceCount - Registry.FreeInstanceIndices.Num());

	int32 InstanceIndex = 0;
	
	for (int32 Index = 0; Index < Instances.Num(); Index++)
	{
		while (Registry.FreeInstanceIndices.Contains(InstanceIndex))
		{
			InstanceIndex++;
		}
		
		Instances[Index].Save(Registry.Instances[InstanceIndex]);
		InstanceMapping.Add(InstanceIndex, Index);
	}
}

void FRecallConversationSnapshot::Restore(FRecallConversationRegistry& OutRegistry) const
{
	const int32 OldInstanceCount = OutRegistry.InstanceCount;
	
	OutRegistry.SerialNumberGenerator = SerialNumberGenerator;
	OutRegistry.FreeInstanceIndices = FreeInstanceIndices;
	OutRegistry.InstanceCount = Instances.Num() + FreeInstanceIndices.Num();

	const int32 MaxInstanceCount = FMath::Max(OldInstanceCount, OutRegistry.InstanceCount);
	for (int32 InstanceIndex = 0; InstanceIndex < MaxInstanceCount; InstanceIndex++)
	{		
		FRecallConversationInstanceEntry& Instance = OutRegistry.Instances[InstanceIndex];
		
		const bool bStarted = InstanceIndex < OutRegistry.InstanceCount
			&& !OutRegistry.FreeInstanceIndices.Contains(InstanceIndex);
		
		if (bStarted)
		{
			const int32 OldInstanceIndex = InstanceMapping[InstanceIndex];
			Instances[OldInstanceIndex].Restore(Instance);
			Instance.RestoreConversation(OutRegistry.ParticipanActors);
		}
		else
		{
			Instance.ReleaseConversation(OutRegistry.ParticipanActors);
		}
	}
}
