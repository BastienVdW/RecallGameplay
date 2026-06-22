// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "EnvQueryContext_Recall_Obstacles.h"

#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"
#include "EnvironmentQuery/EnvQueryTypes.h"

//----------------------------------------------------------------------//
// UEnvQueryContext_Recall_Obstacles
//----------------------------------------------------------------------//
void UEnvQueryContext_Recall_Obstacles::ProvideContext(FEnvQueryInstance& QueryInstance,
	FEnvQueryContextData& ContextData) const
{
	const float FObstacleCount = QueryInstance.NamedParams.FindRef(TEXT("ObstacleCount"));
	const int32 ObstacleCount = *((int32*)&FObstacleCount);

	TArray<FVector> ObstacleLocations;
	ObstacleLocations.SetNum(ObstacleCount);

	for (int32 ObstacleIndex = 0; ObstacleIndex < ObstacleCount; ++ObstacleIndex)
	{
		const float ObstacleLocationX = QueryInstance.NamedParams.FindRef(
			*FString::Printf(TEXT("ObstacleLocationX_%d"), ObstacleIndex));
		const float ObstacleLocationY = QueryInstance.NamedParams.FindRef(
			*FString::Printf(TEXT("ObstacleLocationY_%d"), ObstacleIndex));
		const float ObstacleLocationZ = QueryInstance.NamedParams.FindRef(
			*FString::Printf(TEXT("ObstacleLocationZ_%d"), ObstacleIndex));

		ObstacleLocations[ObstacleIndex] = FVector(
			ObstacleLocationX, ObstacleLocationY, ObstacleLocationZ);
	}
	
	UEnvQueryItemType_Point::SetContextHelper(ContextData, ObstacleLocations);
}
