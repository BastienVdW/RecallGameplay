// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "ExtendedCommonUserWidget.h"

#include "RecallGameplayEffectWidget.generated.h"

/**
 * Widget to display a gameplay effect.
 */
UCLASS(Abstract)
class RECALLGAMEPLAYREPRESENTATION_API URecallGameplayEffectWidget :
	public UExtendedCommonUserWidget
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable, Category=GameplayEffect)
	void SetFromGameplayEffect(const FRecallGameplayEffectRepresentation& GameplayEffect);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=GameplayEffect)
	FString StackCountTextFormat = TEXT("{StackCount}"); 
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<class UCommonRichTextBlock> CommonRichTextBlock_Name;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<class UCommonRichTextBlock> CommonRichTextBlock_Description;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<class UCommonLazyImage> CommonLazyImage_Icon;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<class UCommonRichTextBlock> CommonRichTextBlock_StackCount;
	
};
