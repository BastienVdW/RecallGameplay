// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "EnvQueryContext_Recall_ViewTarget.h"

#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvQueryItemType_Recall_Entity.h"
#include "System/EQS/RecallEnvQueryTypes.h"

//----------------------------------------------------------------------//
// UEnvQueryContext_Recall_ViewTarget
//----------------------------------------------------------------------//
void UEnvQueryContext_Recall_ViewTarget::ProvideContext(FEnvQueryInstance& QueryInstance,
	FEnvQueryContextData& ContextData) const
{
	const float ViewTargetLocationX = QueryInstance.NamedParams.FindRef(RECALL_ENV_QUERY_VIEW_TARGET_LOCATION_X);
	const float ViewTargetLocationY = QueryInstance.NamedParams.FindRef(RECALL_ENV_QUERY_VIEW_TARGET_LOCATION_Y);
	const float ViewTargetLocationZ = QueryInstance.NamedParams.FindRef(RECALL_ENV_QUERY_VIEW_TARGET_LOCATION_Z);
	
	const float ViewTargetRotationYaw = QueryInstance.NamedParams.FindRef(RECALL_ENV_QUERY_VIEW_TARGET_ROTATION_YAW);
	const float ViewTargetRotationPitch = QueryInstance.NamedParams.FindRef(RECALL_ENV_QUERY_VIEW_TARGET_ROTATION_PITCH);
	const float ViewTargetRotationRoll = QueryInstance.NamedParams.FindRef(RECALL_ENV_QUERY_VIEW_TARGET_ROTATION_ROLL);

	const FVector ViewTargetLocation(ViewTargetLocationX, ViewTargetLocationY, ViewTargetLocationZ);
	const FRotator ViewTargetRotation(ViewTargetRotationPitch, ViewTargetRotationYaw, ViewTargetRotationRoll);
	const FTransform ViewTargetTransform(ViewTargetRotation, ViewTargetLocation);

	ContextData.ValueType = UEnvQueryItemType_Recall_Entity::StaticClass();
	ContextData.NumValues = 1;
	ContextData.RawData.SetNumUninitialized(sizeof(FRecallEnvQueryEntityInfo));

	UEnvQueryItemType_Recall_Entity::SetValue((uint8*)ContextData.RawData.GetData(), ViewTargetTransform);
}
