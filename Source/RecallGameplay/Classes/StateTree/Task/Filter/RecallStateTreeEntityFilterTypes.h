// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "Data/GameplayTag/RecallGameplayTagConditionTypes.h"

#include "RecallStateTreeEntityFilterTypes.generated.h"

struct FRecallStateTreeExecutionContext;
struct FMassExtendedEntityHandle;

USTRUCT()
struct RECALLGAMEPLAY_API FRecallStateTreeEntityFilterBase
{
	GENERATED_BODY()
	
	virtual ~FRecallStateTreeEntityFilterBase() = default;

	virtual void FilterEntities(const FRecallStateTreeExecutionContext& Context, TArray<FMassExtendedEntityHandle>& Entities) const
	{
		unimplemented();
	}
};

USTRUCT(DisplayName="Gameplay Tag")
struct RECALLGAMEPLAY_API FRecallStateTreeEntityFilterGameplayTag : public FRecallStateTreeEntityFilterBase
{
	GENERATED_BODY()

public:
	virtual void FilterEntities(const FRecallStateTreeExecutionContext& Context, TArray<FMassExtendedEntityHandle>& Entities) const override;

protected:
	UPROPERTY(EditAnywhere)
	FRecallGameplayTagCondition GameplayTagCondition;
};

UENUM()
enum class EFRecallStateTreeEntityFilterSelectType
{
	Distance,
	Random,
};

USTRUCT(DisplayName="Select")
struct RECALLGAMEPLAY_API FRecallStateTreeEntityFilterSelect : public FRecallStateTreeEntityFilterBase
{
	GENERATED_BODY()

public:
	virtual void FilterEntities(const FRecallStateTreeExecutionContext& Context, TArray<FMassExtendedEntityHandle>& Entities) const override;

protected:
	UPROPERTY(EditAnywhere)
	EFRecallStateTreeEntityFilterSelectType Selection = EFRecallStateTreeEntityFilterSelectType::Distance;
	
	UPROPERTY(EditAnywhere, meta=(ClampMin=1))
	int32 MaxCount = 1;

private:
	void Sort(const FRecallStateTreeExecutionContext& Context, TArray<FMassExtendedEntityHandle>& Entities) const;
	
	static void SortByDistance(const FRecallStateTreeExecutionContext& Context, TArray<FMassExtendedEntityHandle>& Entities);
	static void SortShuffle(const FRecallStateTreeExecutionContext& Context, TArray<FMassExtendedEntityHandle>& Entities);
};
