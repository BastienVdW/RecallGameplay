// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "Crowd/RecallCrowdAgentMutableData.h"
#include "System/Crowd/RecallCrowdAgentHandle.h"

#include "RecallCrowdSnapshotTypes.generated.h"

struct FRecallCrowdAgentData;

#if WITH_RECAST
struct dtCrowdAgent;
#endif // WITH_RECAST

USTRUCT()
struct FRecallDetourCrowdAgent
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere)
	float DesiredSpeed = 0.0f;
	
#if WITH_RECAST
	void Save(const dtCrowdAgent* CrowdAgent);
	void Restore(dtCrowdAgent* CrowdAgent) const;
#endif // WITH_RECAST
};

USTRUCT()
struct FRecallCrowdAgentDataSnapshot
{
	GENERATED_BODY()
	
	FRecallCrowdAgentDataSnapshot() = default;
	FRecallCrowdAgentDataSnapshot(const FRecallCrowdAgentData& AgentData);
	
	/** poly ref that agent is standing on from previous update */
	UPROPERTY(VisibleAnywhere)
	uint64 PrevPoly = 0;

	/** index of agent in detour crowd */
	UPROPERTY(VisibleAnywhere)
	int32 AgentIndex = INDEX_NONE;

	/** remaining time for next path optimization */
	UPROPERTY(VisibleAnywhere)
	float PathOptRemainingTime = 0.0f;

	/** is this agent fully simulated by crowd? */
	UPROPERTY(VisibleAnywhere)
	uint8 bIsSimulated = true;

	/** if set, agent wants path optimizations */
	UPROPERTY(VisibleAnywhere)
	uint8 bWantsPathOptimization = true;

	operator FRecallCrowdAgentData() const;
	
};

USTRUCT()
struct FRecallCrowdAgentSnapshot
{
	GENERATED_BODY()

	FRecallCrowdAgentSnapshot() = default;

	UPROPERTY(VisibleAnywhere)
	FRecallCrowdAgentHandle AgentHandle;

	UPROPERTY(VisibleAnywhere)
	FRecallCrowdAgentDataSnapshot AgentData;

	UPROPERTY(VisibleAnywhere)
	FRecallCrowdAgentMutableData AgentMutableData;
	
	UPROPERTY(VisibleAnywhere)
	FVector MoveDirection = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere)
	FRecallDetourCrowdAgent DetourCrowdAgent;
};

USTRUCT()
struct FRecallCrowdSnapshot
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	uint32 CrowdAgentUniqueIdGenerator = 0;

	UPROPERTY(VisibleAnywhere)
	TArray<FRecallCrowdAgentSnapshot> CrowdAgents;
};
