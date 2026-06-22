// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Device/RecallDeviceCostTypes.h"
#include "GameplayTagContainer.h"

#include "RecallDeviceItemCost.generated.h"

struct FRecallGameplayTagCountMap;

USTRUCT(DisplayName="Inventory")
struct RECALLDEVICEMODULE_API FRecallDeviceItemCost : public FRecallDeviceCostBase
{
	GENERATED_BODY()

public:
	virtual bool Evaluate(const FRecallDeviceCostContext& Context) const override;
	virtual void Consume(const FRecallDeviceCostContext& Context) const override;

protected:	
	UPROPERTY(EditAnywhere)
	bool bUseFactionInventory = false;
	
	UPROPERTY(EditAnywhere, meta=(GameplayTagFilter="Inventory"))
	FGameplayTag ItemTag;
	
	UPROPERTY(EditAnywhere, meta=(ClampMin=0, ClampMax=9999))
	int32 ItemCount = 1;
	
	UPROPERTY(EditAnywhere)
	bool bInvert = false;
	
protected:
	bool EvaluateInventoryCost(const FRecallDeviceCostContext& Context) const;
	TArray<FRecallGameplayTagCountMap*> GetGameplayTagCountMap(const FRecallDeviceCostContext& Context) const;
};
