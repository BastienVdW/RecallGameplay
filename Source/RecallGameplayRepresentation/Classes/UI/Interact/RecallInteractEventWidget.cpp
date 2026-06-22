// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallInteractEventWidget.h"

#include "CommonActionWidget.h"
#include "CommonRichTextBlock.h"

void URecallInteractEventWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void URecallInteractEventWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void URecallInteractEventWidget::SetInteractEvent(const FText& Text, bool bFailed)
{
	/*
	if (CommonActionWidget_Input)
	{
		CommonActionWidget_Input->SetVisibility(bFailed ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
	}
	*/
	
	if (CommonRichTextBlock_Text)
	{
		CommonRichTextBlock_Text->SetText(Text);
		CommonRichTextBlock_Text->SetDefaultColorAndOpacity(bFailed ? FColor::Red : FColor::White);
	}
}
