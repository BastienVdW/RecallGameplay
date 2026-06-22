// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "ConversationTypes.h"
#include "Gameframework/Actor.h"
#include "GameplayTagContainer.h"
#include "Mass/EntityHandle.h"

#include "RecallConversationParticipantActor.generated.h"

class UConversationParticipantComponent;

/*
* Dummy to wrap a conversation component.
*/
UCLASS(ComponentWrapperClass)
class RECALLCONVERSATIONMODULE_API ARecallConversationParticipantActor : public AActor
{
	GENERATED_UCLASS_BODY()

public:
	void SetConversationStatus(bool bStarted);
	void SetConversationParticipant(const FMassEntityHandle& InConversationEntity, const FGameplayTag& ParticipantID,
		const FMassEntityHandle& ParticipantEntity, const FString& PlayerID);

	const FMassEntityHandle& GetConversationEntity() const;
	const FMassEntityHandle& GetParticipantEntity() const;
	const FString& GetParticipantPlayerID() const;
	bool IsPlayerParticipant() const;

	bool IsValidConversationParticipant() const;

	//~ Begin AActor Interface
public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End AActor Interface

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"), Category=Conversation)
	TObjectPtr<UConversationParticipantComponent> ConversationParticipantComponent;

	UPROPERTY(Transient, BlueprintReadOnly, Category=Conversation)
	FMassEntityHandle ConversationEntity;
	UPROPERTY(Transient, BlueprintReadOnly, Category=Conversation)
	FGameplayTag ConversationParticipantID;
	UPROPERTY(Transient, BlueprintReadOnly, Category=Conversation)
	FMassEntityHandle ConversationParticipantEntity;
	UPROPERTY(Transient, BlueprintReadOnly, Category=Conversation)
	FString ConversationParticipantPlayerID;
	UPROPERTY(Transient, BlueprintReadOnly, Category=Conversation)
	int32 ConversationParticipantPlayerIndex = INDEX_NONE;
	
	UPROPERTY(Transient)
	TWeakObjectPtr<class URecallObserverSubjectSubsystem> ObserverSubjectSystem;

	UPROPERTY(Transient)
	bool bConversationStarted = false;
	UPROPERTY(Transient)
	FConversationNodeHandle ConversationNode;

	void OnConversationUpdatedEvent(const FClientConversationMessagePayload& Message);
	void OnConversationStatusChangedEvent(bool bStarted);

	int32 GetOwningPlayerIndex() const;
};
