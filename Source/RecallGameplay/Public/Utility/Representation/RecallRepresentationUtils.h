// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"

struct FMassExtendedEntityHandle;
struct FGameplayTag;
class URecallRepresentationEventSubsystem;

namespace Recall::Representation::Utils
{

RECALLGAMEPLAY_API extern void SendUIEvent(
	const UObject* WorldContextObject, const FMassExtendedEntityHandle& Entity, const FGameplayTag& EventTag);

RECALLGAMEPLAY_API extern void PushUIPopup(URecallRepresentationEventSubsystem& EventSystem,
	const FText& Text, const FVector& Location, float Duration = 3.0f);
RECALLGAMEPLAY_API extern void PushUINotification(URecallRepresentationEventSubsystem& EventSystem,
	const FDataTableRowHandle& Notification, const FString& ControllerID);
	
} // namespace Recall::Representation::Utils
