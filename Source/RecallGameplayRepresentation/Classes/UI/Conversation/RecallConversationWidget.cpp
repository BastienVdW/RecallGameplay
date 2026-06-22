// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallConversationWidget.h"

#include "Dialog/RecallConversationDialogWidget.h"
#include "RecallFrontendUtils.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

URecallConversationWidget::URecallConversationWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bStartedConversation = false;
}

void URecallConversationWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void URecallConversationWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void URecallConversationWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	Recall::Frontend::Utils::RegisterGlobalObserver<IRecallConversationReactInterface>(this);
}

void URecallConversationWidget::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();

	Recall::Frontend::Utils::UnregisterAllGlobalObservers(this);
}

void URecallConversationWidget::OnConversationUpdatedEvent(const FClientConversationMessagePayload& Message,
                                                             int32 PlayerIndex)
{
	if (!IsOwningLocalPlayer(PlayerIndex) || !ensure(bStartedConversation))
	{
		return;
	}
	
	SetConversationMessage(Message);
}

void URecallConversationWidget::OnConversationStatusChangedEvent(bool bStarted, int32 PlayerIndex)
{
	if (!IsOwningLocalPlayer(PlayerIndex))
	{
		return;
	}

	SetConversationStatus(bStarted);
}

void URecallConversationWidget::SetConversationStatus(bool bStarted)
{
	if (bStartedConversation == bStarted)
	{
		return;
	}
	
	bStartedConversation = bStarted;
	
	if (!bStartedConversation)
	{
		if (CommonActivatableWidgetStack_Dialog)
		{
			CommonActivatableWidgetStack_Dialog->ClearWidgets();
		}
	}
}

void URecallConversationWidget::SetConversationMessage(const FClientConversationMessagePayload& Message)
{
	if (CommonActivatableWidgetStack_Dialog)
	{
		CommonActivatableWidgetStack_Dialog->ClearWidgets();
		
		if (ConversationDialogWidgetClass)
		{
			CommonActivatableWidgetStack_Dialog->AddWidget<URecallConversationDialogWidget>(ConversationDialogWidgetClass,
				[&Message](URecallConversationDialogWidget& Widget)
			{
				Widget.SetConversationMessage(Message);
			});
		}
	}
}
