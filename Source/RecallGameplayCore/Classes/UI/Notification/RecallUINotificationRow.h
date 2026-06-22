// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Engine/DataTable.h"

#include "RecallUINotificationRow.generated.h"

USTRUCT()
struct RECALLGAMEPLAYCORE_API FRecallUINotificationRow : public FTableRowBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	FText NotificationText;
	
	UPROPERTY(EditAnywhere, meta=(Units=Seconds))
	float Duration = 5.0f;
	
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<class UTexture2D> IconTexture;
};
