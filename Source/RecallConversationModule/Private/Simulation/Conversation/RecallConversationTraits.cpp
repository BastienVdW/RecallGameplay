// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "Simulation/Conversation/RecallConversationTraits.h"

#include "MassExtendedEntityTemplateRegistry.h"
#include "Simulation/Conversation/RecallConversationFragments.h"

//----------------------------------------------------------------------//
// URecallConversationTrait
//----------------------------------------------------------------------//
void URecallConversationTrait::BuildTemplate(FMassExtendedEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	FMassExtendedEntityManager& EntityManager = UE::MassExtended::Utils::GetEntityManagerChecked(World);

	FRecallConversationFragment& ConversationFragment = BuildContext.AddFragment_GetRef<FRecallConversationFragment>();
	ConversationFragment.ConversationEntryPoint = DefaultConversationEntryPoint;
	ConversationFragment.ConversationEntryIdentifier = DefaultConversationEntryPointIdentifier;
	
	FRecallConversationConstSharedFragment ConstSharedFragment;
	ConstSharedFragment.Conversation = Conversation;
	ConstSharedFragment.DefaultConversationEntryPoint = DefaultConversationEntryPoint;
	ConstSharedFragment.DefaultConversationEntryPointIdentifier = DefaultConversationEntryPointIdentifier;
	ConstSharedFragment.TriggerSettings = TriggerSettings;

	BuildContext.AddConstSharedFragment(EntityManager.GetOrCreateConstSharedFragment(ConstSharedFragment));
}

//----------------------------------------------------------------------//
// URecallConversationParticipantTrait
//----------------------------------------------------------------------//
void URecallConversationParticipantTrait::BuildTemplate(FMassExtendedEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	FMassExtendedEntityManager& EntityManager = UE::MassExtended::Utils::GetEntityManagerChecked(World);

	BuildContext.AddFragment<FRecallConversationParticipantFragment>();

	FRecallConversationParticipantConstSharedFragment ConstSharedFragment;
	ConstSharedFragment.ParticipantID = ParticipantID;

	BuildContext.AddConstSharedFragment(EntityManager.GetOrCreateConstSharedFragment(ConstSharedFragment));
}
