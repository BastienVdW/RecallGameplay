// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "ExtendedCommonUserWidget.h"

#include "RecallGameplayEffectListWidget.generated.h"

struct FRecallGameplayEffectRepresentation;

/**
 * Widget to display multiple gameplay effect as a list.
 */
UCLASS(Abstract)
class RECALLGAMEPLAYREPRESENTATION_API URecallGameplayEffectListWidget :
	public UExtendedCommonUserWidget
{
	GENERATED_UCLASS_BODY()

public:
	void SetFromGameplayEffects(const TArray<FRecallGameplayEffectRepresentation>& GameplayEffects) const;

protected:
	// UUserWidget interface
	virtual void NativePreConstruct() override;
	// End of UUserWidget interface

protected:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UPanelWidget> PanelWidget_List;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=GameplayEffect)
	TSubclassOf<class URecallGameplayEffectWidget> GameplayEffectWidgetClass;
};
