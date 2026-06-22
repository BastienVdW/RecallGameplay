// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallConversationDialogWidget.h"

#include "CommonLazyImage.h"
#include "CommonRichTextBlock.h"
#include "Data/Conversation/RecallConversationParticipantAsset.h"
#include "Engine/AssetManager.h"
#include "RecallConversationDialogChoiceWidget.h"
#include "System/Conversation/RecallConversationParticipantSubsystem.h"

#define LOCTEXT_NAMESPACE "RecallConversationDialogWidget"

URecallConversationDialogWidget::URecallConversationDialogWidget()
	: Super()
{
	DefaultChoiceText = NSLOCTEXT("RecallConversationDialogWidget", "DefaultChoice", "End");
}

void URecallConversationDialogWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void URecallConversationDialogWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (ParticipantStreamableHandle.IsValid())
	{
		ParticipantStreamableHandle->CancelHandle();
		ParticipantStreamableHandle.Reset();
	}
}

void URecallConversationDialogWidget::SetConversationMessage(const FClientConversationMessagePayload& Message)
{
	SetConversationDialog(Message.Message.SpeakerID, Message.Message.Text);
	
	TArray<FClientConversationOptionEntry> Options = Message.Options;
	if (Options.Num() == 0)
	{
		FClientConversationOptionEntry& DefaultOption = Options.AddDefaulted_GetRef();
		DefaultOption.ChoiceText = DefaultChoiceText;
	}
	
	for (int32 ChoiceIndex = 0; ChoiceIndex < RECALL_CONVERSATION_DIALOG_CHOICE_BUTTON_COUNT; ChoiceIndex++)
	{
		const TObjectPtr<URecallConversationDialogChoiceWidget> Button = GetChoiceButton(ChoiceIndex);
		if (!Button)
		{
			continue;
		}

		if (Options.IsValidIndex(ChoiceIndex))
		{
			Button->SetConversationChoice(Options[ChoiceIndex], GetOwningLocalPlayerIndex());
			Button->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			Button->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void URecallConversationDialogWidget::SetConversationDialog(const FGameplayTag& ParticipantID,
	const FText& DialogText)
{
	if (ParticipantStreamableHandle.IsValid())
	{
		ParticipantStreamableHandle->CancelHandle();
		ParticipantStreamableHandle.Reset();
	}
	
	if (CommonLazyImage_ParticipantIcon)
	{
		CommonLazyImage_ParticipantIcon->SetVisibility(ESlateVisibility::Hidden);
	}
		
	if (CommonRichTextBlock_ParticipantName)
	{
		CommonRichTextBlock_ParticipantName->SetVisibility(ESlateVisibility::Hidden);
	}
	
	if (UAssetManager* AssetManager = UAssetManager::GetIfInitialized())
	{
		const auto& ParticipantSystem = URecallConversationParticipantSubsystem::GetRef(GetWorld());
		const TSoftObjectPtr<URecallConversationParticipantAsset> ParticipantAssetPath = ParticipantSystem.GetParticipantAssetPath(
			ParticipantID);

		if (!ParticipantAssetPath.IsNull())
		{
			ParticipantStreamableHandle = AssetManager->GetStreamableManager().RequestAsyncLoad(
				ParticipantAssetPath.ToSoftObjectPath(),
				FStreamableDelegate::CreateUObject(this, &ThisClass::OnParticipantAssetLoaded));
		}
	}
	
	if (CommonRichTextBlock_Text)
	{
		CommonRichTextBlock_Text->SetText(DialogText);
	}
}

TObjectPtr<URecallConversationDialogChoiceWidget> URecallConversationDialogWidget::GetChoiceButton(int32 Index) const
{
	switch (Index)
	{
	case 0:
		return CommonButton_Choice1;
		
	case 1:
		return CommonButton_Choice2;
		
	case 2:
		return CommonButton_Choice3;
		
	case 3:
		return CommonButton_Choice4;
		
	case 4:
		return CommonButton_Choice5;
		
	case 5:
		return CommonButton_Choice6;

	default:
		checkNoEntry();
		return nullptr;
	}
}

void URecallConversationDialogWidget::OnParticipantAssetLoaded()
{
	if (!ensure(ParticipantStreamableHandle.IsValid()))
	{
		return;
	}

	const URecallConversationParticipantAsset* ParticipantAsset = Cast<URecallConversationParticipantAsset>(
		ParticipantStreamableHandle->GetLoadedAsset());
	ParticipantStreamableHandle.Reset();	
	if (!ParticipantAsset)
	{
		return;
	}
	
	if (CommonLazyImage_ParticipantIcon)
	{
		CommonLazyImage_ParticipantIcon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		CommonLazyImage_ParticipantIcon->SetBrushFromLazyTexture(ParticipantAsset->ParticipantIcon);
	}
		
	if (CommonRichTextBlock_ParticipantName)
	{
		CommonRichTextBlock_ParticipantName->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		CommonRichTextBlock_ParticipantName->SetText(ParticipantAsset->ParticipantName);
	}
}

#undef LOCTEXT_NAMESPACE
