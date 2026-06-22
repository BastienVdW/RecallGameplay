// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallCrowdSnapshotTypes.h"

#include "Crowd/RecallCrowdManager.h"

#if WITH_RECAST
#include "DetourCrowd/DetourCrowd.h"
#endif // WITH_RECAST

#if WITH_RECAST
void FRecallDetourCrowdAgent::Save(const dtCrowdAgent* CrowdAgent)
{
	DesiredSpeed = CrowdAgent->desiredSpeed;
}

void FRecallDetourCrowdAgent::Restore(dtCrowdAgent* CrowdAgent) const
{
	CrowdAgent->desiredSpeed = DesiredSpeed;
}
#endif // WITH_RECAST

FRecallCrowdAgentDataSnapshot::FRecallCrowdAgentDataSnapshot(const FRecallCrowdAgentData& AgentData)
	: PrevPoly(AgentData.PrevPoly)
	, AgentIndex(AgentData.AgentIndex)
	, PathOptRemainingTime(AgentData.PathOptRemainingTime)
	, bIsSimulated(AgentData.bIsSimulated)
	, bWantsPathOptimization(AgentData.bWantsPathOptimization)
{
}

FRecallCrowdAgentDataSnapshot::operator FRecallCrowdAgentData() const
{
	FRecallCrowdAgentData AgentData;
	AgentData.PrevPoly = PrevPoly;
	AgentData.AgentIndex = AgentIndex;
	AgentData.PathOptRemainingTime = PathOptRemainingTime;
	AgentData.bIsSimulated = bIsSimulated;
	AgentData.bWantsPathOptimization = bWantsPathOptimization;

	return AgentData;
}
