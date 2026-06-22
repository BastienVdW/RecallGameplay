// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "ExtendedCommonUserWidget.h"
#include "Data/Interact/RecallInteractTypes.h"

#include "RecallInteractProgressWidget.generated.h"

/**
 * Widget to track the progress of all the interactions.
 * 
 */
UCLASS(Abstract)
class RECALLGAMEPLAYREPRESENTATION_API URecallInteractProgressWidget :
	public UExtendedCommonUserWidget
{
	GENERATED_BODY()

	// UUserWidget implementation Begin
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	// UUserWidget implementation End

public:
	void SetInteraction(const FRecallInteractProgress& Interaction);
	
protected:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<class UProgressBar> ProgressBar_Interact;

private:
	UPROPERTY(Transient)
	TObjectPtr<class UCanvasPanelSlot> CanvasPanelSlot;

};
