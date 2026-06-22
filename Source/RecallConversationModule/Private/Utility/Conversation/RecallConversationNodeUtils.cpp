// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "Utility/Conversation/RecallConversationNodeUtils.h"

#include "Actor/Conversation/RecallConversationParticipantActor.h"
#include "ConversationContext.h"
#include "MassEntityUtils.h"
#include "MassEntityView.h"
#include "Utility/Simulation/RecallSimulationUtils.h"

namespace Recall::Conversation::Node::Utils
{
	
bool IsBeingRestored(const FConversationContext& Context)
{
	// While this is called outside the processing phase, for safety,
	// we will consider that the simulation is being restored
	return !Simulation::Utils::IsSimulationProcessingPhase(Context.GetWorld());
}

TArray<FGameplayTag> GetPlayerParticipants(const FConversationContext& Context)
{
	TArray<FGameplayTag> Results;
	
	for (const FConversationParticipantEntry& ParticipantEntry : Context.GetParticipantsCopy().List)
	{
		const ARecallConversationParticipantActor* ParticipantActor = CastChecked<ARecallConversationParticipantActor>(ParticipantEntry.Actor);
		if (ParticipantActor->IsPlayerParticipant())
		{
			Results.Add(ParticipantEntry.ParticipantID);
		}
	}

	return Results;
}

TArray<FGameplayTag> GetAllParticipants(const FConversationContext& Context)
{
	const FConversationParticipants Participants = Context.GetParticipantsCopy();

	TArray<FGameplayTag> Results;
	Results.Reserve(Participants.List.Num());
	
	for (const FConversationParticipantEntry& ParticipantEntry : Participants.List)
	{
		Results.Add(ParticipantEntry.ParticipantID);
	}

	return Results;
}

FMassEntityHandle GetParticipantEntity(const FConversationContext& Context,
                                               const FGameplayTag& ParticipantID)
{
	const FConversationParticipantEntry* ParticipantEntryPtr = Context.GetParticipant(ParticipantID);
	checkf(ParticipantEntryPtr != nullptr, TEXT("%hs Participant does not exist"), __FUNCTION__);
	const ARecallConversationParticipantActor* ParticipantActor = CastChecked<ARecallConversationParticipantActor>(ParticipantEntryPtr->Actor);
	return ParticipantActor->GetParticipantEntity();
}

TArray<FMassEntityHandle> GetParticipantEntities(
	const FConversationContext& Context, const TArray<FGameplayTag>& Participants)
{
	TArray<FMassEntityHandle> Results;
	Results.Reserve(Participants.Num());

	for (const FGameplayTag& ParticipantID : Participants)
	{
		Results.Add(GetParticipantEntity(Context, ParticipantID));	
	}

	return Results;
}

TArray<FMassEntityHandle> GetAllParticipantEntities(const FConversationContext& Context)
{
	const TArray<FGameplayTag> Participants = GetAllParticipants(Context);
	return GetParticipantEntities(Context, Participants);
}

TArray<FMassEntityHandle> GetPlayerParticipantEntities(
	const FConversationContext& Context)
{
	const TArray<FGameplayTag> Participants = GetPlayerParticipants(Context);
	return GetParticipantEntities(Context, Participants);
}

FMassEntityManager& GetEntityManagerChecked(const FConversationContext& Context)
{
	checkf(Context.GetWorld(), TEXT("%hs Invalid world"), __FUNCTION__);
	checkf(!IsBeingRestored(Context),
		TEXT("%hs Not allowed to access entity manager while restoring simulation"), __FUNCTION__);
	return UE::Mass::Utils::GetEntityManagerChecked(*Context.GetWorld());
}

FMassEntityView CreateEntityView(const FConversationContext& Context,
	const FMassEntityHandle& Entity)
{
	return FMassEntityView(GetEntityManagerChecked(Context), Entity);
}

FMassEntityHandle GetConversationEntity(const FConversationContext& Context)
{
	for (const FConversationParticipantEntry& ParticipantEntry : Context.GetParticipantsCopy().List)
	{
		const ARecallConversationParticipantActor* ParticipantActor = CastChecked<ARecallConversationParticipantActor>(ParticipantEntry.Actor);
		return ParticipantActor->GetConversationEntity();
	}

	// Conversations should always have participants
	checkNoEntry();
	return FMassEntityHandle();
}
	
TArray<FMassEntityHandle> GetTargetEntities(
	const FConversationContext& Context, ERecallConversationNodeTarget Target)
{
	switch (Target)
	{
	case ERecallConversationNodeTarget::Player:
		return GetPlayerParticipantEntities(Context);
		
	case ERecallConversationNodeTarget::Owner:
		return { GetConversationEntity(Context) };

	default:
		unimplemented();
		return {};
	}
}
	
} // namespace Recall::Conversation::Node::Utils
