// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"

#include "RecallConversationInputTypes.generated.h"

struct FConversationChoiceReference;

UENUM()
enum class ERecallConversationInputType : uint8
{
	None,
	Choice,
};

USTRUCT()
struct RECALLCONVERSATIONMODULE_API FRecallConversationInputCommand
{
	GENERATED_BODY()

	FRecallConversationInputCommand() = default;
	FRecallConversationInputCommand(const FString& Options);
	
	UPROPERTY(VisibleAnywhere)
	ERecallConversationInputType Type = ERecallConversationInputType::None;
	
	UPROPERTY(VisibleAnywhere)
	FGuid NodeGUID;

public:
	FString ToOptions() const;
	bool IsValid() const;
	FConversationChoiceReference ToChoiceReference() const;
};
