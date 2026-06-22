// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "ExtendedCommonUserWidget.h"
#include "Observer/Representation/RecallRepresentationReactInterface.h"

#include "RecallGameRuleTimerWidget.generated.h"

class UProgressBar;

UCLASS(Abstract)
class RECALLGAMEPLAYREPRESENTATION_API URecallGameRuleTimerWidget :
	public UExtendedCommonUserWidget,
	public IRecallRepresentationReactInterface
{
	GENERATED_BODY()

	// UUserWidget implementation Begin
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	// UUserWidget implementation End

	// IRecallRepresentationReactInterface implementation Begin
	virtual void OnRender() override;
	// IRecallRepresentationReactInterface implementation End

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Match Timer")
	TObjectPtr<UProgressBar> ProgressBar_MatchTimer;

	/**
	 * If true, progress bar starts at 1.0 and counts down to 0.0 (default).
	 * If false, progress bar starts at 0.0 and counts up to 1.0.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Match Timer")
	bool bProgressBarCountsDown = true;

private:
	UPROPERTY(Transient)
	float CachedMatchTimeSeconds = 0.0f;
	
	UPROPERTY(Transient)
	float CachedMatchDuration = 0.0f;
	
	void UpdateMatchTimer();
};
