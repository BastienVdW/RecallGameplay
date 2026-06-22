// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallCrowdAgent.h"

FRecallCrowdAgent::FRecallCrowdAgent(UObject& InOwner, const FRecallCrowdAgentSettings& Settings)
	: Owner(InOwner)
	, AgentSettings(Settings)
{
}

void FRecallCrowdAgent::ApplyCrowdAgentVelocity(const FVector& NewVelocity, const FVector& DestPathCorner,
	bool bTraversingLink, bool bIsNearEndOfPath)
{
	if (bTraversingLink)
	{
		return;
	}
	
	AgentBlackboard.VelocityCentimetersPerSecond = NewVelocity;
	AgentBlackboard.bDirtyVelocity = true;
}

void FRecallCrowdAgent::ApplyCrowdAgentPosition(const FVector& NewPosition)
{
	AgentBlackboard.Location = NewPosition;
	AgentBlackboard.bDirtyLocation = true;
}

void FRecallCrowdAgent::StartUsingCustomLink(INavLinkCustomInterface* CustomNavLink, const FVector& DestPoint)
{
	unimplemented();
}

void FRecallCrowdAgent::OnNavNodeChanged(NavNodeRef NewPolyRef, NavNodeRef PrevPolyRef, int32 CorridorSize)
{
}

bool FRecallCrowdAgent::ShouldTrackMovingGoal(FVector& OutGoalLocation) const
{
	return false;
}

void FRecallCrowdAgent::UpdateDestinationForMovingGoal(const FVector& NewDestination)
{
	checkNoEntry();
}

FVector FRecallCrowdAgent::GetCrowdAgentLocation() const
{
	return AgentBlackboard.Location;
}

FVector FRecallCrowdAgent::GetCrowdAgentVelocity() const
{
	return AgentBlackboard.VelocityCentimetersPerSecond;
}

void FRecallCrowdAgent::GetCrowdAgentCollisions(float& CylinderRadius, float& CylinderHalfHeight) const
{
	CylinderRadius = AgentBlackboard.CylinderRadius;
	CylinderHalfHeight = AgentBlackboard.CylinderHalfHeight;
}

float FRecallCrowdAgent::GetCrowdAgentMaxSpeed() const
{
	return AgentBlackboard.MaxSpeedCentimetersPerSecond;
}

int32 FRecallCrowdAgent::GetCrowdAgentAvoidanceGroup() const
{
	return 1;
}

int32 FRecallCrowdAgent::GetCrowdAgentGroupsToAvoid() const
{
	return MAX_int32;
}

int32 FRecallCrowdAgent::GetCrowdAgentGroupsToIgnore() const
{
	return 0;
}
