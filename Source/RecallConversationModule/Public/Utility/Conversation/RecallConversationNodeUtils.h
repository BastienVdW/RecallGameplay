// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "Conversation/RecallConversationNodeTypes.h"

struct FConversationContext;
struct FGameplayTag;

struct FMassEntityHandle;
struct FMassEntityManager;
struct FMassEntityView;

/**
 * Helper methods to use inside conversation nodes.
 */
namespace Recall::Conversation::Node::Utils
{
	
/**
 * The conversation is in the process of being restored outside the processing phase.
 * The simulation and its entities should not be directly modified from a task node when that happens.
 */
RECALLCONVERSATIONMODULE_API extern bool IsBeingRestored(const FConversationContext& Context);

/**
 * Return a list of all the player participants for this conversation;
 */
RECALLCONVERSATIONMODULE_API extern TArray<FGameplayTag> GetPlayerParticipants(const FConversationContext& Context);

RECALLCONVERSATIONMODULE_API TArray<FGameplayTag> GetAllParticipants(const FConversationContext& Context);
	
/**
 * Return the entity that own the participant.
 */
RECALLCONVERSATIONMODULE_API extern FMassEntityHandle GetParticipantEntity(const FConversationContext& Context, const FGameplayTag& ParticipantID);
RECALLCONVERSATIONMODULE_API extern TArray<FMassEntityHandle> GetParticipantEntities(const FConversationContext& Context, const TArray<FGameplayTag>& Participants);
RECALLCONVERSATIONMODULE_API extern TArray<FMassEntityHandle> GetAllParticipantEntities(const FConversationContext& Context);
RECALLCONVERSATIONMODULE_API extern TArray<FMassEntityHandle> GetPlayerParticipantEntities(const FConversationContext& Context);

RECALLCONVERSATIONMODULE_API extern FMassEntityManager& GetEntityManagerChecked(const FConversationContext& Context);
RECALLCONVERSATIONMODULE_API extern FMassEntityView CreateEntityView(const FConversationContext& Context, const FMassEntityHandle& Entity);

RECALLCONVERSATIONMODULE_API extern FMassEntityHandle GetConversationEntity(const FConversationContext& Context);
RECALLCONVERSATIONMODULE_API extern TArray<FMassEntityHandle> GetTargetEntities(
	const FConversationContext& Context, ERecallConversationNodeTarget Target);
	
} // namespace Recall::Conversation::Node::Utils
