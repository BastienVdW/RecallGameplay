// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallInteractTypes.h"

UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Interacting,								"State.Interacting",								"Performing an interaction");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_BlockInteraction,							"State.BlockInteraction",							"Block interaction with interactable entities");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_InteractSpeedModifier,					"Attribute.InteractSpeedModifier",					"Modify how fast an interaction tick");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(StateTreeEvent_Interact_Contextual_Primary, 		"StateTreeEvent.Interact.Contextual.Primary",		"Primary contextual interaction");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(StateTreeEvent_Interact_Contextual_Secondary, 	"StateTreeEvent.Interact.Contextual.Secondary",		"Secondary contextual interaction");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(StateTreeEvent_Interact_Contextual_Tertiary, 	"StateTreeEvent.Interact.Contextual.Tertiary",		"Tertiary contextual interaction");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(StateTreeEvent_Interact_Contextual_Quaternary, 	"StateTreeEvent.Interact.Contextual.Quaternary",	"Quaternary contextual interaction");

FRecallInteractionParameters::FRecallInteractionParameters()
{
	Events.AddDefaulted();
}

bool FRecallInteractionParameters::HasEvent(int32 Index) const
{
	return Events.IsValidIndex(Index);
}

FRecallInteractionEvent& FRecallInteractionParameters::GetMutableEventChecked(int32 Index)
{
	check(HasEvent(Index));
	return Events[Index];
}

const FRecallInteractionEvent& FRecallInteractionParameters::GetEventChecked(int32 Index) const
{
	check(HasEvent(Index));
	return Events[Index];
}
