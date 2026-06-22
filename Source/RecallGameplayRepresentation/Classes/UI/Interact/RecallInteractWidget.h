// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "ExtendedCommonUserWidget.h"
#include "Interact/RecallInteractReactInterface.h"

#include "RecallInteractWidget.generated.h"

/**
 * Widget to show interaction key and progress for local players.
 */
UCLASS(Abstract)
class RECALLGAMEPLAYREPRESENTATION_API URecallInteractWidget :
	public UExtendedCommonUserWidget,
	public IRecallInteractReactInterface
{
	GENERATED_BODY()

public:
	URecallInteractWidget();

	// UUserWidget implementation Begin
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	// UUserWidget implementation End

	// IRecallInteractReactInterface implementation Begin
public:
	virtual void SetInteraction(const FRecallInteractState& State, int32 PlayerIndex = INDEX_NONE) override;
	// IRecallInteractReactInterface implementation End

protected:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<class UProgressBar> ProgressBar_Interact;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<class UOverlay> Overlay_Interact;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<class URecallInteractEventWidget> InteractEvent_Primary;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<class URecallInteractEventWidget> InteractEvent_Secondary;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<class URecallInteractEventWidget> InteractEvent_Tertiary;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<class URecallInteractEventWidget> InteractEvent_Quaternary;
	
private:
	UPROPERTY(Transient)
	TObjectPtr<class UCanvasPanelSlot> CanvasPanelSlot;
	UPROPERTY(Transient)
	TMap<TObjectPtr<UInputAction>, FUIActionBindingHandle> ContextualActionBindingHandles;

	void SetContextual(const FRecallInteractProgressRepresentation& Progress,
		const FRecallInteractionRepresentation& Representation);
	
	FSimpleDelegate GetOnExecuteContextualActionDelegate(ERecallInteractInput Input);
	
	void OnExecuteContextualPrimary();
	void OnExecuteContextualSecondary();
	void OnExecuteContextualTertiary();
	void OnExecuteContextualQuaternary();

	void PushInputOption(const FString& Option);
	
	void SetInteractable(const FRecallInteractProgressRepresentation& Progress,
		const FRecallInteractionRepresentation& Representation, const FVector& Location);
	
	TObjectPtr<class URecallInteractEventWidget> GetInteractEventWidget(ERecallInteractInput Input) const;
};
