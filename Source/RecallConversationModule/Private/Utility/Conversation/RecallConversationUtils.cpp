// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "Utility/Conversation/RecallConversationUtils.h"

#include "Conversation/RecallConversationTypes.h"
#include "MassExtendedCommandBuffer.h"
#include "MassExtendedEntityUtils.h"
#include "MassExtendedEntityView.h"
#include "MassExtendedExecutionContext.h"
#include "Simulation/Conversation/RecallConversationFragments.h"
#include "Simulation/Controller/RecallControllerFragments.h"
#include "System/Conversation/RecallConversationSubsystem.h"

namespace Recall::Conversation::Utils
{

struct FRecallConversationContext
{
	FMassExtendedExecutionContext& ExecutionContext;
	URecallConversationSubsystem& ConversationSystem;
	FRecallConversationFragment& ConversationFragment;
	const FRecallConversationConstSharedFragment& ConversationConstSharedFragment;
	const FGameplayTag& EntryPoint;
	const FString& EntryIdentifier;
};
	
struct FRecallConversationParticipantContext
{
	FMassExtendedExecutionContext& ExecutionContext;
	const FMassExtendedEntityHandle& ParticipantEntity;
	FRecallConversationParticipantFragment& ConversationParticipantFragment;
	const FRecallConversationParticipantConstSharedFragment& ConversationParticipantConstSharedFragment;
	const FString& PlayerID;
};
	
static void ForValidConversationParticipant(FMassExtendedExecutionContext& Context,
	const TArray<FMassExtendedEntityHandle>& ParticipantEntities,
	const TFunction<void(const FRecallConversationParticipantContext& ConversationParticipantContext)>& Function)
{
	const UWorld* World = Context.GetWorld();
	if (!ensureAlwaysMsgf(IsValid(World),
			TEXT("%hs Invalid world"), __FUNCTION__))
	{
		return;
	}

	const FMassExtendedEntityManager& EntityManager = UE::MassExtended::Utils::GetEntityManagerChecked(*World);
	
	for (const FMassExtendedEntityHandle& ParticipantEntity : ParticipantEntities)
	{
		const FMassExtendedEntityView ParticipantView(EntityManager, ParticipantEntity);
		auto* ConversationParticipantFragmentPtr = ParticipantView.GetFragmentDataPtr<FRecallConversationParticipantFragment>();
		if (ConversationParticipantFragmentPtr == nullptr)
		{
			continue;
		}
		
		const auto& ConversationParticipantConstFragment = ParticipantView.GetConstSharedFragmentData<FRecallConversationParticipantConstSharedFragment>();
		
		const FRecallControllerFragment* PlayerControllerFragmentPtr = ParticipantView.GetFragmentDataPtr<FRecallControllerFragment>();
		const FString PlayerID = PlayerControllerFragmentPtr != nullptr ? PlayerControllerFragmentPtr->ControllerID : FString();
		
		const FRecallConversationParticipantContext ConversationParticipantContext{ Context, ParticipantEntity,
			*ConversationParticipantFragmentPtr, ConversationParticipantConstFragment, PlayerID };

		Function(ConversationParticipantContext);
	}
}
	
static void ForValidConversation(FMassExtendedExecutionContext& Context,
	const FMassExtendedEntityHandle& ConversationEntity, const FGameplayTag& ConversationEntryPoint,
	const FString& ConversationEntryIdentifier,
	const TFunction<void(const FRecallConversationContext& ConversationContext)>& Function)
{
	const UWorld* World = Context.GetWorld();
	if (!ensureAlwaysMsgf(IsValid(World), TEXT("%hs Invalid world"), __FUNCTION__))
	{
		return;
	}
	
	const FMassExtendedEntityManager& EntityManager = UE::MassExtended::Utils::GetEntityManagerChecked(*World);
	const FMassExtendedEntityView InteractableView(EntityManager, ConversationEntity);
	FRecallConversationFragment* ConversationFragmentPtr = InteractableView.GetFragmentDataPtr<FRecallConversationFragment>();
	if (ConversationFragmentPtr == nullptr)
	{
		UE_LOG(LogRecallConversation, Warning, 
			TEXT("%hs Conversation trait is missing from interactable entity"), __FUNCTION__)
		return;
	}

	URecallConversationSubsystem* ConversationSystemPtr = UWorld::GetSubsystem<URecallConversationSubsystem>(World);
	if (!ensureAlwaysMsgf(ConversationSystemPtr != nullptr,
			TEXT("%hs Invalid conversation system"), __FUNCTION__))
	{
		return;
	}

	const auto& ConversationConstSharedFragment = InteractableView.GetConstSharedFragmentData<FRecallConversationConstSharedFragment>();
	const FGameplayTag& EntryPoint = ConversationEntryPoint.IsValid() ? ConversationEntryPoint : ConversationFragmentPtr->ConversationEntryPoint;
	const FString& EntryIdentifier = !ConversationEntryIdentifier.IsEmpty() ? ConversationEntryIdentifier : ConversationFragmentPtr->ConversationEntryIdentifier;
	const FRecallConversationContext ConversationContext{ Context, *ConversationSystemPtr,
		*ConversationFragmentPtr, ConversationConstSharedFragment, EntryPoint, EntryIdentifier };

	Function(ConversationContext);
}
	
bool CanStartConversation(
	FMassExtendedExecutionContext& Context, const FMassExtendedEntityHandle& ParticipantEntity,
	const FMassExtendedEntityHandle& ConversationEntity, const FGameplayTag& ConversationEntryPoint,
	const FString& ConversationEntryIdentifier)
{
	const TArray<FMassExtendedEntityHandle> ParticipantEntities = { ParticipantEntity };
	return CanStartConversation(Context, ParticipantEntities,
		ConversationEntity, ConversationEntryPoint, ConversationEntryIdentifier);
}
	
bool CanStartConversation(
	FMassExtendedExecutionContext& Context, const TArray<FMassExtendedEntityHandle>& ParticipantEntities,
	const FMassExtendedEntityHandle& ConversationEntity, const FGameplayTag& ConversationEntryPoint,
	const FString& ConversationEntryIdentifier)
{
	bool bCanStart = false;
	
	ForValidConversation(Context, ConversationEntity, ConversationEntryPoint, ConversationEntryIdentifier,
	[&ConversationEntity, &ParticipantEntities, &bCanStart](const FRecallConversationContext& Context)
	{
		URecallConversationSubsystem& ConversationSystem = Context.ConversationSystem;
		const TObjectPtr<const UConversationDatabase>& Conversation = Context.ConversationConstSharedFragment.Conversation;
		const FGameplayTag& EntryPoint = Context.EntryPoint;
		const FString& EntryIdentifier = Context.EntryIdentifier;

		uint8 ParticipantCount = 0;
		
		ForValidConversationParticipant(Context.ExecutionContext, ParticipantEntities,
			[&ParticipantCount, &ConversationEntity, &ConversationSystem,
				&Conversation, &EntryPoint, &EntryIdentifier](const FRecallConversationParticipantContext& Context)
		{
			if (ConversationSystem.CanStartConversation(ConversationEntity, EntryPoint, EntryIdentifier, Conversation,
					Context.ConversationParticipantConstSharedFragment.ParticipantID, Context.ParticipantEntity, Context.PlayerID))
			{
				ParticipantCount++;
			}
		});

		bCanStart |= ParticipantCount > 0 && ParticipantCount == ParticipantEntities.Num();
	});

	return bCanStart;
}
	
void StartConversation(
	FMassExtendedExecutionContext& Context, const FMassExtendedEntityHandle& ParticipantEntity,
	const FMassExtendedEntityHandle& ConversationEntity, const FGameplayTag& ConversationEntryPoint,
	const FString& ConversationEntryIdentifier)
{
	const TArray<FMassExtendedEntityHandle> ParticipantEntities = { ConversationEntity, ParticipantEntity };
	StartConversation(Context, ParticipantEntities,
		ConversationEntity, ConversationEntryPoint, ConversationEntryIdentifier);
}
	
void StartConversation(FMassExtendedExecutionContext& Context, const TArray<FMassExtendedEntityHandle>& ParticipantEntities,
	const FMassExtendedEntityHandle& ConversationEntity, const FGameplayTag& ConversationEntryPoint,
	const FString& ConversationEntryIdentifier)
{
	ForValidConversation(Context, ConversationEntity, ConversationEntryPoint, ConversationEntryIdentifier,
	[&ConversationEntity, &ParticipantEntities](const FRecallConversationContext& Context)
	{
		URecallConversationSubsystem& ConversationSystem = Context.ConversationSystem;
		FRecallConversationHandle& ConversationHandle = Context.ConversationFragment.ConversationHandle =
			Context.ConversationSystem.CreateConversationInstance(ConversationEntity);
		if (!ConversationHandle.IsValid())
		{
			UE_LOG(LogRecallConversation, Warning,
				TEXT("%hs Failed to create conversation instance"), __FUNCTION__);
			return;
		}
		
		bool bAnyParticipant = false;

		ForValidConversationParticipant(Context.ExecutionContext, ParticipantEntities,
			[&ConversationSystem, &ConversationHandle, &bAnyParticipant](const FRecallConversationParticipantContext& Context)
		{
			if (Context.ConversationParticipantFragment.ConversationHandle.IsSet())
			{
				UE_LOG(LogRecallConversation, Warning,
					TEXT("%hs Participant is already in an other conversation"), __FUNCTION__);
				return;
			}
		
			ConversationSystem.AddConversationParticipant(ConversationHandle,
				Context.ConversationParticipantConstSharedFragment.ParticipantID, Context.ParticipantEntity, Context.PlayerID);
			Context.ConversationParticipantFragment.ConversationHandle = ConversationHandle;
			
			bAnyParticipant = true;
		});
	
		if (bAnyParticipant && Context.ConversationSystem.StartConversation(ConversationHandle,
				Context.EntryPoint, Context.EntryIdentifier, Context.ConversationConstSharedFragment.Conversation))
		{
			Context.ExecutionContext.Defer().AddTag<FRecallConversationActiveTag>(ConversationEntity);
		}
		else
		{
			Context.ConversationSystem.ReleaseConversationInstance(ConversationHandle);
		}
	});	
}
	
} // namespace Recall::Conversation::Utils
