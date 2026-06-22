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

struct FMassExtendedEntityHandle;
struct FMassExtendedEntityManager;
struct FMassExtendedEntityView;

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
RECALLCONVERSATIONMODULE_API extern FMassExtendedEntityHandle GetParticipantEntity(const FConversationContext& Context, const FGameplayTag& ParticipantID);
RECALLCONVERSATIONMODULE_API extern TArray<FMassExtendedEntityHandle> GetParticipantEntities(const FConversationContext& Context, const TArray<FGameplayTag>& Participants);
RECALLCONVERSATIONMODULE_API extern TArray<FMassExtendedEntityHandle> GetAllParticipantEntities(const FConversationContext& Context);
RECALLCONVERSATIONMODULE_API extern TArray<FMassExtendedEntityHandle> GetPlayerParticipantEntities(const FConversationContext& Context);

RECALLCONVERSATIONMODULE_API extern FMassExtendedEntityManager& GetEntityManagerChecked(const FConversationContext& Context);
RECALLCONVERSATIONMODULE_API extern FMassExtendedEntityView CreateEntityView(const FConversationContext& Context, const FMassExtendedEntityHandle& Entity);

RECALLCONVERSATIONMODULE_API extern FMassExtendedEntityHandle GetConversationEntity(const FConversationContext& Context);
RECALLCONVERSATIONMODULE_API extern TArray<FMassExtendedEntityHandle> GetTargetEntities(
	const FConversationContext& Context, ERecallConversationNodeTarget Target);
	
} // namespace Recall::Conversation::Node::Utils
