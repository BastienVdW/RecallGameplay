// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallConversationDialogChoiceWidget.h"

#include "CommonRichTextBlock.h"
#include "ConversationTypes.h"
#include "Utility/Conversation/RecallConversationFunctionLibrary.h"

void URecallConversationDialogChoiceWidget::NativeOnClicked()
{
	Super::NativeOnClicked();

	URecallConversationFunctionLibrary::SetInputConversationCommand(
		this, ChoiceReference, PlayerIndex);
}

void URecallConversationDialogChoiceWidget::SetConversationChoice(
	const FClientConversationOptionEntry& OptionEntry, int32 InPlayerIndex)
{
	PlayerIndex = InPlayerIndex;
	ChoiceReference = OptionEntry.ChoiceReference;
	
	if (CommonRichTextBlock_Text)
	{
		CommonRichTextBlock_Text->SetText(OptionEntry.ChoiceText);
	}
}
