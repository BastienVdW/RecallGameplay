// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "ExtendedCommonUserWidget.h"

#include "RecallPopupWidget.generated.h"

/**
 * Popup widget displayed at a world location and fading away.
 */
UCLASS(Abstract)
class RECALLGAMEPLAYREPRESENTATION_API URecallPopupWidget :
	public UExtendedCommonUserWidget
{
	GENERATED_BODY()

	// UUserWidget implementation Begin
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	// UUserWidget implementation End

public:
	void ShowPopup(const FVector& Location, const FText& Popup, float Duration);

protected:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional), Category=Popup)
	TObjectPtr<class UCommonRichTextBlock> CommonRichTextBlock_Text;
	
	UPROPERTY(BlueprintReadOnly, Transient, meta=(BindWidgetAnimOptional))
	TObjectPtr<class UWidgetAnimation> Animation_PopupFadeIn;
	
	UPROPERTY(BlueprintReadOnly, Transient, meta=(BindWidgetAnimOptional))
	TObjectPtr<class UWidgetAnimation> Animation_PopupFadeOut;

protected:
	UPROPERTY(Transient)
	FVector PopupLocation = FVector::ZeroVector;
	UPROPERTY(Transient)
	FTimerHandle PopupTimerHandle;
	
	UFUNCTION()
	void OnPopupTimeOut();

	void UpdateViewportPosition() const;
};
