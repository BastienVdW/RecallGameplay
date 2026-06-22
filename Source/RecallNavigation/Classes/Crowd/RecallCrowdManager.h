// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CrowdManagerBase.h"
#include "CoreMinimal.h"
#include "Stats/Stats.h"
#include "UObject/Object.h"
#include "AI/Navigation/NavigationTypes.h"
#include "Tickable.h"
#include "DrawDebugHelpers.h"
#include "NavFilters/NavigationQueryFilter.h"
#include "RecallCrowdManager.generated.h"

class ANavigationData;
class dtCrowd;
class dtObstacleAvoidanceDebugData;
class ICrowdAgentInterface;
class FRecallCrowdAgent;
class URecallCrowdManager;
struct dtCrowdAgent;
struct dtCrowdAgentDebugInfo;
struct dtCrowdAgentParams;
struct FNavMeshPath;

#if WITH_RECAST
struct dtQuerySpecialLinkFilter;
struct dtCrowdAgentParams;
class dtCrowd;
struct dtCrowdAgent;
struct dtCrowdAgentDebugInfo;
class dtObstacleAvoidanceDebugData;
#endif

struct FNavigationQueryFilter;
typedef TSharedPtr<const FNavigationQueryFilter, ESPMode::ThreadSafe> FSharedConstNavQueryFilter;

/**
 *  Crowd manager is responsible for handling crowds using Detour (Recast library)
 *
 *  Agents will respect navmesh for all steering and avoidance updates, 
 *  but it's slower than AvoidanceManager solution (RVO, cares only about agents)
 *
 *  All agents will operate on the same navmesh data, which will be picked from
 *  navigation system defaults (UNavigationSystemV1::SupportedAgents[0])
 *
 *  To use it, you have to add CrowdFollowingComponent to your agent
 *  (usually: replace class of PathFollowingComponent in AIController by adding 
 *   those lines in controller's constructor
 *
 *   ACrowdAIController::ACrowdAIController(const FObjectInitializer& ObjectInitializer)
 *       : Super(ObjectInitializer.SetDefaultSubobjectClass<FRecallCrowdAgent>(TEXT("PathFollowingComponent")))
 *
 *   or simply add both components and switch move requests between them)
 *
 *  Actors that should be avoided, but are not being simulated by crowd (like players)
 *  should implement CrowdAgentInterface AND register/unregister themselves with crowd manager:
 *  
 *   URecallCrowdManager* CrowdManager = URecallCrowdManager::GetCurrent(this);
 *   if (CrowdManager)
 *   {
 *      CrowdManager->RegisterAgent(this);
 *   }
 *
 *   Check flags in FCrowdDebug namespace (CrowdManager.cpp) for debugging options.
 */

struct FRecallCrowdAvoidanceConfig;
struct FRecallCrowdAvoidanceSamplingPattern;

struct FRecallCrowdAgentData
{
#if WITH_RECAST
	/** special filter for checking offmesh links */
	TSharedPtr<dtQuerySpecialLinkFilter> LinkFilter;
#endif

	/** poly ref that agent is standing on from previous update */
	NavNodeRef PrevPoly;

	/** index of agent in detour crowd */
	int32 AgentIndex;

	/** remaining time for next path optimization */
	float PathOptRemainingTime;

	/** is this agent fully simulated by crowd? */
	uint32 bIsSimulated : 1;

	/** if set, agent wants path optimizations */
	uint32 bWantsPathOptimization : 1;

	FRecallCrowdAgentData() :	PrevPoly(0), AgentIndex(-1), PathOptRemainingTime(0), bIsSimulated(false), bWantsPathOptimization(false) {}

	bool IsValid() const { return AgentIndex >= 0; }
	RECALLNAVIGATION_API void ClearFilter();
};

struct FRecallCrowdTickHelper : FTickableGameObject
{
	TWeakObjectPtr<URecallCrowdManager> Owner;

	FRecallCrowdTickHelper() : Owner(NULL) {}
	virtual void Tick(float DeltaTime);
	virtual bool IsTickable() const { return Owner.IsValid(); }
	virtual bool IsTickableInEditor() const { return true; }
	virtual bool IsTickableWhenPaused() const { return true; }
	virtual TStatId GetStatId() const;
};

DECLARE_LOG_CATEGORY_EXTERN(LogRecallCrowd, Log, All);

