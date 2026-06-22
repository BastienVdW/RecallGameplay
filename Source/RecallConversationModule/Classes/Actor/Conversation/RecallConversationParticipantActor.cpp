// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallConversationParticipantActor.h"

#include "ConversationParticipantComponent.h"
#include "Representation/Conversation/RecallConversationReactInterface.h"
#include "System/Observer/RecallObserverSubjectSubsystem.h"
#include "System/Synchronization/RecallSynchronizationTypes.h"
#include "Utility/Player/RecallPlayerUtils.h"

ARecallConversationParticipantActor::ARecallConversationParticipantActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetReplicates(false);
	bOnlyRelevantToOwner = true;
	
	RootComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("RootComponent"));
	ConversationParticipantComponent = ObjectInitializer.CreateDefaultSubobject<UConversationParticipantComponent>(this, TEXT("ConversationParticipantComponent"));
}

void ARecallConversationParticipantActor::BeginPlay()
{
	Super::BeginPlay();

	if (ConversationParticipantComponent)
	{
		ConversationParticipantComponent->ConversationUpdated.AddUObject(this, &ThisClass::OnConversationUpdatedEvent);
		ConversationParticipantComponent->ConversationStatusChanged.AddUObject(this, &ThisClass::OnConversationStatusChangedEvent);
	}
	
	ObserverSubjectSystem = UWorld::GetSubsystem<URecallObserverSubjectSubsystem>(GetWorld());
}

void ARecallConversationParticipantActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (ConversationParticipantComponent)
	{
		ConversationParticipantComponent->ConversationUpdated.RemoveAll(this);
		ConversationParticipantComponent->ConversationStatusChanged.RemoveAll(this);
	}

	ObserverSubjectSystem.Reset();
}

void ARecallConversationParticipantActor::SetConversationParticipant(const FMassExtendedEntityHandle& InConversationEntity,
	const FGameplayTag& ParticipantID, const FMassExtendedEntityHandle& ParticipantEntity, const FString& PlayerID)
{
	ConversationEntity = InConversationEntity;
	ConversationParticipantID = ParticipantID;
	ConversationParticipantEntity = ParticipantEntity;
	ConversationParticipantPlayerID = PlayerID;

	if (!ConversationParticipantPlayerID.IsEmpty())
	{
		Recall::Player::Utils::FindLocalPlayerIndex(this, ConversationParticipantPlayerID, ConversationParticipantPlayerIndex);
	}
	
	bConversationStarted = false;
	ConversationNode = FConversationNodeHandle();
}

const FMassExtendedEntityHandle& ARecallConversationParticipantActor::GetConversationEntity() const
{
	check(ConversationEntity.IsValid());
	return ConversationEntity;
}

const FMassExtendedEntityHandle& ARecallConversationParticipantActor::GetParticipantEntity() const
{
	check(ConversationParticipantEntity.IsValid());
	return ConversationParticipantEntity;
}

const FString& ARecallConversationParticipantActor::GetParticipantPlayerID() const
{
	return ConversationParticipantPlayerID;
}

bool ARecallConversationParticipantActor::IsPlayerParticipant() const
{
	return !GetParticipantPlayerID().IsEmpty();
}

bool ARecallConversationParticipantActor::IsValidConversationParticipant() const
{
	return ConversationParticipantID.IsValid();
}

void ARecallConversationParticipantActor::OnConversationStatusChangedEvent(bool bStarted)
{
	SetConversationStatus(bStarted);
}

int32 ARecallConversationParticipantActor::GetOwningPlayerIndex() const
{
	return ConversationParticipantPlayerIndex;
}

void ARecallConversationParticipantActor::OnConversationUpdatedEvent(const FClientConversationMessagePayload& Message)
{
	const int32 PlayerIndex = GetOwningPlayerIndex();
	if (PlayerIndex == INDEX_NONE || Message.CurrentNode == ConversationNode)
	{
		return;
	}
	
	if (!IsRollback(this) && ObserverSubjectSystem.IsValid())
	{
		ConversationNode = Message.CurrentNode;

		const auto Observers = ObserverSubjectSystem->GetObservers<IRecallConversationReactInterface>();		
		AsyncTask(ENamedThreads::GameThread, [Observers, Message, PlayerIndex]() mutable
		{
			for (const auto& Observer : Observers)
			{
				Observer.Interface.OnConversationUpdatedEvent(Message, PlayerIndex);	
			}
		});
	}
}

void ARecallConversationParticipantActor::SetConversationStatus(bool bStarted)
{
	const int32 PlayerIndex = GetOwningPlayerIndex();
	if (PlayerIndex == INDEX_NONE || bConversationStarted == bStarted)
	{
		return;
	}

	if (!IsRollback(this) && ObserverSubjectSystem.IsValid())
	{
		bConversationStarted = bStarted;
		
		const auto& Observers = ObserverSubjectSystem->GetObservers<IRecallConversationReactInterface>();
		AsyncTask(ENamedThreads::GameThread, [Observers, bStarted, PlayerIndex]() mutable
		{
			for (const auto& Observer : Observers)
			{
				if (!Observer.IsValid())
				{
					continue;
				}
				
				Observer.Interface.OnConversationStatusChangedEvent(bStarted, PlayerIndex);
			}
		});
	}
}
