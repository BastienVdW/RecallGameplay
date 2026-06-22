// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallNotificationPanelWidget.h"

#include "Components/PanelWidget.h"
#include "GameFramework/PlayerState.h"
#include "RecallFrontendUtils.h"
#include "RecallNotificationWidget.h"
#include "Player/Interface/RecallPlayerStateInterface.h"

void URecallNotificationPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	Recall::Frontend::Utils::RegisterGlobalObserver<IRecallUINotificationInterface>(this);
}

void URecallNotificationPanelWidget::NativeDestruct()
{
	Super::NativeDestruct();

	Recall::Frontend::Utils::UnregisterAllGlobalObservers(this);
}

void URecallNotificationPanelWidget::ShowNotification(const FDataTableRowHandle& Notification, const FString& ControllerID)
{
	const IRecallPlayerStateInterface* PlayerState = Cast<IRecallPlayerStateInterface>(GetOwningPlayerState());
	if (PlayerState == nullptr || PlayerState->GetSimPlayerId() != ControllerID)
	{
		return;
	}
	
	NotificationQueue.Enqueue(Notification);
	if (UpdateNotificationTimer.IsValid())
	{
		return;
	}
	
	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(UpdateNotificationTimer, this, &ThisClass::TryShowNextNotification,
			0.1f, true);
	}
}

void URecallNotificationPanelWidget::TryShowNextNotification()
{
	if (!PanelWidget_Notification || PanelWidget_Notification->GetChildrenCount() >= MaxNotificationCount)
	{
		return;
	}
	
	FDataTableRowHandle Notification;
	if (!NotificationQueue.Dequeue(Notification))
	{
		if (const UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(UpdateNotificationTimer);
		}
		return;
	}

	URecallNotificationWidget* NewWidget = Cast<URecallNotificationWidget>(CreateWidgetInstance(*this, NotificationWidgetClass, NAME_None));
	if (IsValid(NewWidget))
	{
		NewWidget->SetNotification(Notification);
		PanelWidget_Notification->AddChild(NewWidget);
	}
}
