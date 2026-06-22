// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "Navigation/CrowdAgentInterface.h"
#include "RecallCrowdAgentBlackboard.h"
#include "RecallCrowdAgentSettings.h"
#include "RecallCrowdAgentMutableData.h"

class INavLinkCustomInterface;

class FRecallCrowdAgent : public ICrowdAgentInterface
{
public:
	FRecallCrowdAgent(UObject& InOwner, const FRecallCrowdAgentSettings& Settings = {});

public:
	/** pass agent velocity to movement component */
	virtual void ApplyCrowdAgentVelocity(const FVector& NewVelocity, const FVector& DestPathCorner, bool bTraversingLink, bool bIsNearEndOfPath);
	
	/** pass desired position to movement component (after resolving collisions between crowd agents) */
	virtual void ApplyCrowdAgentPosition(const FVector& NewPosition);

	/** next path segment if custom nav link, try passing control to it */
	virtual void StartUsingCustomLink(INavLinkCustomInterface* CustomNavLink, const FVector& DestPoint);

	/** called when agent moved to next nav node (poly) */
	virtual void OnNavNodeChanged(NavNodeRef NewPolyRef, NavNodeRef PrevPolyRef, int32 CorridorSize);

	virtual bool ShouldTrackMovingGoal(FVector& OutGoalLocation) const;
	
	void UpdateDestinationForMovingGoal(const FVector& NewDestination);

	FORCEINLINE bool IsCrowdSlowdownAtGoalEnabled() const { return static_cast<bool>(AgentSettings.bEnableSlowdownAtGoal); }
	FORCEINLINE bool IsCrowdPathOffsetEnabled() const { return static_cast<bool>(AgentSettings.bEnablePathOffset); }
	FORCEINLINE bool IsCrowdOptimizeVisibilityEnabled() const { return static_cast<bool>(AgentSettings.bEnableOptimizeVisibility); /** don't check suspend here! */ }
	bool IsCrowdSimulationEnabled() const { return MutableData.SimulationState == ERecallCrowdSimulationState::Enabled; }
	FORCEINLINE bool IsCrowdSimulatioSuspended() const { return static_cast<bool>(MutableData.bSuspendCrowdSimulation); }
	FORCEINLINE bool IsCrowdAnticipateTurnsEnabled() const { return static_cast<bool>(AgentSettings.bEnableAnticipateTurns); }
	FORCEINLINE bool IsCrowdObstacleAvoidanceEnabled() const { return static_cast<bool>(AgentSettings.bEnableObstacleAvoidance); }
	FORCEINLINE bool IsCrowdSeparationEnabled() const { return static_cast<bool>(AgentSettings.bEnableSeparation); }
	FORCEINLINE bool IsCrowdOptimizeTopologyEnabled() const { return static_cast<bool>(AgentSettings.bEnableOptimizeTopology); }
	FORCEINLINE bool IsCrowdSimulationActive() const { return IsCrowdSimulationEnabled() && !IsCrowdSimulatioSuspended(); }
	/** checks if bEnableAnticipateTurns is set to true, and if crowd simulation is not suspended */
	FORCEINLINE bool IsCrowdAnticipateTurnsActive() const { return IsCrowdAnticipateTurnsEnabled() && !IsCrowdSimulatioSuspended(); }
	/** checks if bEnableObstacleAvoidance is set to true, and if crowd simulation is not suspended */
	FORCEINLINE bool IsCrowdObstacleAvoidanceActive() const { return IsCrowdObstacleAvoidanceEnabled() && !IsCrowdSimulatioSuspended(); }
	/** checks if bEnableSeparation is set to true, and if crowd simulation is not suspended */
	FORCEINLINE bool IsCrowdSeparationActive() const { return IsCrowdSeparationEnabled() && !IsCrowdSimulatioSuspended(); }
	/** checks if bEnableOptimizeTopology is set to true, and if crowd simulation is not suspended */
	FORCEINLINE bool IsCrowdOptimizeTopologyActive() const { return IsCrowdOptimizeTopologyEnabled() && !IsCrowdSimulatioSuspended(); }
	
	FORCEINLINE float GetCrowdSeparationWeight() const { return AgentSettings.SeparationWeight; }
	FORCEINLINE float GetCrowdCollisionQueryRange() const { return AgentSettings.CollisionQueryRange; }
	FORCEINLINE float GetCrowdPathOptimizationRange() const { return AgentSettings.PathOptimizationRange; }
	FORCEINLINE ERecallCrowdAvoidanceQuality GetCrowdAvoidanceQuality() const { return AgentSettings.AvoidanceQuality; }
	FORCEINLINE float GetCrowdAvoidanceRangeMultiplier() const { return AgentSettings.AvoidanceRangeMultiplier; }
	UObject* GetOuter() const { return &Owner; }

	FORCEINLINE void SetSimulationState(ERecallCrowdSimulationState State) { MutableData.SimulationState = State; }

	FORCEINLINE const FRecallCrowdAgentMutableData& GetMutableData() const { return MutableData; }
	FORCEINLINE void SetMutableData(const FRecallCrowdAgentMutableData& Data) { MutableData = Data; }

	FORCEINLINE void SetSuspendCrowdSimulation(bool bSuspend) { MutableData.bSuspendCrowdSimulation = bSuspend; }

	FORCEINLINE FRecallCrowdAgentBlackboard& GetMutableBlackboard() { return AgentBlackboard; }
	FORCEINLINE const FRecallCrowdAgentBlackboard& GetBlackboard() const { return AgentBlackboard; }

public:
	// ICrowdAgentInterface BEGIN
	virtual FVector GetCrowdAgentLocation() const override;
	virtual FVector GetCrowdAgentVelocity() const override;
	virtual void GetCrowdAgentCollisions(float& CylinderRadius, float& CylinderHalfHeight) const override;
	virtual float GetCrowdAgentMaxSpeed() const override;
	virtual int32 GetCrowdAgentAvoidanceGroup() const override;
	virtual int32 GetCrowdAgentGroupsToAvoid() const override;
	virtual int32 GetCrowdAgentGroupsToIgnore() const override;
	// ICrowdAgentInterface END
	
protected:
	UObject& Owner;
	const FRecallCrowdAgentSettings AgentSettings;
	
	FRecallCrowdAgentMutableData MutableData;
	FRecallCrowdAgentBlackboard AgentBlackboard;
};
