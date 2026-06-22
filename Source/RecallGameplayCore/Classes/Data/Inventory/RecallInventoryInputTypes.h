// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "RecallInventoryInputTypes.generated.h"

UENUM(BlueprintType)
enum class ERecallInventoryInputType : uint8
{
	None			UMETA(Hidden),

	UseItem,
};

USTRUCT(BlueprintType)
struct RECALLGAMEPLAYCORE_API FRecallInventoryInputCommand
{
	GENERATED_BODY()

public:
	FRecallInventoryInputCommand() = default;
	FRecallInventoryInputCommand(const FString& Options);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ERecallInventoryInputType Type = ERecallInventoryInputType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(GameplayTagFilter="Inventory"))
	FGameplayTag ItemTag;
	
public:
	FORCEINLINE bool IsValid() const { return Type != ERecallInventoryInputType::None; }
	FString ToOptions() const;
};


