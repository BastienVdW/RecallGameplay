// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "EnvironmentQuery/Items/EnvQueryItemType_VectorBase.h"

#include "EnvQueryItemType_Recall_Entity.generated.h"

struct FRecallEnvQueryEntityInfo;

UCLASS(meta=(DisplayName="Recall Entity Item Type"), MinimalAPI)
class UEnvQueryItemType_Recall_Entity : public UEnvQueryItemType_VectorBase
{
	GENERATED_BODY()

public:
	typedef FRecallEnvQueryEntityInfo FValueType;

	UEnvQueryItemType_Recall_Entity();

	static const FRecallEnvQueryEntityInfo& GetValue(const uint8* RawData);
	static void SetValue(uint8* RawData, const FRecallEnvQueryEntityInfo& Value);

	virtual FVector GetItemLocation(const uint8* RawData) const override;
	virtual FRotator GetItemRotation(const uint8* RawData) const override;
};
