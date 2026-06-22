// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CommonButtonBase.h"
#include "ConversationTypes.h"

#include "RecallConversationDialogChoiceWidget.generated.h"

/**
 * Widget to show a conversation dialog choice.
 */
UCLASS(Abstract)
class RECALLGAMEPLAYREPRESENTATION_API URecallConversationDialogChoiceWidget : public UCommonButtonBase
{
	GENERATED_BODY()

	// UCommonButtonBase implementation Begin
public:
	virtual void NativeOnClicked() override;
	// UCommonButtonBase implementation End
	
public:
	void SetConversationChoice(const FClientConversationOptionEntry& OptionEntry, int32 InPlayerIndex);
	
protected:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<class UCommonRichTextBlock> CommonRichTextBlock_Text;

	UPROPERTY(Transient)
	FConversationChoiceReference ChoiceReference;
	UPROPERTY(Transient)
	int32 PlayerIndex = 0;
};
