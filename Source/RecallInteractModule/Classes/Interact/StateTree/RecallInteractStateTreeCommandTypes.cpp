// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallInteractStateTreeCommandTypes.h"

#include "MassExtendedEntityView.h"
#include "RecallSignalSubsystem.h"
#include "Simulation/StateTree/RecallStateTreeFragments.h"
#include "Simulation/StateTree/RecallStateTreeSignalTypes.h"
#include "System/AI/RecallStateTreeSubsystem.h"

//----------------------------------------------------------------------//
// FRecallInteractSendStateTreeEventCommand
//----------------------------------------------------------------------//
void FRecallInteractSendStateTreeEventCommand::OnExecute(const FRecallInteractContext& Context) const
{
	URecallStateTreeSubsystem* StateTreeSystem = UWorld::GetSubsystem<URecallStateTreeSubsystem>(Context.GetWorld());
	if (!ensureAlwaysMsgf(IsValid(StateTreeSystem), TEXT("%hs Invalid state tree system"), __FUNCTION__))
	{
		return;
	}

	const FMassExtendedEntityHandle& EventTargetEntity = Context.GetTargetEntity(EventTarget);
	const FMassExtendedEntityView EntityView(Context.GetEntityManagerChecked(), EventTargetEntity);
	const FRecallStateTreeInstanceFragment* StateTreeInstanceFragmentPtr = EntityView.GetFragmentDataPtr<FRecallStateTreeInstanceFragment>();
	if (StateTreeInstanceFragmentPtr == nullptr)
	{
		UE_LOG(LogRecallInteractCommand, Warning,
			TEXT("%hs The target entity does not have a state tree attached"), __FUNCTION__);
		return;
	}
	
	FStateTreeEvent StateTreeEvent;
	StateTreeEvent.Tag = Tag;
	StateTreeEvent.Payload = FInstancedStruct::Make<FMassExtendedEntityHandle>(Context.GetTargetEntity(PayloadTarget));
	StateTreeEvent.Origin = Origin;
		
	StateTreeSystem->SendStateTreeEvent(StateTreeInstanceFragmentPtr->InstanceHandle, StateTreeEvent);

	if (Context.SignalSystemPtr != nullptr)
	{
		Context.SignalSystemPtr->SignalEntity(
			Recall::StateTree::Signals::EventReceived, EventTargetEntity);
	}
}
