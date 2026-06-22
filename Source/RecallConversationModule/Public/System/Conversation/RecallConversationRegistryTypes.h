// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "ConversationTypes.h"
#include "Mass/EntityHandle.h"

#include "RecallConversationRegistryTypes.generated.h"

#define RECALL_CONVERSATION_POOL_SIZE 8
#define RECALL_CONVERSATION_PARTICIPANT_POOL_SIZE 16

/**
 * Keep track of a choice that was made to advance the conversation.
 */
USTRUCT()
struct RECALLCONVERSATIONMODULE_API FRecallConversationChoice
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	int32 Seed = 0;
	
	UPROPERTY(VisibleAnywhere)
	FConversationChoiceReference Choice;
};

/**
 * Keep track of conversation participant data.
 */
USTRUCT()
struct RECALLCONVERSATIONMODULE_API FRecallConversationParticipant
{
	GENERATED_BODY()

	/**
	 * Entity owning this participant.
	 */
	UPROPERTY(VisibleAnywhere)
	FMassEntityHandle Entity;
	
	/**
	 * Player controlling this participant, if any
	 */
	UPROPERTY(VisibleAnywhere)
	FString PlayerID;
};

/**
 * Saved data of a conversation instance.
 */
USTRUCT()
struct RECALLCONVERSATIONMODULE_API FRecallConversationInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere)
	FMassEntityHandle Entity;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<const UConversationDatabase> ActiveConversationGraph = nullptr;
	
	UPROPERTY(VisibleAnywhere)
	bool bStarted = false;

	UPROPERTY(VisibleAnywhere)
	TMap<FGameplayTag, FRecallConversationParticipant> Participants;
	
	UPROPERTY(VisibleAnywhere)
	int32 InitialSeed = 0;
	
	UPROPERTY(VisibleAnywhere)
	FGameplayTag EntryPoint = FGameplayTag::EmptyTag;

	UPROPERTY(VisibleAnywhere)
	FString EntryPointIdentifier;

	UPROPERTY(VisibleAnywhere)
	TArray<FRecallConversationChoice> Choices;

	FORCEINLINE void Reset()
	{
		Entity.Reset();
		ActiveConversationGraph = nullptr;
		bStarted = false;
		Participants.Reset();
		InitialSeed = 0;
		EntryPoint = FGameplayTag::EmptyTag;
		EntryPointIdentifier.Empty();
		Choices.Reset();
	}
	
	FORCEINLINE TArray<FMassEntityHandle> GetParticipantEntities() const
	{
		TArray<FMassEntityHandle> Results;
		Results.Reserve(Participants.Num());
		for (const TPair<FGameplayTag, FRecallConversationParticipant>& Participant : Participants)
		{
			Results.Add(Participant.Value.Entity);
		}
		return Results;
	}
};

/**
 * Entry to keep track of an active conversation.
 */
USTRUCT()
struct RECALLCONVERSATIONMODULE_API FRecallConversationInstanceEntry
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere)
	FRecallConversationInstanceData MutableData;
	
	UPROPERTY(VisibleAnywhere, Transient)
	TObjectPtr<class UConversationInstance> ConversationInstance;
	
	/**
	 * Release the conversation by resetting its mutable data.
	 */
	void ReleaseConversation(TArray<TWeakObjectPtr<AActor>>& ParticipanActors);

	/**
	 * Restore the conversation based on its mutable data.
	 */
	void RestoreConversation(TArray<TWeakObjectPtr<AActor>>& ParticipanActors) const;

	/**
	 * Helper method to override the conversation current seed.
	 */
	void SetConversationSeed(int32 Seed) const;
};

/**
 * Registry to keep track of all the conversation instances
 */
USTRUCT()
struct RECALLCONVERSATIONMODULE_API FRecallConversationRegistry
{
	GENERATED_BODY()

	/**
	 * Conversation triggers to be processed during the following frame.
	 * A conversation trigger will force a conversation to start.
	 */
	UPROPERTY(VisibleAnywhere)
	FGameplayTagContainer ConversationTriggerQueue;
	
	/**
	 * Generate serial number for conversation instance handle.
	 */
	UPROPERTY(VisibleAnywhere)
	uint32 SerialNumberGenerator = 0;

	/**
	 * List of unused conversation instance indices so they can be recycled.
	 */
	UPROPERTY(VisibleAnywhere)
	TArray<int32> FreeInstanceIndices;

	/**
	 * How many instances are currently being used.
	 * Include instances that have been released.
	 */
	UPROPERTY(VisibleAnywhere)
	int32 InstanceCount = 0;

	/**
	 * Pool of conversation instances.
	 */
	UPROPERTY(VisibleAnywhere)
	TArray<FRecallConversationInstanceEntry> Instances;

	/**
	 * Pool of actors for the conversation participants.
	 */
	UPROPERTY(VisibleAnywhere, Transient)
	TArray<TWeakObjectPtr<AActor>> ParticipanActors;
};

/**
 * Snapshot of a conversation instance data so it can be restored.
 */
USTRUCT()
struct RECALLCONVERSATIONMODULE_API FRecallConversationInstanceSnapshot
{
	GENERATED_BODY()

public:
	void Save(const FRecallConversationInstanceEntry& Instance);
	void Restore(FRecallConversationInstanceEntry& OutInstance) const;

protected:
	UPROPERTY(VisibleAnywhere)
	FRecallConversationInstanceData InstanceData;
};

/**
 * Snapshot of the conversation registry so it can be restored.
 */
USTRUCT()
struct RECALLCONVERSATIONMODULE_API FRecallConversationSnapshot
{
	GENERATED_BODY()

	FRecallConversationSnapshot() = default;
	FRecallConversationSnapshot(const FRecallConversationRegistry& Registry);
	
public:
	void Save(const FRecallConversationRegistry& Registry);
	void Restore(FRecallConversationRegistry& OutRegistry) const;
	
protected:
	/**
	 * Keep track of the state of the registry's serial number generator.
	 */
	UPROPERTY(VisibleAnywhere)
	uint32 SerialNumberGenerator = 0;

	/**
	 * List of unused conversation instance indices so we can restore the conversation instance pool.
	 */
	UPROPERTY(VisibleAnywhere)
	TArray<int32> FreeInstanceIndices;

	/**
	 * Snapshot of the active conversation instances.
	 */
	UPROPERTY(VisibleAnywhere)
	TArray<FRecallConversationInstanceSnapshot> Instances;
	
	/**
	 * Remap the conversation instances so we do not serialize unused instanced.
	 */
	UPROPERTY(VisibleAnywhere)
	TMap<int32, int32> InstanceMapping;
};
