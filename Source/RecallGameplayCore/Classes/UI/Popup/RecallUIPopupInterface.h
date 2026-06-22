// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "RecallUIPopupInterface.generated.h"

UINTERFACE()
class RECALLGAMEPLAYCORE_API URecallUIPopupInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 * Interface for widgets to display popup on screen.
 */
class RECALLGAMEPLAYCORE_API IRecallUIPopupInterface
{
	GENERATED_IINTERFACE_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category=UI)
	void ShowPopup(const FVector& Location, const FText& Popup, float Duration);
};
