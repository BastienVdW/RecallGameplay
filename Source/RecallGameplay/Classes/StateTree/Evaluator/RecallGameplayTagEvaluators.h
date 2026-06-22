// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "StateTree/RecallStateTreeEvaluatorBase.h"
#include "StateTreeExecutionTypes.h"

#include "RecallGameplayTagEvaluators.generated.h"

USTRUCT()
struct RECALLGAMEPLAY_API FRecallFactionEvaluatorInstanceData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category=Output)
	FGameplayTagContainer FactionTags;

	UPROPERTY(VisibleAnywhere, Category=Output)
	FGameplayTagContainer EnemyFactionTags;
};

USTRUCT(meta=(DisplayName="Faction"))
struct RECALLGAMEPLAY_API FRecallFactionEvaluator : public FRecallStateTreeEvaluatorBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallFactionEvaluatorInstanceData;

protected:
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual void TreeStart(FStateTreeExecutionContext& Context) const override;
	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	
protected:
	TStateTreeExternalDataHandle<struct FRecallGameplayTagFragment, EStateTreeExternalDataRequirement::Optional> GameplayTagFragmentHandle;
};
