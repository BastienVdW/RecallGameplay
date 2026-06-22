// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "ExtendedCommonUserWidget.h"
#include "UI/Notification/RecallUINotificationInterface.h"

#include "RecallNotificationPanelWidget.generated.h"

/**
 * Widget to receive and handle notifications from the game simulation.
 */
UCLASS(Abstract)
class RECALLGAMEPLAYREPRESENTATION_API URecallNotificationPanelWidget :
	public UExtendedCommonUserWidget,
	public IRecallUINotificationInterface
{
	GENERATED_BODY()
	
	//~ Begin IRecallUINotificationInterface interface
public:	
	virtual void ShowNotification(const FDataTableRowHandle& Notification, const FString& ControllerID) override;
	//~ End IRecallUINotificationInterface interface

	// UUserWidget implementation Begin
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	// UUserWidget implementation End
	
protected:
	/**
	* Define how many notifications can be displayed at the same time.
	*/
	UPROPERTY(EditAnywhere, Category=Notification, meta=(ClampMin=1))
	int32 MaxNotificationCount = 1;

	/**
	 * Class how the widget used to display on-screen notification.
	 */
	UPROPERTY(EditAnywhere, Category=Notification)
	TSubclassOf<class URecallNotificationWidget> NotificationWidgetClass;
	
	/**
	 * Panel to which the notification will be added.
	 */
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category=Notification)
	TObjectPtr<class UPanelWidget> PanelWidget_Notification;

	UPROPERTY(Transient)
	FTimerHandle UpdateNotificationTimer;
	
	TQueue<FDataTableRowHandle> NotificationQueue;

	void TryShowNextNotification();
};
