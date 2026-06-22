// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "ExtendedCommonUserWidget.h"

#include "RecallNotificationWidget.generated.h"

/**
 * Widget to display a notification.
 */
UCLASS(Abstract)
class RECALLGAMEPLAYREPRESENTATION_API URecallNotificationWidget :
	public UExtendedCommonUserWidget
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable, Category=Notification)
	void SetNotification(UPARAM(meta=(RowType="/Script/RecallGameplayCore.RecallUINotificationRow")) FDataTableRowHandle Notification);

	// UUserWidget implementation Begin
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	// UUserWidget implementation End
	
protected:
	/**
	 * Format the notification text to display.
	 */
	UPROPERTY(EditAnywhere, Category=Notification)
	FText NotificationText;

	UPROPERTY(BlueprintReadOnly, Transient, meta=(BindWidgetAnimOptional), Category=Notification)
	TObjectPtr<class UWidgetAnimation> Animation_StartNotification;

	UPROPERTY(BlueprintReadOnly, Transient, meta=(BindWidgetAnimOptional), Category=Notification)
	TObjectPtr<class UWidgetAnimation> Animation_StopNotification;
	
	/**
	 * Optional widget to display the notification text.
	 */
	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional), Category=Notification)
	TObjectPtr<class UCommonRichTextBlock> CommonRichTextBlock_Notif;
	
	/**
	 * Optional widget to display the notification icon.
	 */
	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional), Category=Notification)
	TObjectPtr<class UCommonLazyImage> CommonLazyImage_Icon;

	UPROPERTY(Transient, BlueprintReadOnly, meta=(ExposeOnSpawn))
	float NotificationDuration = 3.0f;

	void StartNotification();

	UFUNCTION()
	void FinishStartNotification();

	void StopNotification();
	
	UFUNCTION()
	void FinishStopNotification();
};
