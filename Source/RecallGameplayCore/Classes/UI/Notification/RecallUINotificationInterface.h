// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "UObject/Interface.h"

#include "RecallUINotificationInterface.generated.h"

UINTERFACE()
class RECALLGAMEPLAYCORE_API URecallUINotificationInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 * Interface for widgets to display notifications on screen.
 */
class RECALLGAMEPLAYCORE_API IRecallUINotificationInterface
{
	GENERATED_IINTERFACE_BODY()
	
public:
	virtual void ShowNotification(const FDataTableRowHandle& Notification, const FString& ControllerID) {}
};
