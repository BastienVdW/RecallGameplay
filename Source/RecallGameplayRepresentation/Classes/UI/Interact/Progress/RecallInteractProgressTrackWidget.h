// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "ExtendedCommonUserWidget.h"
#include "Interact/RecallInteractProgressReactInterface.h"

#include "RecallInteractProgressTrackWidget.generated.h"

/**
 * Widget to track the progress of all the interactions.
 * 
 */
UCLASS(Abstract)
class RECALLGAMEPLAYREPRESENTATION_API URecallInteractProgressTrackWidget :
	public UExtendedCommonUserWidget,
	public IRecallInteractProgressReactInterface
{
	GENERATED_BODY()

	// UUserWidget implementation Begin
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	// UUserWidget implementation End
	
	// IRecallInteractProgressReactInterface implementation Begin
public:
	virtual void SetInteractions(const TArray<FRecallInteractProgress>& Interactions) override;
	// IRecallInteractProgressReactInterface implementation End

protected:
	UPROPERTY(EditAnywhere, Category=Progress)
	TSubclassOf<class URecallInteractProgressWidget> ProgressWidgetClass;
	
	UPROPERTY(EditAnywhere, Category=Progress)
	int32 MaxProgressWidgetCount = 10;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<class UCanvasPanel> CanvasPanel_Track;

protected:
	UPROPERTY(Transient)
	TArray<TObjectPtr<URecallInteractProgressWidget>> ProgressWidgets;
};
