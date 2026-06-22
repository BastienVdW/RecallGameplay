// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "ExtendedCommonUserWidget.h"

#include "RecallInteractEventWidget.generated.h"

/**
 * Widget to show interaction key and progress for local players.
 */
UCLASS(Abstract)
class RECALLGAMEPLAYREPRESENTATION_API URecallInteractEventWidget :
	public UExtendedCommonUserWidget
{
	GENERATED_BODY()

	// UUserWidget implementation Begin
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	// UUserWidget implementation End

public:
	UFUNCTION(BlueprintCallable)
	void SetInteractEvent(const FText& Text, bool bFailed);

protected:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<class UCommonActionWidget> CommonActionWidget_Input;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<class UCommonRichTextBlock> CommonRichTextBlock_Text;
};
