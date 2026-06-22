// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "ExtendedCommonActivatableWidget.h"
#include "Representation/Conversation/RecallConversationReactInterface.h"

#include "RecallConversationWidget.generated.h"

/**
 * Widget to show conversation.
 */
UCLASS(Abstract)
class RECALLGAMEPLAYREPRESENTATION_API URecallConversationWidget :
	public UExtendedCommonActivatableWidget,
	public IRecallConversationReactInterface
{	
	GENERATED_UCLASS_BODY()

public:
	void SetConversationStatus(bool bStarted);
	void SetConversationMessage(const FClientConversationMessagePayload& Message);

	// UUserWidget implementation Begin
protected:	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	// UUserWidget implementation End

	// UCommonActivatableWidget implementation Begin
protected:	
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;
	// UCommonActivatableWidget implementation End
	
	// UUserWidget implementation Begin
protected:	
	virtual void OnConversationUpdatedEvent(const FClientConversationMessagePayload& Message, int32 PlayerIndex = 0) override;
	virtual void OnConversationStatusChangedEvent(bool bStarted, int32 PlayerIndex = 0) override;
	// UUserWidget implementation End
	
protected:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<class UCommonActivatableWidgetStack> CommonActivatableWidgetStack_Dialog;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class URecallConversationDialogWidget> ConversationDialogWidgetClass;

	UPROPERTY(Transient, BlueprintReadOnly)
	uint8 bStartedConversation : 1;
};
