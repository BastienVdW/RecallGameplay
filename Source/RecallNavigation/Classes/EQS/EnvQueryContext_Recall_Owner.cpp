// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "EnvQueryContext_Recall_Owner.h"

#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvQueryItemType_Recall_Entity.h"
#include "System/EQS/RecallEnvQueryTypes.h"

//----------------------------------------------------------------------//
// UEnvQueryContext_Recall_Owner
//----------------------------------------------------------------------//
void UEnvQueryContext_Recall_Owner::ProvideContext(FEnvQueryInstance& QueryInstance,
	FEnvQueryContextData& ContextData) const
{
	const float OwnerLocationX = QueryInstance.NamedParams.FindRef(RECALL_ENV_QUERY_OWNER_LOCATION_X);
	const float OwnerLocationY = QueryInstance.NamedParams.FindRef(RECALL_ENV_QUERY_OWNER_LOCATION_Y);
	const float OwnerLocationZ = QueryInstance.NamedParams.FindRef(RECALL_ENV_QUERY_OWNER_LOCATION_Z);
	
	const float OwnerRotationYaw = QueryInstance.NamedParams.FindRef(RECALL_ENV_QUERY_OWNER_ROTATION_YAW);
	const float OwnerRotationPitch = QueryInstance.NamedParams.FindRef(RECALL_ENV_QUERY_OWNER_ROTATION_PITCH);
	const float OwnerRotationRoll = QueryInstance.NamedParams.FindRef(RECALL_ENV_QUERY_OWNER_ROTATION_ROLL);

	const FVector OwnerLocation(OwnerLocationX, OwnerLocationY, OwnerLocationZ);
	const FRotator OwnerRotation(OwnerRotationPitch, OwnerRotationYaw, OwnerRotationRoll);
	const FTransform OwnerTransform(OwnerRotation, OwnerLocation);

	ContextData.ValueType = UEnvQueryItemType_Recall_Entity::StaticClass();
	ContextData.NumValues = 1;
	ContextData.RawData.SetNumUninitialized(sizeof(FRecallEnvQueryEntityInfo));

	UEnvQueryItemType_Recall_Entity::SetValue((uint8*)ContextData.RawData.GetData(), OwnerTransform);
}
