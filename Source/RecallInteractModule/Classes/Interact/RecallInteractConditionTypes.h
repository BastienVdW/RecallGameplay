// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "RecallInteractContextTypes.h"

#include "RecallInteractConditionTypes.generated.h"

USTRUCT()
struct RECALLINTERACTMODULE_API FRecallInteractCondition
{
	GENERATED_BODY()
	
public:
	virtual ~FRecallInteractCondition() = default;
	virtual bool EvaluateCondition(const FRecallInteractContext& Context, FText& OutFailedText) const;

#if WITH_EDITORONLY_DATA
protected:
	/**
	 * Comment for other developers.
	 * Won't be used in-game, and won't be cooked.
	 */
	UPROPERTY(EditAnywhere, meta=(MultiLine))
	FString Comment;
#endif // WITH_EDITORONLY_DATA
};

