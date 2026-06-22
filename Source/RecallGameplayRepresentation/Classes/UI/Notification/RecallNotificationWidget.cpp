// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallNotificationWidget.h"

#include "CommonLazyImage.h"
#include "CommonRichTextBlock.h"
#include "UI/Notification/RecallUINotificationRow.h"

#define LOCTEXT_NAMESPACE "URecallNotificationWidget"

URecallNotificationWidget::URecallNotificationWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NotificationText = NSLOCTEXT("URecallNotificationWidget", "NotificationText", "{NotificationText}");
}

void URecallNotificationWidget::NativeConstruct()
{
	Super::NativeConstruct();

	StartNotification();
}

void URecallNotificationWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void URecallNotificationWidget::SetNotification(FDataTableRowHandle Notification)
{
	static const FString ContextString(TEXT("URecallNotificationWidget::SetNotification"));
	const FRecallUINotificationRow* NotificationRow = Notification.GetRow<FRecallUINotificationRow>(ContextString);
	if (NotificationRow == nullptr)
	{
		return;
	}

	if (CommonRichTextBlock_Notif)
	{
		FFormatNamedArguments Args;
		Args.Add("NotificationText", NotificationRow->NotificationText);
		
		CommonRichTextBlock_Notif->SetText(FText::Format(NotificationText, Args));
	}
	
	if (CommonLazyImage_Icon)
	{
		CommonLazyImage_Icon->SetBrushFromLazyTexture(NotificationRow->IconTexture, true);
	}

	NotificationDuration = NotificationRow->Duration;
}

void URecallNotificationWidget::StartNotification()
{
	if (Animation_StartNotification)
	{
		FWidgetAnimationDynamicEvent Delegate;
		Delegate.BindDynamic(this, &ThisClass::FinishStartNotification);
		
		BindToAnimationFinished(Animation_StartNotification, Delegate);
		PlayAnimationForward(Animation_StartNotification, 1.0f, true);
	}
	else
	{
		FinishStartNotification();	
	}
}

void URecallNotificationWidget::FinishStartNotification()
{
	UnbindAllFromAnimationFinished(Animation_StartNotification);
	
	if (NotificationDuration > 0.0f)
	{
		if (const UWorld* World = GetWorld())
		{
			FTimerHandle DummyTimer;
			World->GetTimerManager().SetTimer(DummyTimer, this, &ThisClass::StopNotification,
				NotificationDuration, false);
		}
	}
	else
	{
		StopNotification();
	}
}

void URecallNotificationWidget::StopNotification()
{
	if (Animation_StopNotification)
	{
		FWidgetAnimationDynamicEvent Delegate;
		Delegate.BindDynamic(this, &ThisClass::FinishStopNotification);
		
		BindToAnimationFinished(Animation_StopNotification, Delegate);
		PlayAnimationForward(Animation_StopNotification, 1.0f, true);
	}
	else
	{
		FinishStopNotification();	
	}
}

void URecallNotificationWidget::FinishStopNotification()
{
	UnbindAllFromAnimationFinished(Animation_StopNotification);	
	RemoveFromParent();
}

#undef LOCTEXT_NAMESPACE
