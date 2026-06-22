// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "ExtendedCommonUserWidget.h"
#include "UI/Popup/RecallUIPopupInterface.h"

#include "RecallPopupCanvasWidget.generated.h"

/**
 * Canvas to display popup widgets at a specific world location.
 */
UCLASS(Abstract)
class RECALLGAMEPLAYREPRESENTATION_API URecallPopupCanvasWidget :
	public UExtendedCommonUserWidget,
	public IRecallUIPopupInterface
{
	GENERATED_BODY()

	// IRecallUIPopupInterface implementation Begin
public:
	virtual void ShowPopup_Implementation(
		const FVector& Location, const FText& Popup, float Duration) override;
	// IRecallUIPopupInterface implementation End

	// UUserWidget implementation Begin
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	// UUserWidget implementation End
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Popup)
	TSubclassOf<class URecallPopupWidget> PopupWidgetClass;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<class UCanvasPanel> CanvasPanel_Popup;

};
