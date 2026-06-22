// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "System/Interface/RecallSimulationReactSystemInterface.h"
#include "Mass/ExternalSubsystemTraits.h"
#include "RecallConversationRegistryTypes.h"

#include "RecallConversationSubsystem.generated.h"

struct FGameplayTagContainer;
struct FRecallConversationHandle;

UCLASS()
class RECALLCONVERSATIONMODULE_API URecallConversationSubsystem : 
	public UWorldSubsystem,
	public IRecallSimulationReactSystemInterface
{
	GENERATED_BODY()

public:
	// Create/release a conversation instance
	FRecallConversationHandle CreateConversationInstance(const FMassEntityHandle& Entity);
	void ReleaseConversationInstance(FRecallConversationHandle& Handle);

	// Condition
	bool CanStartConversation(const FMassEntityHandle& ConversationEntity,
		const FGameplayTag& EntryPoint, const FString& EntryIdentifier, const TObjectPtr<const UConversationDatabase>& Graph,
		const FGameplayTag& ParticipantID, const FMassEntityHandle& ParticipantEntity, FString ParticipantPlayerID = FString());

	// Participants
	void AddConversationParticipant(const FRecallConversationHandle& Handle, const FGameplayTag& ParticipantID,
		const FMassEntityHandle& Entity, FString PlayerID = FString());
	void RemoveConversationParticipant(const FRecallConversationHandle& Handle, const FGameplayTag& ParticipantID);
	TArray<FString> GetConversationParticipantPlayers(const FRecallConversationHandle& Handle) const;

	// Conversation progress
	bool StartConversation(FRecallConversationHandle& Handle,
		const FGameplayTag& EntryPoint, const FString& EntryPointIdentifier, const TObjectPtr<const UConversationDatabase>& Graph);
	void AdvanceConversation(FRecallConversationHandle& Handle, const FConversationChoiceReference& ChoiceReference);

	void RefreshConversation(const FRecallConversationHandle& Handle) const;

	// Triggers
	void PushConversationTrigger(const FGameplayTag& TriggerTag);
	void PushConversationTriggers(const FGameplayTagContainer& TriggerTags);
	void PopConversationTriggers(FGameplayTagContainer& OutTriggerTags);
	
protected:
	// UWorldSubsystem implementation Begin
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// UWorldSubsystem implementation End
	
	// IRecallSimulationReactSystemInterface implementation Begin
	virtual void Reset() override;
	virtual void Save(const FRecallSnapshotContext& Context, FInstancedStruct& OutSnapshot) override;
	virtual void Restore(const FRecallSnapshotContext& Context, const FInstancedStruct& InSnapshot) override;
	// IRecallSimulationReactSystemInterface implementation End

protected:
	UPROPERTY(VisibleAnywhere)
	TWeakObjectPtr<class URecallRandomNumberSubsystem> RandomNumberSystem;
	UPROPERTY(VisibleAnywhere)
	TWeakObjectPtr<class URecallSignalSubsystem> SignalSystem;

	UPROPERTY(VisibleAnywhere)
	FRecallConversationRegistry ConversationRegistry;

	mutable FCriticalSection DataGuard;

	void CheckIfHandleIsValid(const FRecallConversationHandle& Handle) const;
	int32 GetNextSeedChecked() const;
	class URecallSignalSubsystem& GetSignalSystemChecked() const;

	void OnActorsInitialized(const FActorsInitializedParams& Params);
};

template<>
struct TMassExternalSubsystemTraits<URecallConversationSubsystem>
{
	enum
	{
		GameThreadOnly = false
	};
};
