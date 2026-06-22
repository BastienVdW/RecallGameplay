// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "StateTree/Player/RecallPlayerConditionBase.h"

#include "RecallInteractPlayerConditions.generated.h"

USTRUCT()
struct RECALLINTERACTMODULE_API FRecallStateTreeClosestInteractConditionInstanceData
{
	GENERATED_BODY()
};
STATETREE_POD_INSTANCEDATA(FRecallStateTreeClosestInteractConditionInstanceData);

USTRUCT(DisplayName="Has Interact Target")
struct RECALLINTERACTMODULE_API FRecallStateTreeClosestInteractCondition : public FRecallPlayerConditionBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallStateTreeClosestInteractConditionInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;

	UPROPERTY(EditAnywhere, Category="Parameter")
	bool bInvert = false;

protected:
	TStateTreeExternalDataHandle<struct FRecallInteractorFragment> InteractorFragmentHandle;
};
