// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallConversationStateTreeEventSideEffect.h"

#include "Conversation/RecallConversationTypes.h"
#include "ConversationContext.h"
#include "MassExtendedEntityView.h"
#include "RecallSignalSubsystem.h"
#include "Simulation/StateTree/RecallStateTreeFragments.h"
#include "Simulation/StateTree/RecallStateTreeSignalTypes.h"
#include "System/AI/RecallStateTreeSubsystem.h"
#include "Utility/Conversation/RecallConversationNodeUtils.h"

//----------------------------------------------------------------------//
// URecallConversationStateTreeEventSideEffect
//----------------------------------------------------------------------//
void URecallConversationStateTreeEventSideEffect::CauseSideEffect_Implementation(
	const FConversationContext& Context) const
{
	URecallStateTreeSubsystem* StateTreeSystem = UWorld::GetSubsystem<URecallStateTreeSubsystem>(Context.GetWorld());
	if (!ensureAlwaysMsgf(IsValid(StateTreeSystem),
		TEXT("%hs Invalid state tree system"), __FUNCTION__))
	{
		return;
	}

	const TArray<FMassExtendedEntityHandle> SourceEntities = Recall::Conversation::Node::Utils::GetTargetEntities(Context, Source);
	const TArray<FMassExtendedEntityHandle> TargetEntities = Recall::Conversation::Node::Utils::GetTargetEntities(Context, Target);

	for (const FMassExtendedEntityHandle& TargetEntity : TargetEntities)
	{
		const FMassExtendedEntityView EntityView = Recall::Conversation::Node::Utils::CreateEntityView(Context, TargetEntity);
		const FRecallStateTreeInstanceFragment* StateTreeInstanceFragmentPtr = EntityView.GetFragmentDataPtr<FRecallStateTreeInstanceFragment>();
		if (StateTreeInstanceFragmentPtr == nullptr)
		{
			UE_LOG(LogRecallConversation, Warning,
				TEXT("%hs The target entity does not have a state tree attached"), __FUNCTION__);
			return;
		}

		for (const FMassExtendedEntityHandle& SourceEntity : SourceEntities)
		{
			FStateTreeEvent StateTreeEvent;
			StateTreeEvent.Tag = Tag;
			StateTreeEvent.Payload = FInstancedStruct::Make<FMassExtendedEntityHandle>(SourceEntity);
			StateTreeEvent.Origin = Origin;
		
			StateTreeSystem->SendStateTreeEvent(StateTreeInstanceFragmentPtr->InstanceHandle, StateTreeEvent);
		}
	}
	
	if (URecallSignalSubsystem* SignalSystem = UWorld::GetSubsystem<URecallSignalSubsystem>(Context.GetWorld()))
	{
		SignalSystem->SignalEntities(Recall::StateTree::Signals::EventReceived, TargetEntities);
	}
}
