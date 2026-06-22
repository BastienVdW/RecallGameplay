// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "Utility/Representation/RecallRepresentationUtils.h"

#include "MassEntityUtils.h"
#include "MassEntityView.h"
#include "Simulation/Controller/RecallControllerFragments.h"
#include "System/Representation/RecallRepresentationEventSubsystem.h"
#include "Utility/Player/RecallPlayerUtils.h"
#include "UI/Event/RecallUIEventReactInterface.h"
#include "UI/Notification/RecallUINotificationInterface.h"
#include "UI/Popup/RecallUIPopupInterface.h"

namespace Recall::Representation::Utils
{

void SendUIEvent(const UObject* WorldContextObject,
	const FMassEntityHandle& Entity, const FGameplayTag& EventTag)
{
	const UWorld* World = IsValid(WorldContextObject) ? WorldContextObject->GetWorld() : nullptr;
	if (!ensure(IsValid(World)))
	{
		return;
	}
	
	auto* RepresentationEventSystem = UWorld::GetSubsystem<URecallRepresentationEventSubsystem>(World);
	if (!ensureAlwaysMsgf(IsValid(RepresentationEventSystem),
		TEXT("%hs Invalid state tree system"), __FUNCTION__))
	{
		return;
	}
	
	const FMassEntityManager& EntityManager = UE::Mass::Utils::GetEntityManagerChecked(*World);
	const FMassEntityView TargetView(EntityManager, Entity);
	const auto* PlayerControllerFragmentPtr = TargetView.GetFragmentDataPtr<FRecallControllerFragment>();
	if (PlayerControllerFragmentPtr == nullptr)
	{
		return;
	}

	int32 PlayerIndex = INDEX_NONE;
	if (!Recall::Player::Utils::FindLocalPlayerIndex(
			World, PlayerControllerFragmentPtr->ControllerID, PlayerIndex))
	{
		return;
	}

	RepresentationEventSystem->PushObserverEvent<IRecallUIEventReactInterface>(
		[PlayerIndex, EventTag](auto& Observer)
	{
		if (Observer.IsValid())
		{
			IRecallUIEventReactInterface::Execute_OnUIEvent(Observer.Object.Get(), EventTag, PlayerIndex);
		}
	});
}

void PushUIPopup(URecallRepresentationEventSubsystem& EventSystem,
	const FText& Text, const FVector& Location, float Duration /*= 3.0f*/)
{
	EventSystem.PushObserverEvent<IRecallUIPopupInterface>(
		[Text, Location, Duration](auto& Observer)
	{
		if (Observer.IsValid())
		{
			IRecallUIPopupInterface::Execute_ShowPopup(
				Observer.Object.Get(), Location, Text, Duration);
		}
	});
}

void PushUINotification(URecallRepresentationEventSubsystem& EventSystem,
	const FDataTableRowHandle& Notification, const FString& ControllerID)
{
	if (Notification.IsNull())
	{
		return;
	}
	
	EventSystem.PushObserverEvent<IRecallUINotificationInterface>(
	[Notification, ControllerID](auto& Observer)
	{
		if (Observer.IsValid())
		{
			Observer.Interface.ShowNotification(Notification, ControllerID);
		}
	});
}
	
} // namespace Recall::Representation::Utils