UCLASS(MinimalAPI)
class  URecallCrowdManager : public UCrowdManagerBase
{
	GENERATED_BODY()

public:
	RECALLNAVIGATION_API URecallCrowdManager(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	RECALLNAVIGATION_API virtual void Tick(float DeltaTime) override;
	RECALLNAVIGATION_API virtual void BeginDestroy() override;

	/** adds new agent to crowd */
	RECALLNAVIGATION_API void RegisterAgent(ICrowdAgentInterface* Agent);

	/** removes agent from crowd */
	RECALLNAVIGATION_API void UnregisterAgent(const ICrowdAgentInterface* Agent);

	/** updates agent data */
	RECALLNAVIGATION_API void UpdateAgentParams(const ICrowdAgentInterface* Agent) const;

	/** refresh agent state */
	RECALLNAVIGATION_API void UpdateAgentState(const ICrowdAgentInterface* Agent) const;

	/** update agent after using custom link */
	RECALLNAVIGATION_API void OnAgentFinishedCustomLink(const ICrowdAgentInterface* Agent) const;

	/** sets move target for crowd agent (only for fully simulated) */
	RECALLNAVIGATION_API bool SetAgentMoveTarget(const FRecallCrowdAgent* AgentComponent, const FVector& MoveTarget, FSharedConstNavQueryFilter Filter) const;

	/** sets move direction for crowd agent (only for fully simulated) */
	RECALLNAVIGATION_API bool SetAgentMoveDirection(const FRecallCrowdAgent* AgentComponent, const FVector& MoveDirection) const;

	/** sets move target using path (only for fully simulated) */
	RECALLNAVIGATION_API bool SetAgentMovePath(const FRecallCrowdAgent* AgentComponent, const FNavMeshPath* Path, int32 PathSectionStart, int32 PathSectionEnd, const FVector& PathSectionEndLocation) const;

	/** clears move target for crowd agent (only for fully simulated) */
	RECALLNAVIGATION_API void ClearAgentMoveTarget(const FRecallCrowdAgent* AgentComponent) const;

	/** switch agent to waiting state */
	RECALLNAVIGATION_API void PauseAgent(const FRecallCrowdAgent* AgentComponent) const;

	/** resumes agent movement */
	RECALLNAVIGATION_API void ResumeAgent(const FRecallCrowdAgent* AgentComponent, bool bForceReplanPath = true) const;

	/** check if object is a valid crowd agent */
	RECALLNAVIGATION_API bool IsAgentValid(const FRecallCrowdAgent* AgentComponent) const;
	RECALLNAVIGATION_API bool IsAgentValid(const ICrowdAgentInterface* Agent) const;

	/** returns number of nearby agents */
	RECALLNAVIGATION_API int32 GetNumNearbyAgents(const ICrowdAgentInterface* Agent) const;

	/** returns a list of locations of nearby agents */
	RECALLNAVIGATION_API int32 GetNearbyAgentLocations(const ICrowdAgentInterface* Agent, TArray<FVector>& OutLocations) const;

	/** reads existing avoidance config or returns false */
	RECALLNAVIGATION_API bool GetAvoidanceConfig(int32 Idx, FRecallCrowdAvoidanceConfig& Data) const;

	/** remove started offmesh connections from corridor */
	RECALLNAVIGATION_API void SetOffmeshConnectionPruning(bool bRemoveFromCorridor);

	/** block path visibility raycasts when crossing different nav areas */
	RECALLNAVIGATION_API void SetSingleAreaVisibilityOptimization(bool bEnable);

	/** adjust current position in path's corridor, starting test from PathStartIdx */
	RECALLNAVIGATION_API void AdjustAgentPathStart(const FRecallCrowdAgent* AgentComponent, const FNavMeshPath* Path, int32& PathStartIdx) const;

#if WITH_EDITOR
	RECALLNAVIGATION_API virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	RECALLNAVIGATION_API void DebugTick() const;
#endif

	/** notify called when detour navmesh is changed */
	RECALLNAVIGATION_API void OnNavMeshUpdate();

	/** Tests if NavData is a suitable nav data type to be used by this CrowdManager
	 *	instance. */
	RECALLNAVIGATION_API virtual bool IsSuitableNavData(const ANavigationData& NavData) const;

	/** Called by the nav system when a new navigation data instance is registered. 
	 *	If the CrowdManager instance had no nav data cached it will consider this
	 *	NavDataInstance and update if necesary. */
	RECALLNAVIGATION_API virtual void OnNavDataRegistered(ANavigationData& NavData) override;

	/** Called by the nav system when a navigation data instance is removed. The 
	 *	crowd manager will see if it's the nav data being used by it an if so try
	 *	to find another one. If there's none the crowd manager will stop working. */
	RECALLNAVIGATION_API virtual void OnNavDataUnregistered(ANavigationData& NavData) override;

	virtual void CleanUp(float DeltaTime) override {};

	const ANavigationData* GetNavData() const { return MyNavData; }

	RECALLNAVIGATION_API UWorld* GetWorld() const override;

	void SetAgentData(const ICrowdAgentInterface* Agent, const FRecallCrowdAgentData& Data) { ActiveAgents[Agent] = Data; }
	const FRecallCrowdAgentData& GetAgentDataChecked(const ICrowdAgentInterface* Agent) { return ActiveAgents.FindChecked(Agent); }

	RECALLNAVIGATION_API void CreateCrowdManager();
	RECALLNAVIGATION_API void DestroyCrowdManager();

#if WITH_RECAST
	dtCrowdAgent* GetMutableDetourCrowdAgent(const ICrowdAgentInterface* Agent);
	const dtCrowdAgent* GetDetourCrowdAgent(const ICrowdAgentInterface* Agent) const;
#endif // WITH_RECAST

protected:

	UPROPERTY(transient)
	TObjectPtr<ANavigationData> MyNavData;

	uint32 bPruneStartedOffmeshConnections : 1;
	uint32 bSingleAreaVisibilityOptimization : 1;
	uint32 bEarlyReachTestOptimization : 1;
	uint32 bAllowPathReplan : 1;
	
	/** agents registered in crowd manager */
	TMap<ICrowdAgentInterface*, FRecallCrowdAgentData> ActiveAgents;

	/** temporary flags for crowd agents */
	TArray<uint8> AgentFlags;

#if WITH_RECAST
	/** crowd manager */
	dtCrowd* DetourCrowd;

	/** debug data */
	dtCrowdAgentDebugInfo* DetourAgentDebug;
	dtObstacleAvoidanceDebugData* DetourAvoidanceDebug;
#endif

#if WITH_EDITOR
	FRecallCrowdTickHelper* TickHelper;
#endif // WITH_EDITOR

	/** try to initialize nav data from already existing ones */
	RECALLNAVIGATION_API virtual void UpdateNavData();

	/** setup params of crowd avoidance */
	RECALLNAVIGATION_API virtual void UpdateAvoidanceConfig();

	/** called from tick, just after updating agents proximity data */
	RECALLNAVIGATION_API virtual void PostProximityUpdate();

	/** called from tick, after move points were updated, before any steering/avoidance */
	RECALLNAVIGATION_API virtual void PostMovePointUpdate();

	/** Sets NavData as MyNavData. If Null and bFindNewNavDataIfNull is true then
	 *	the manager will search for a new NavData instance that meets the 
	 *	IsSuitableNavData() condition. */
	RECALLNAVIGATION_API void SetNavData(ANavigationData* NavData, const bool bFindNewNavDataIfNull = true);

#if WITH_RECAST
	RECALLNAVIGATION_API void AddAgent(const ICrowdAgentInterface* Agent, FRecallCrowdAgentData& AgentData) const;
	RECALLNAVIGATION_API void RemoveAgent(const ICrowdAgentInterface* Agent, FRecallCrowdAgentData* AgentData) const;
	RECALLNAVIGATION_API void GetAgentParams(const ICrowdAgentInterface* Agent, dtCrowdAgentParams& AgentParams) const;

	/** prepare agent for next step of simulation */
	RECALLNAVIGATION_API void PrepareAgentStep(const ICrowdAgentInterface* Agent, FRecallCrowdAgentData& AgentData, float DeltaTime) const;

	/** pass new velocity to movement components */
	RECALLNAVIGATION_API virtual void ApplyVelocity(FRecallCrowdAgent* AgentComponent, int32 AgentIndex) const;

	/** check changes in crowd simulation and adjust UE specific properties (smart links, poly updates) */
	RECALLNAVIGATION_API void UpdateAgentPaths();

	/** switch debugger to object selected in PIE */
	RECALLNAVIGATION_API void UpdateSelectedDebug(const ICrowdAgentInterface* Agent, int32 AgentIndex) const;

#if ENABLE_DRAW_DEBUG
	RECALLNAVIGATION_API UWorld* GetDebugDrawingWorld() const;
	RECALLNAVIGATION_API void DrawDebugCorners(const dtCrowdAgent* CrowdAgent) const;
	RECALLNAVIGATION_API void DrawDebugCollisionSegments(const dtCrowdAgent* CrowdAgent) const;
	RECALLNAVIGATION_API void DrawDebugPath(const dtCrowdAgent* CrowdAgent) const;
	RECALLNAVIGATION_API void DrawDebugVelocityObstacles(const dtCrowdAgent* CrowdAgent) const;
	RECALLNAVIGATION_API void DrawDebugPathOptimization(const dtCrowdAgent* CrowdAgent) const;
	RECALLNAVIGATION_API void DrawDebugNeighbors(const dtCrowdAgent* CrowdAgent) const;
	RECALLNAVIGATION_API void DrawDebugSharedBoundary() const;
#endif // ENABLE_DRAW_DEBUG

#endif
};
