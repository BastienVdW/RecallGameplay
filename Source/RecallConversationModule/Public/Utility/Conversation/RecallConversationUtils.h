// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"

struct FGameplayTag;
struct FMassExtendedEntityHandle;
struct FMassExtendedExecutionContext;

namespace Recall::Conversation::Utils
{

RECALLCONVERSATIONMODULE_API extern bool CanStartConversation(
	FMassExtendedExecutionContext& Context, const TArray<FMassExtendedEntityHandle>& ParticipantEntities,
	const FMassExtendedEntityHandle& ConversationEntity, const FGameplayTag& ConversationEntryPoint,
	const FString& ConversationEntryIdentifier);
RECALLCONVERSATIONMODULE_API extern bool CanStartConversation(
	FMassExtendedExecutionContext& Context, const FMassExtendedEntityHandle& ParticipantEntity,
	const FMassExtendedEntityHandle& ConversationEntity, const FGameplayTag& ConversationEntryPoint,
	const FString& ConversationEntryIdentifier);
	
RECALLCONVERSATIONMODULE_API extern void StartConversation(
	FMassExtendedExecutionContext& Context, const TArray<FMassExtendedEntityHandle>& ParticipantEntities,
	const FMassExtendedEntityHandle& ConversationEntity, const FGameplayTag& ConversationEntryPoint,
	const FString& ConversationEntryIdentifier);
RECALLCONVERSATIONMODULE_API extern void StartConversation(
	FMassExtendedExecutionContext& Context, const FMassExtendedEntityHandle& ParticipantEntity,
	const FMassExtendedEntityHandle& ConversationEntity, const FGameplayTag& ConversationEntryPoint,
	const FString& ConversationEntryIdentifier);

} // namespace Recall::Conversation::Utils
