// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "ExtendedCommonActivatableWidget.h"
#include "ConversationTypes.h"

#include "RecallConversationDialogWidget.generated.h"

#define RECALL_CONVERSATION_DIALOG_CHOICE_BUTTON_COUNT 6

/**
 * Widget to show a conversation dialog.
 */
UCLASS(Abstract)
class RECALLGAMEPLAYREPRESENTATION_API URecallConversationDialogWidget : public UExtendedCommonActivatableWidget
{
	GENERATED_BODY()

public:
	URecallConversationDialogWidget();
	
	void SetConversationMessage(const FClientConversationMessagePayload& Message);
	void SetConversationDialog(const FGameplayTag& ParticipantID, const FText& DialogText);

	// UUserWidget implementation Begin
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	// UUserWidget implementation End
	
protected:
	UPROPERTY(EditAnywhere, Category=Dialog)
	FText DefaultChoiceText;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<class UCommonRichTextBlock> CommonRichTextBlock_Text;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<class UCommonLazyImage> CommonLazyImage_ParticipantIcon;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<class UCommonRichTextBlock> CommonRichTextBlock_ParticipantName;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<class URecallConversationDialogChoiceWidget> CommonButton_Choice1;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<class URecallConversationDialogChoiceWidget> CommonButton_Choice2;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<class URecallConversationDialogChoiceWidget> CommonButton_Choice3;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<class URecallConversationDialogChoiceWidget> CommonButton_Choice4;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<class URecallConversationDialogChoiceWidget> CommonButton_Choice5;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<class URecallConversationDialogChoiceWidget> CommonButton_Choice6;

private:
	TObjectPtr<URecallConversationDialogChoiceWidget> GetChoiceButton(int32 Index) const;

	TSharedPtr<struct FStreamableHandle> ParticipantStreamableHandle;
	
	UFUNCTION()
	void OnParticipantAssetLoaded();
};
