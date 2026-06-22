// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"

#include "RecallDeviceCostTypes.generated.h"

struct FRecallDeviceCostContext;

USTRUCT()
struct RECALLDEVICEMODULE_API FRecallDeviceCostBase
{
	GENERATED_BODY()
	
	virtual ~FRecallDeviceCostBase() = default;
	virtual bool Evaluate(const FRecallDeviceCostContext& Context) const;
	virtual void Consume(const FRecallDeviceCostContext& Context) const;
};

USTRUCT(DisplayName="And")
struct RECALLDEVICEMODULE_API FRecallDeviceCostAnd : public FRecallDeviceCostBase
{
	GENERATED_BODY()
	
public:
	virtual bool Evaluate(const FRecallDeviceCostContext& Context) const override;
	virtual void Consume(const FRecallDeviceCostContext& Context) const override;

protected:
	UPROPERTY(EditAnywhere, meta=(BaseStruct="/Script/RecallDeviceModule.RecallDeviceConditionBase", ExcludeBaseStruct))
	TArray<FInstancedStruct> Costs;
};


USTRUCT(DisplayName="Or")
struct RECALLDEVICEMODULE_API FRecallDeviceCostOr : public FRecallDeviceCostBase
{
	GENERATED_BODY()

public:
	virtual bool Evaluate(const FRecallDeviceCostContext& Context) const override;
	virtual void Consume(const FRecallDeviceCostContext& Context) const override;

protected:
	UPROPERTY(EditAnywhere, meta=(BaseStruct="/Script/RecallDeviceModule.RecallDeviceConditionBase", ExcludeBaseStruct))
	TArray<FInstancedStruct> Costs;
	
protected:
	bool Evaluate_Internal(const FRecallDeviceCostContext& Context, bool bConsume = false) const;
};
