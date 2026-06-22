// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "EnvQueryContext_Recall_Target.h"

#include "EnvQueryItemType_Recall_Entity.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "System/EQS/RecallEnvQueryTypes.h"

//----------------------------------------------------------------------//
// UEnvQueryContext_Recall_Target
//----------------------------------------------------------------------//
void UEnvQueryContext_Recall_Target::ProvideContext(FEnvQueryInstance& QueryInstance,
	FEnvQueryContextData& ContextData) const
{
	const float TargetLocationX = QueryInstance.NamedParams.FindRef(RECALL_ENV_QUERY_TARGET_LOCATION_X);
	const float TargetLocationY = QueryInstance.NamedParams.FindRef(RECALL_ENV_QUERY_TARGET_LOCATION_Y);
	const float TargetLocationZ = QueryInstance.NamedParams.FindRef(RECALL_ENV_QUERY_TARGET_LOCATION_Z);
	
	const float TargetRotationYaw = QueryInstance.NamedParams.FindRef(RECALL_ENV_QUERY_TARGET_ROTATION_YAW);
	const float TargetRotationPitch = QueryInstance.NamedParams.FindRef(RECALL_ENV_QUERY_TARGET_ROTATION_PITCH);
	const float TargetRotationRoll = QueryInstance.NamedParams.FindRef(RECALL_ENV_QUERY_TARGET_ROTATION_ROLL);

	const FVector TargetLocation(TargetLocationX, TargetLocationY, TargetLocationZ);
	const FRotator TargetRotation(TargetRotationPitch, TargetRotationYaw, TargetRotationRoll);
	const FTransform TargetTransform(TargetRotation, TargetLocation);

	ContextData.ValueType = UEnvQueryItemType_Recall_Entity::StaticClass();
	ContextData.NumValues = 1;
	ContextData.RawData.SetNumUninitialized(sizeof(FRecallEnvQueryEntityInfo));

	UEnvQueryItemType_Recall_Entity::SetValue((uint8*)ContextData.RawData.GetData(), TargetTransform);
}
