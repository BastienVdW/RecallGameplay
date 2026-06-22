// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "StateTree/RecallStateTreeConditionBase.h"
#include "MassExtendedEntityHandle.h"

#include "RecallCharacterConditions.generated.h"

USTRUCT()
struct RECALLGAMEPLAY_API FRecallCharacterStandingConditionInstanceData
{
	GENERATED_BODY()
};
STATETREE_POD_INSTANCEDATA(FRecallCharacterStandingConditionInstanceData);

USTRUCT(DisplayName="Character Standing")
struct RECALLGAMEPLAY_API FRecallCharacterStandingCondition : public FRecallStateTreeConditionBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRecallCharacterStandingConditionInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;

	UPROPERTY(EditAnywhere, Category=Parameter)
	bool bInvert = false;
	
protected:
	TStateTreeExternalDataHandle<struct FRecallPhysicsBodyFragment> BodyFragmentHandle;
	TStateTreeExternalDataHandle<class URecallPhysicsSubsystem> PhysicsSystemHandle;
};
