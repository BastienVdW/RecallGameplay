// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"

struct FGameplayTag;
struct FMassEntityHandle;
struct FMassExecutionContext;

namespace Recall::Conversation::Utils
{

RECALLCONVERSATIONMODULE_API extern bool CanStartConversation(
	FMassExecutionContext& Context, const TArray<FMassEntityHandle>& ParticipantEntities,
	const FMassEntityHandle& ConversationEntity, const FGameplayTag& ConversationEntryPoint,
	const FString& ConversationEntryIdentifier);
RECALLCONVERSATIONMODULE_API extern bool CanStartConversation(
	FMassExecutionContext& Context, const FMassEntityHandle& ParticipantEntity,
	const FMassEntityHandle& ConversationEntity, const FGameplayTag& ConversationEntryPoint,
	const FString& ConversationEntryIdentifier);
	
RECALLCONVERSATIONMODULE_API extern void StartConversation(
	FMassExecutionContext& Context, const TArray<FMassEntityHandle>& ParticipantEntities,
	const FMassEntityHandle& ConversationEntity, const FGameplayTag& ConversationEntryPoint,
	const FString& ConversationEntryIdentifier);
RECALLCONVERSATIONMODULE_API extern void StartConversation(
	FMassExecutionContext& Context, const FMassEntityHandle& ParticipantEntity,
	const FMassEntityHandle& ConversationEntity, const FGameplayTag& ConversationEntryPoint,
	const FString& ConversationEntryIdentifier);

} // namespace Recall::Conversation::Utils
