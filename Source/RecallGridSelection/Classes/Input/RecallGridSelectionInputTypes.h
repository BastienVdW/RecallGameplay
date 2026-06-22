// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "RecallGridSelectionInputTypes.generated.h"

UENUM(BlueprintType)
enum class ERecallGridSelectionInputType : uint8
{
	None			UMETA(Hidden),

	Select,
	Deselect,
};

USTRUCT(BlueprintType)
struct RECALLGRIDSELECTION_API FRecallGridSelectionInputCommand
{
	GENERATED_BODY()

public:
	FRecallGridSelectionInputCommand() = default;
	FRecallGridSelectionInputCommand(const FString& Options);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ERecallGridSelectionInputType Type = ERecallGridSelectionInputType::None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntVector GridPosition = FIntVector::ZeroValue;
	
public:
	FORCEINLINE bool IsValid() const { return Type != ERecallGridSelectionInputType::None; }
	FORCEINLINE bool IsSelect() const { return Type == ERecallGridSelectionInputType::Select; }
	FORCEINLINE bool IsDeselect() const { return Type == ERecallGridSelectionInputType::Deselect; }
	FString ToOptions() const;
};
