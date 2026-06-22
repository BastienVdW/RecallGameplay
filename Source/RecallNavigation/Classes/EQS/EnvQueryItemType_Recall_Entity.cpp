// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#include "EnvQueryItemType_Recall_Entity.h"

#include "EnvironmentQuery/EnvQueryTypes.h"
#include "System/EQS/RecallEnvQueryTypes.h"

UEnvQueryItemType_Recall_Entity::UEnvQueryItemType_Recall_Entity()
{
	ValueSize = sizeof(FRecallEnvQueryEntityInfo);
}

const FRecallEnvQueryEntityInfo& UEnvQueryItemType_Recall_Entity::GetValue(const uint8* RawData)
{
	return GetValueFromMemory<FRecallEnvQueryEntityInfo>(RawData);
}

void UEnvQueryItemType_Recall_Entity::SetValue(uint8* RawData, const FRecallEnvQueryEntityInfo& Value)
{
	return SetValueInMemory<FRecallEnvQueryEntityInfo>(RawData, Value);
}

FVector UEnvQueryItemType_Recall_Entity::GetItemLocation(const uint8* RawData) const
{
	const FRecallEnvQueryEntityInfo& EntityInfo = GetValue(RawData);	
	return EntityInfo.CachedTransform.GetLocation();
}

FRotator UEnvQueryItemType_Recall_Entity::GetItemRotation(const uint8* RawData) const
{
	const FRecallEnvQueryEntityInfo& EntityInfo = GetValue(RawData);
	return EntityInfo.CachedTransform.GetRotation().Rotator();
}
