// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallCrowdManager.h"

#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "NavigationSystem.h"
#include "NavMesh/RecastNavMesh.h"
#include "VisualLogger/VisualLogger.h"
#include "Engine/Engine.h"
#include "Math/UnrealMathUtility.h"

#if WITH_RECAST
#include "NavMesh/RecastHelpers.h"
#include "DetourCrowd/DetourObstacleAvoidance.h"
#include "DetourCrowd/DetourCrowd.h"
#include "Detour/DetourNavMesh.h"
#include "NavMesh/RecastQueryFilter.h"
#endif

#include "Crowd/RecallCrowdAgent.h"
#include "Settings/RecallGameplaySettings.h"
#include "System/Crowd/RecallCrowdSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RecallCrowdManager)

DECLARE_STATS_GROUP(TEXT("Crowd"), STATGROUP_AICrowd, STATCAT_Advanced);

DECLARE_CYCLE_STAT(TEXT("Nav Tick: crowd simulation"), STAT_Recall_Crowd_Tick, STATGROUP_AICrowd);
DECLARE_CYCLE_STAT(TEXT("Step: corridor update"), STAT_Recall_Crowd_StepCorridorTime, STATGROUP_AICrowd);
DECLARE_CYCLE_STAT(TEXT("Step: paths"), STAT_Recall_Crowd_StepPathsTime, STATGROUP_AICrowd);
DECLARE_CYCLE_STAT(TEXT("Step: proximity"), STAT_Recall_Crowd_StepProximityTime, STATGROUP_AICrowd);
DECLARE_CYCLE_STAT(TEXT("Step: next point"), STAT_Recall_Crowd_StepNextPointTime, STATGROUP_AICrowd);
DECLARE_CYCLE_STAT(TEXT("Step: steering"), STAT_Recall_Crowd_StepSteeringTime, STATGROUP_AICrowd);
DECLARE_CYCLE_STAT(TEXT("Step: avoidance"), STAT_Recall_Crowd_StepAvoidanceTime, STATGROUP_AICrowd);
DECLARE_CYCLE_STAT(TEXT("Step: collisions"), STAT_Recall_Crowd_StepCollisionsTime, STATGROUP_AICrowd);
DECLARE_CYCLE_STAT(TEXT("Step: components"), STAT_Recall_Crowd_StepComponentsTime, STATGROUP_AICrowd);
DECLARE_CYCLE_STAT(TEXT("Step: navlinks"), STAT_Recall_Crowd_StepNavLinkTime, STATGROUP_AICrowd);
DECLARE_CYCLE_STAT(TEXT("Step: movement"), STAT_Recall_Crowd_StepMovementTime, STATGROUP_AICrowd);
DECLARE_CYCLE_STAT(TEXT("Agent Update Time"), STAT_Recall_Crowd_AgentUpdateTime, STATGROUP_AICrowd);
DECLARE_DWORD_COUNTER_STAT(TEXT("Num Agents"), STAT_Recall_Crowd_NumAgents, STATGROUP_AICrowd);

DEFINE_LOG_CATEGORY(LogRecallCrowd);

namespace FRecallCrowdDebug
{
	/** if set, debug information will be displayed for agent selected in editor */
	int32 DebugSelectedActors = 0;
	FAutoConsoleVariableRef CVarRecallDebugSelectedActors(TEXT("recall.crowd.DebugSelectedActors"), DebugSelectedActors,
		TEXT("Enable debug drawing for selected crowd agent.\n0: Disable, 1: Enable"), ECVF_Default);

	/** if set, basic debug information will be recorded in VisLog for all agents */
	int32 DebugVisLog = 0;
	FAutoConsoleVariableRef CVarRecallDebugVisLog(TEXT("recall.crowd.DebugVisLog"), DebugVisLog,
		TEXT("Enable detailed vislog recording for all crowd agents.\n0: Disable, 1: Enable"), ECVF_Default);

	/** debug flags, works only for selected actor */
	int32 DrawDebugCorners = 1;
	FAutoConsoleVariableRef CVarRecallDrawDebugCorners(TEXT("recall.crowd.DrawDebugCorners"), DrawDebugCorners,
		TEXT("Draw path corners data, requires recall.crowd.DebugSelectedActors.\n0: Disable, 1: Enable"), ECVF_Default);
	
	int32 DrawDebugCollisionSegments = 1;
	FAutoConsoleVariableRef CVarRecallDrawDebugCollisionSegments(TEXT("recall.crowd.DrawDebugCollisionSegments"), DrawDebugCollisionSegments,
		TEXT("Draw colliding navmesh edges, requires recall.crowd.DebugSelectedActors.\n0: Disable, 1: Enable"), ECVF_Default);
	
	int32 DrawDebugPath = 1;
	FAutoConsoleVariableRef CVarRecallDrawDebugPath(TEXT("recall.crowd.DrawDebugPath"), DrawDebugPath,
		TEXT("Draw active paths, requires recall.crowd.DebugSelectedActors.\n0: Disable, 1: Enable"), ECVF_Default);
	
	int32 DrawDebugVelocityObstacles = 1;
	FAutoConsoleVariableRef CVarRecallDrawDebugVelocityObstacles(TEXT("recall.crowd.DrawDebugVelocityObstacles"), DrawDebugVelocityObstacles,
		TEXT("Draw velocity obstacle sampling, requires recall.crowd.DebugSelectedActors.\n0: Disable, 1: Enable"), ECVF_Default);
	
	int32 DrawDebugPathOptimization = 1;
	FAutoConsoleVariableRef CVarRecallDrawDebugPathOptimization(TEXT("recall.crowd.DrawDebugPathOptimization"), DrawDebugPathOptimization,
		TEXT("Draw path optimization data, requires recall.crowd.DebugSelectedActors.\n0: Disable, 1: Enable"), ECVF_Default);
	
	int32 DrawDebugNeighbors = 1;
	FAutoConsoleVariableRef CVarRecallDrawDebugNeighbors(TEXT("recall.crowd.DrawDebugNeighbors"), DrawDebugNeighbors,
		TEXT("Draw current neighbors data, requires recall.crowd.DebugSelectedActors.\n0: Disable, 1: Enable"), ECVF_Default);

	/** debug flags, don't depend on agent */
	int32 DrawDebugBoundaries = 0;
	FAutoConsoleVariableRef CVarRecallDrawDebugBoundaries(TEXT("recall.crowd.DrawDebugBoundaries"), DrawDebugBoundaries,
		TEXT("Draw shared navmesh boundaries used by crowd simulation.\n0: Disable, 1: Enable"), ECVF_Default);

	const FVector Offset(0, 0, 20);

	const FColor Corner(128, 0, 0);
	const FColor CornerLink(192, 0, 0);
	const FColor CornerFixed(192, 192, 0);
	const FColor CollisionRange(192, 0, 128);
	const FColor CollisionSeg0(192, 0, 128);
	const FColor CollisionSeg1(96, 0, 64);
	const FColor CollisionSegIgnored(128, 128, 128);
	const FColor Path(255, 255, 255);
	const FColor PathSpecial(255, 192, 203);
	const FColor PathOpt(0, 128, 0);
	const FColor AvoidanceRange(255, 255, 255);
	const FColor Neighbor(0, 192, 128);

	const float LineThickness = 3.f;
}

void FRecallCrowdTickHelper::Tick(float DeltaTime)
{
#if WITH_EDITOR
	if (Owner.IsValid())
	{
		Owner->DebugTick();
	}
#endif // WITH_EDITOR
}

TStatId FRecallCrowdTickHelper::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(FRecallCrowdTickHelper, STATGROUP_Tickables);
}

void FRecallCrowdAgentData::ClearFilter()
{
#if WITH_RECAST
	LinkFilter.Reset();
#endif
}

URecallCrowdManager::URecallCrowdManager(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MyNavData = NULL;
#if WITH_RECAST
	DetourCrowd = NULL;

	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		DetourAvoidanceDebug = dtAllocObstacleAvoidanceDebugData();
		DetourAvoidanceDebug->init(2048);

		DetourAgentDebug = new dtCrowdAgentDebugInfo();
		FMemory::Memzero(DetourAgentDebug, sizeof(dtCrowdAgentDebugInfo));
		DetourAgentDebug->idx = -1;
		DetourAgentDebug->vod = DetourAvoidanceDebug;
	}
	else
	{
		DetourAgentDebug = NULL;
		DetourAvoidanceDebug = NULL;
	}
#endif
#if WITH_EDITOR
	TickHelper = NULL;
	if (!HasAnyFlags(RF_ClassDefaultObject) && GIsEditor)
	{
		TickHelper = new FRecallCrowdTickHelper();
		TickHelper->Owner = this;
	}
#endif

	bSingleAreaVisibilityOptimization = true;
	bPruneStartedOffmeshConnections = false;
	bEarlyReachTestOptimization = false;
	bAllowPathReplan = true;
}

void URecallCrowdManager::BeginDestroy()
{
#if WITH_RECAST
	// cleanup allocated link filters
	ActiveAgents.Empty();

	dtFreeObstacleAvoidanceDebugData(DetourAvoidanceDebug);
	delete DetourAgentDebug;
#endif

#if WITH_EDITOR
	delete TickHelper;
#endif

#if WITH_RECAST
	DestroyCrowdManager();
#endif // WITH_RECAST
	Super::BeginDestroy();
}

void URecallCrowdManager::Tick(float DeltaTime)
{
	SCOPE_CYCLE_COUNTER(STAT_Recall_Crowd_Tick);
	INC_DWORD_STAT_BY(STAT_Recall_Crowd_NumAgents, ActiveAgents.Num());

#if WITH_RECAST
	if (DetourCrowd)
	{
		int32 NumActive = DetourCrowd->cacheActiveAgents();
		if (NumActive)
		{
			MyNavData->BeginBatchQuery();

			for (auto It = ActiveAgents.CreateIterator(); It; ++It)
			{
				// collect position and velocity
				FRecallCrowdAgentData& AgentData = It.Value();
				if (AgentData.IsValid())
				{
					PrepareAgentStep(It.Key(), AgentData, DeltaTime);
				}
			}

			// corridor update from previous step
			{
				SCOPE_CYCLE_COUNTER(STAT_Recall_Crowd_StepCorridorTime);
				DetourCrowd->updateStepCorridor(DeltaTime, DetourAgentDebug);
			}

			// regular steps
			if (bAllowPathReplan)
			{
				SCOPE_CYCLE_COUNTER(STAT_Recall_Crowd_StepPathsTime);
				DetourCrowd->updateStepPaths(DeltaTime, DetourAgentDebug);
			}
			{
				SCOPE_CYCLE_COUNTER(STAT_Recall_Crowd_StepProximityTime);
				DetourCrowd->updateStepProximityData(DeltaTime, DetourAgentDebug);
				PostProximityUpdate();
			}
			{
				SCOPE_CYCLE_COUNTER(STAT_Recall_Crowd_StepNextPointTime);
				DetourCrowd->updateStepNextMovePoint(DeltaTime, DetourAgentDebug);
				PostMovePointUpdate();
			}
			{
				SCOPE_CYCLE_COUNTER(STAT_Recall_Crowd_StepSteeringTime);
				DetourCrowd->updateStepSteering(DeltaTime, DetourAgentDebug);
			}
			{
				SCOPE_CYCLE_COUNTER(STAT_Recall_Crowd_StepAvoidanceTime);
				DetourCrowd->updateStepAvoidance(DeltaTime, DetourAgentDebug);
			}

			const URecallGameplaySettings* GameplaySettings = GetDefault<URecallGameplaySettings>();
			if (GameplaySettings->bResolveCollisions)
			{
				SCOPE_CYCLE_COUNTER(STAT_Recall_Crowd_StepCollisionsTime);
				DetourCrowd->updateStepMove(DeltaTime, DetourAgentDebug);
			}
			{
				SCOPE_CYCLE_COUNTER(STAT_Recall_Crowd_StepComponentsTime);
				UpdateAgentPaths();
			}
			{
				SCOPE_CYCLE_COUNTER(STAT_Recall_Crowd_StepNavLinkTime);
				DetourCrowd->updateStepOffMeshVelocity(DeltaTime, DetourAgentDebug);
			}

			// velocity updates
			{
				SCOPE_CYCLE_COUNTER(STAT_Recall_Crowd_StepMovementTime);
				for (auto It = ActiveAgents.CreateIterator(); It; ++It)
				{
					const FRecallCrowdAgentData& AgentData = It.Value();
					if (AgentData.bIsSimulated && AgentData.IsValid())
					{
						FRecallCrowdAgent* CrowdComponent = static_cast<FRecallCrowdAgent*>(It.Key());
						if (CrowdComponent && CrowdComponent->IsCrowdSimulationEnabled())
						{
							ApplyVelocity(CrowdComponent, AgentData.AgentIndex);
						}
					}
				}
			}

			MyNavData->FinishBatchQuery();

#if WITH_EDITOR
			// normalize samples only for debug drawing purposes
			DetourAvoidanceDebug->normalizeSamples();
#endif
		}
	}
#endif // WITH_RECAST
}

#if WITH_EDITOR
void URecallCrowdManager::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
#if WITH_RECAST
	// recreate crowd manger
	DestroyCrowdManager();
	CreateCrowdManager();
#endif // WITH_RECAST
}
#endif // WITH_EDITOR

void URecallCrowdManager::RegisterAgent(ICrowdAgentInterface* Agent)
{
	UpdateNavData();

	FRecallCrowdAgentData AgentData;

#if WITH_RECAST
	if (DetourCrowd)
	{
		AddAgent(Agent, AgentData);
	}
#endif

	ActiveAgents.Add(Agent, AgentData);
}

void URecallCrowdManager::UnregisterAgent(const ICrowdAgentInterface* Agent)
{
#if WITH_RECAST
	FRecallCrowdAgentData* AgentData = ActiveAgents.Find(Agent);
	if (DetourCrowd && AgentData)
	{
		RemoveAgent(Agent, AgentData);
	}
#endif

	ActiveAgents.Remove(Agent);
}

bool URecallCrowdManager::IsAgentValid(const FRecallCrowdAgent* AgentComponent) const
{
	const FRecallCrowdAgentData* AgentData = ActiveAgents.Find(AgentComponent);
	return AgentData && AgentData->IsValid();
}

bool URecallCrowdManager::IsAgentValid(const ICrowdAgentInterface* Agent) const
{
	const FRecallCrowdAgentData* AgentData = ActiveAgents.Find(Agent);
	return AgentData && AgentData->IsValid();
}

void URecallCrowdManager::UpdateAgentParams(const ICrowdAgentInterface* Agent) const
{
	SCOPE_CYCLE_COUNTER(STAT_Recall_Crowd_AgentUpdateTime);

#if WITH_RECAST
	const FRecallCrowdAgentData* AgentData = ActiveAgents.Find(Agent);
	if (DetourCrowd && AgentData && AgentData->IsValid())
	{
		dtCrowdAgentParams Params;
		GetAgentParams(Agent, Params);
		Params.linkFilter = AgentData->LinkFilter;

		// store for updating with constant intervals
		((FRecallCrowdAgentData*)AgentData)->bWantsPathOptimization = (Params.updateFlags & DT_CROWD_OPTIMIZE_VIS) != 0;

		DetourCrowd->updateAgentParameters(AgentData->AgentIndex, Params);
	}
#endif
}

void URecallCrowdManager::UpdateAgentState(const ICrowdAgentInterface* Agent) const
{
	SCOPE_CYCLE_COUNTER(STAT_Recall_Crowd_AgentUpdateTime);

#if WITH_RECAST
	const FRecallCrowdAgentData* AgentData = ActiveAgents.Find(Agent);
	if (DetourCrowd && AgentData && AgentData->IsValid())
	{
		DetourCrowd->updateAgentState(AgentData->AgentIndex, false);
	}
#endif
}

void URecallCrowdManager::OnAgentFinishedCustomLink(const ICrowdAgentInterface* Agent) const
{
#if WITH_RECAST
	const FRecallCrowdAgentData* AgentData = ActiveAgents.Find(Agent);
	if (DetourCrowd && AgentData && AgentData->IsValid())
	{
		DetourCrowd->setAgentBackOnLink(AgentData->AgentIndex);
	}
#endif
}

bool URecallCrowdManager::SetAgentMoveTarget(const FRecallCrowdAgent* AgentComponent, const FVector& MoveTarget, FSharedConstNavQueryFilter Filter) const
{
	SCOPE_CYCLE_COUNTER(STAT_Recall_Crowd_AgentUpdateTime);

	bool bSuccess = false;

#if WITH_RECAST
	const FRecallCrowdAgentData* AgentData = ActiveAgents.Find(AgentComponent);
	if (AgentData && AgentData->bIsSimulated && AgentData->IsValid() && DetourCrowd)
	{
		FNavLocation ProjectedLoc;
		MyNavData->ProjectPoint(MoveTarget, ProjectedLoc, MyNavData->GetDefaultQueryExtent(), Filter);

		const INavigationQueryFilterInterface* NavFilter = Filter.IsValid() ? Filter->GetImplementation() : MyNavData->GetDefaultQueryFilterImpl();
		const dtQueryFilter* DetourFilter = ((const FRecastQueryFilter*)NavFilter)->GetAsDetourQueryFilter();
		DetourCrowd->updateAgentFilter(AgentData->AgentIndex, DetourFilter);
		DetourCrowd->updateAgentState(AgentData->AgentIndex, false);

		const FVector RcTargetPos = Unreal2RecastPoint(MoveTarget);
		bSuccess = DetourCrowd->requestMoveTarget(AgentData->AgentIndex, ProjectedLoc.NodeRef, &RcTargetPos.X);
	}
#endif

	return bSuccess;
}

bool URecallCrowdManager::SetAgentMoveDirection(const FRecallCrowdAgent* AgentComponent, const FVector& MoveDirection) const
{
	SCOPE_CYCLE_COUNTER(STAT_Recall_Crowd_AgentUpdateTime);

	bool bSuccess = false;

#if WITH_RECAST
	const FRecallCrowdAgentData* AgentData = ActiveAgents.Find(AgentComponent);
	if (AgentData && AgentData->bIsSimulated && AgentData->IsValid() && DetourCrowd)
	{
		DetourCrowd->updateAgentState(AgentData->AgentIndex, false);

		const FVector RcTargetVelocity = Unreal2RecastPoint(MoveDirection * AgentComponent->GetCrowdAgentMaxSpeed());
		bSuccess = DetourCrowd->requestMoveVelocity(AgentData->AgentIndex, &RcTargetVelocity.X);
	}
#endif

	return bSuccess;
}

bool URecallCrowdManager::SetAgentMovePath(const FRecallCrowdAgent* AgentComponent, const FNavMeshPath* Path,
	int32 PathSectionStart, int32 PathSectionEnd, const FVector& PathSectionEndLocation) const
{
	SCOPE_CYCLE_COUNTER(STAT_Recall_Crowd_AgentUpdateTime);

	bool bSuccess = false;

#if WITH_RECAST
	const FRecallCrowdAgentData* AgentData = ActiveAgents.Find(AgentComponent);
	ARecastNavMesh* RecastNavData = Cast<ARecastNavMesh>(MyNavData);
	if (AgentData && AgentData->bIsSimulated && AgentData->IsValid() && 
		DetourCrowd && RecastNavData &&
		Path && (Path->GetPathPoints().Num() > 1) &&
		Path->PathCorridor.IsValidIndex(PathSectionStart) && Path->PathCorridor.IsValidIndex(PathSectionEnd))
	{
		FVector TargetPos = PathSectionEndLocation;
		if (PathSectionEnd < (Path->PathCorridor.Num() - 1))
		{
			RecastNavData->GetPolyCenter(Path->PathCorridor[PathSectionEnd], TargetPos);
		}

		TArray<dtPolyRef> PathRefs;
		for (int32 Idx = PathSectionStart; Idx <= PathSectionEnd; Idx++)
		{
			PathRefs.Add(Path->PathCorridor[Idx]);
		}

		const INavigationQueryFilterInterface* NavFilter = Path->GetFilter().IsValid() ? Path->GetFilter()->GetImplementation() : MyNavData->GetDefaultQueryFilterImpl();
		const dtQueryFilter* DetourFilter = ((const FRecastQueryFilter*)NavFilter)->GetAsDetourQueryFilter();
		DetourCrowd->updateAgentFilter(AgentData->AgentIndex, DetourFilter);
		DetourCrowd->updateAgentState(AgentData->AgentIndex, false);

		const FVector RcTargetPos = Unreal2RecastPoint(TargetPos);
		bSuccess = DetourCrowd->requestMoveTarget(AgentData->AgentIndex, PathRefs.Last(), &RcTargetPos.X);
		if (bSuccess)
		{
			bSuccess = DetourCrowd->setAgentCorridor(AgentData->AgentIndex, PathRefs.GetData(), PathRefs.Num());
		}
	}
#endif

	return bSuccess;
}

void URecallCrowdManager::ClearAgentMoveTarget(const FRecallCrowdAgent* AgentComponent) const
{
	SCOPE_CYCLE_COUNTER(STAT_Recall_Crowd_AgentUpdateTime);

#if WITH_RECAST
	const FRecallCrowdAgentData* AgentData = ActiveAgents.Find(AgentComponent);
	if (AgentData && AgentData->bIsSimulated && AgentData->IsValid() && DetourCrowd)
	{
		DetourCrowd->resetMoveTarget(AgentData->AgentIndex);
		DetourCrowd->resetAgentVelocity(AgentData->AgentIndex);
	}
#endif
}

void URecallCrowdManager::PauseAgent(const FRecallCrowdAgent* AgentComponent) const
{
	SCOPE_CYCLE_COUNTER(STAT_Recall_Crowd_AgentUpdateTime);

#if WITH_RECAST
	const FRecallCrowdAgentData* AgentData = ActiveAgents.Find(AgentComponent);
	if (AgentData && AgentData->bIsSimulated && AgentData->IsValid() && DetourCrowd)
	{
		DetourCrowd->setAgentWaiting(AgentData->AgentIndex);
		DetourCrowd->resetAgentVelocity(AgentData->AgentIndex);
	}
#endif

}

void URecallCrowdManager::ResumeAgent(const FRecallCrowdAgent* AgentComponent, bool bForceReplanPath) const
{
	SCOPE_CYCLE_COUNTER(STAT_Recall_Crowd_AgentUpdateTime);

#if WITH_RECAST
	const FRecallCrowdAgentData* AgentData = ActiveAgents.Find(AgentComponent);
	if (AgentData && AgentData->bIsSimulated && AgentData->IsValid() && DetourCrowd)
	{
		DetourCrowd->updateAgentState(AgentData->AgentIndex, bForceReplanPath);
	}
#endif
}

int32 URecallCrowdManager::GetNumNearbyAgents(const ICrowdAgentInterface* Agent) const
{
	int32 NumNearby = 0;

#if WITH_RECAST
	const FRecallCrowdAgentData* AgentData = ActiveAgents.Find(Agent);
	if (AgentData && AgentData->bIsSimulated && AgentData->IsValid() && DetourCrowd)
	{
		const dtCrowdAgent* ag = DetourCrowd->getAgent(AgentData->AgentIndex);
		NumNearby = ag ? ag->nneis : 0;
	}
#endif

	return NumNearby;
}

int32 URecallCrowdManager::GetNearbyAgentLocations(const ICrowdAgentInterface* Agent, TArray<FVector>& OutLocations) const
{
	const int32 InitialSize = OutLocations.Num();
#if WITH_RECAST
	const FRecallCrowdAgentData* AgentData = ActiveAgents.Find(Agent);

	if (AgentData && AgentData->bIsSimulated && AgentData->IsValid() && DetourCrowd)
	{
		const dtCrowdAgent* CrowdAgent = DetourCrowd->getAgent(AgentData->AgentIndex);

		if (CrowdAgent)
		{
			OutLocations.Reserve(InitialSize + CrowdAgent->nneis);

			for (int32 NeighbourIndex = 0; NeighbourIndex < CrowdAgent->nneis; NeighbourIndex++)
			{
				const dtCrowdAgent* NeighbourAgent = DetourCrowd->getAgent(CrowdAgent->neis[NeighbourIndex].idx);
				if (NeighbourAgent)
				{
					OutLocations.Add(Recast2UnrealPoint(NeighbourAgent->npos));
				}
			}
		}
	}
#endif

	return OutLocations.Num() - InitialSize;
}

bool URecallCrowdManager::GetAvoidanceConfig(int32 Idx, FRecallCrowdAvoidanceConfig& Data) const
{
	const URecallGameplaySettings* GameplaySettings = GetDefault<URecallGameplaySettings>();
	if (GameplaySettings->AvoidanceConfig.IsValidIndex(Idx))
	{
		Data = GameplaySettings->AvoidanceConfig[Idx];
		return true;
	}

	return false;
}

void URecallCrowdManager::AdjustAgentPathStart(const FRecallCrowdAgent* AgentComponent, const FNavMeshPath* Path, int32& PathStartIdx) const
{
#if WITH_RECAST
	const FRecallCrowdAgentData* AgentData = ActiveAgents.Find(AgentComponent);
	if (AgentData && AgentData->bIsSimulated && AgentData->IsValid() && 
		DetourCrowd && Path && Path->PathCorridor.Num() > PathStartIdx)
	{
		const dtCrowdAgent* ag = DetourCrowd->getAgent(AgentData->AgentIndex);
		const dtPolyRef* agPath = ag->corridor.getPath();

		for (int32 Idx = 0; Idx < ag->corridor.getPathCount(); Idx++)
		{
			const dtPolyRef TestRef = ag->corridor.getFirstPoly();

			for (int32 TestIdx = PathStartIdx; TestIdx < Path->PathCorridor.Num(); TestIdx++)
			{
				if (Path->PathCorridor[TestIdx] == TestRef)
				{
					PathStartIdx = TestIdx;
					return;
				}
			}
		}
	}
#endif
}

void URecallCrowdManager::SetOffmeshConnectionPruning(bool bRemoveFromCorridor)
{
	bPruneStartedOffmeshConnections = bRemoveFromCorridor;
#if WITH_RECAST
	if (DetourCrowd)
	{
		DetourCrowd->setPruneStartedOffmeshConnections(bRemoveFromCorridor);
	}
#endif
}

void URecallCrowdManager::SetSingleAreaVisibilityOptimization(bool bEnable)
{
	bSingleAreaVisibilityOptimization = bEnable;
#if WITH_RECAST
	if (DetourCrowd)
	{
		DetourCrowd->setSingleAreaVisibilityOptimization(bEnable);
	}
#endif
}

#if WITH_RECAST

void URecallCrowdManager::AddAgent(const ICrowdAgentInterface* Agent, FRecallCrowdAgentData& AgentData) const
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("URecallCrowdManager::AddAgent");
	SCOPE_CYCLE_COUNTER(STAT_Recall_Crowd_AgentUpdateTime);

	dtCrowdAgentParams Params;
	GetAgentParams(Agent, Params);

	// store for updating with constant intervals
	AgentData.bWantsPathOptimization = (Params.updateFlags & DT_CROWD_OPTIMIZE_VIS) != 0;

	// create link filter for fully simulated agents
	// (used to determine if agent can traverse smart links)
	TSharedPtr<dtQuerySpecialLinkFilter> MyLinkFilter;
	const FRecallCrowdAgent* CrowdComponent = static_cast<const FRecallCrowdAgent*>(Agent);
	if (CrowdComponent)
	{
		UNavigationSystemV1* NavSys = Cast<UNavigationSystemV1>(GetOuter());
		MyLinkFilter = MakeShareable(new FRecastSpeciaLinkFilter(NavSys, CrowdComponent->GetOuter()));
	}

	Params.linkFilter = MyLinkFilter;

	const FVector RcAgentPos = Unreal2RecastPoint(Agent->GetCrowdAgentLocation());
	const dtQueryFilter* DefaultFilter = ((const FRecastQueryFilter*)MyNavData->GetDefaultQueryFilterImpl())->GetAsDetourQueryFilter();

	AgentData.AgentIndex = DetourCrowd->addAgent(&RcAgentPos.X, Params, DefaultFilter);
	AgentData.bIsSimulated = (Params.collisionQueryRange > 0.0f) && (CrowdComponent == NULL || CrowdComponent->IsCrowdSimulationEnabled());
	AgentData.LinkFilter = MyLinkFilter;
}

void URecallCrowdManager::RemoveAgent(const ICrowdAgentInterface* Agent, FRecallCrowdAgentData* AgentData) const
{
	SCOPE_CYCLE_COUNTER(STAT_Recall_Crowd_AgentUpdateTime);

	DetourCrowd->removeAgent(AgentData->AgentIndex);
	AgentData->ClearFilter();
}

void URecallCrowdManager::GetAgentParams(const ICrowdAgentInterface* Agent, dtCrowdAgentParams& AgentParams) const
{
	float CylRadius = 0.0f, CylHalfHeight = 0.0f;
	Agent->GetCrowdAgentCollisions(CylRadius, CylHalfHeight);

	// first release the shared pointer
	AgentParams.linkFilter = nullptr;
	// this is actually a bit @hacky if we have non-POD types in dtCrowdAgentParams
	FMemory::Memzero(&AgentParams, sizeof(dtCrowdAgentParams));

	AgentParams.radius = CylRadius;
	AgentParams.height = CylHalfHeight * 2.0f;
	AgentParams.avoidanceQueryMultiplier = 1.0f;
	AgentParams.avoidanceGroup = Agent->GetCrowdAgentAvoidanceGroup();
	AgentParams.groupsToAvoid = Agent->GetCrowdAgentGroupsToAvoid();
	AgentParams.groupsToIgnore = Agent->GetCrowdAgentGroupsToIgnore();

	// skip maxSpeed, it will be constantly updated in every tick
	// skip maxAcceleration, we don't use Detour's movement code

	const FRecallCrowdAgent* CrowdComponent = static_cast<const FRecallCrowdAgent*>(Agent);
	if (CrowdComponent)
	{
		AgentParams.collisionQueryRange = CrowdComponent->GetCrowdCollisionQueryRange();
		AgentParams.pathOptimizationRange = CrowdComponent->GetCrowdPathOptimizationRange();
		AgentParams.separationWeight = CrowdComponent->GetCrowdSeparationWeight();
		AgentParams.obstacleAvoidanceType = IntCastChecked<unsigned char>((int32)CrowdComponent->GetCrowdAvoidanceQuality());
		AgentParams.avoidanceQueryMultiplier = CrowdComponent->GetCrowdAvoidanceRangeMultiplier();

		if (CrowdComponent->IsCrowdSimulationEnabled())
		{
			AgentParams.updateFlags =
				(CrowdComponent->IsCrowdAnticipateTurnsActive() ? DT_CROWD_ANTICIPATE_TURNS : 0) |
				(CrowdComponent->IsCrowdObstacleAvoidanceActive() ? DT_CROWD_OBSTACLE_AVOIDANCE : 0) |
				(CrowdComponent->IsCrowdSeparationActive() ? DT_CROWD_SEPARATION : 0) |
				(CrowdComponent->IsCrowdOptimizeVisibilityEnabled() ? (DT_CROWD_OPTIMIZE_VIS | DT_CROWD_OPTIMIZE_VIS_MULTI) : 0) |
				(CrowdComponent->IsCrowdOptimizeTopologyActive() ? DT_CROWD_OPTIMIZE_TOPO : 0) |
				(CrowdComponent->IsCrowdPathOffsetEnabled() ? DT_CROWD_OFFSET_PATH : 0) |
				(CrowdComponent->IsCrowdSlowdownAtGoalEnabled() ? DT_CROWD_SLOWDOWN_AT_GOAL : 0);
		}
	}
}

void URecallCrowdManager::PrepareAgentStep(const ICrowdAgentInterface* Agent, FRecallCrowdAgentData& AgentData, float DeltaTime) const
{
	const URecallGameplaySettings* GameplaySettings = GetDefault<URecallGameplaySettings>();
	
	dtCrowdAgent* ag = (dtCrowdAgent*)DetourCrowd->getAgent(AgentData.AgentIndex);
	ag->params.maxSpeed = Agent->GetCrowdAgentMaxSpeed();

	FVector RcLocation = Unreal2RecastPoint(Agent->GetCrowdAgentLocation());
	FVector RcVelocity = Unreal2RecastPoint(Agent->GetCrowdAgentVelocity());

	dtVcopy(ag->npos, &RcLocation.X);
	dtVcopy(ag->vel, &RcVelocity.X);

	if (AgentData.bWantsPathOptimization)
	{
		AgentData.PathOptRemainingTime -= DeltaTime;
		if (AgentData.PathOptRemainingTime > 0)
		{
			ag->params.updateFlags &= ~DT_CROWD_OPTIMIZE_VIS;
		}
		else
		{
			ag->params.updateFlags |= DT_CROWD_OPTIMIZE_VIS;
			AgentData.PathOptRemainingTime = GameplaySettings->PathOptimizationInterval;
		}
	}
}

void URecallCrowdManager::ApplyVelocity(FRecallCrowdAgent* AgentComponent, int32 AgentIndex) const
{
	const dtCrowdAgent* ag = DetourCrowd->getAgent(AgentIndex);
	const dtCrowdAgentAnimation* anims = DetourCrowd->getAgentAnims();

	const FVector NewVelocity = Recast2UnrealPoint(ag->nvel);
	const FVector::FReal* RcDestCorner = anims[AgentIndex].active ? anims[AgentIndex].endPos : 
		ag->ncorners ? &ag->cornerVerts[0] : &ag->npos[0];

	const bool bIsNearEndOfPath = (ag->ncorners == 1) && ((ag->cornerFlags[0] & DT_STRAIGHTPATH_OFFMESH_CONNECTION) == 0);

	const FVector DestPathCorner = Recast2UnrealPoint(RcDestCorner);
	AgentComponent->ApplyCrowdAgentVelocity(NewVelocity, DestPathCorner, anims[AgentIndex].active != 0, bIsNearEndOfPath);

	const URecallGameplaySettings* GameplaySettings = GetDefault<URecallGameplaySettings>();
	if (GameplaySettings->bResolveCollisions)
	{
		const FVector NewPosition = Recast2UnrealPoint(ag->npos);
		AgentComponent->ApplyCrowdAgentPosition(NewPosition);
	}
}

void URecallCrowdManager::UpdateAgentPaths()
{
	UNavigationSystemV1* NavSys = Cast<UNavigationSystemV1>(GetOuter());
	ARecastNavMesh* RecastNavData = Cast<ARecastNavMesh>(MyNavData);
	if (RecastNavData == NULL)
	{
		return;
	}

	const dtCrowdAgentAnimation* AgentAnims = DetourCrowd->getAgentAnims();
	for (auto It = ActiveAgents.CreateIterator(); It; ++It)
	{
		FRecallCrowdAgentData& AgentData = It.Value();
		if (AgentData.bIsSimulated && AgentData.IsValid())
		{
			FRecallCrowdAgent* CrowdComponent = nullptr;

			const dtCrowdAgent* Agent = DetourCrowd->getAgent(AgentData.AgentIndex);
			dtPolyRef AgentPolyRef = Agent->corridor.getFirstPoly();

			// look for newly triggered smart links
			const dtCrowdAgentAnimation& AnimInfo = AgentAnims[AgentData.AgentIndex];
			if (AnimInfo.active)
			{
				AgentPolyRef = AnimInfo.polyRef;

				if (AnimInfo.t == 0)
				{
					const FNavLinkId NavLinkId = RecastNavData->GetNavLinkUserId(AnimInfo.polyRef);
					INavLinkCustomInterface* CustomLink = NavSys->GetCustomLink(NavLinkId);

					if (CustomLink)
					{
						FVector EndPt = Recast2UnrealPoint(AnimInfo.endPos);

						// switch to waiting state
						DetourCrowd->setAgentWaiting(AgentData.AgentIndex);
						DetourCrowd->resetAgentVelocity(AgentData.AgentIndex);

						// start using smart link
						CrowdComponent = (CrowdComponent ? CrowdComponent : (FRecallCrowdAgent*)static_cast<const FRecallCrowdAgent*>(It.Key()));
						if (CrowdComponent)
						{
							CrowdComponent->StartUsingCustomLink(CustomLink, EndPt);
						}
					}
				}
			}

			// look for poly updates
			if (AgentPolyRef != AgentData.PrevPoly)
			{
				CrowdComponent = (CrowdComponent ? CrowdComponent : (FRecallCrowdAgent*)static_cast<const FRecallCrowdAgent*>(It.Key()));
				if (CrowdComponent)
				{
					CrowdComponent->OnNavNodeChanged(AgentPolyRef, AgentData.PrevPoly, Agent->corridor.getPathCount());
					AgentData.PrevPoly = AgentPolyRef;
				}
			}
		}
	}
}

void URecallCrowdManager::UpdateSelectedDebug(const ICrowdAgentInterface* Agent, int32 AgentIndex) const
{
#if WITH_EDITOR
	const UObject* Obj = Cast<const UObject>(Agent);
	if (GIsEditor && Obj)
	{
		const AController* TestController = Cast<const AController>(Obj->GetOuter());
		if (TestController && TestController->GetPawn() && TestController->GetPawn()->IsSelected())
		{
			DetourAgentDebug->idx = AgentIndex;
		}
	}
#endif
}

void URecallCrowdManager::CreateCrowdManager()
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("URecallCrowdManager::CreateCrowdManager");
	
	if (MyNavData == nullptr)
	{
		// run update and quit since UpdateNavData will call CreateCrowdManager
		// if navigation mesh is found
		UpdateNavData();

		UE_CLOG(MyNavData == nullptr, LogRecallCrowd, Warning, TEXT("Unable to find RecastNavMesh instance while trying to create URecallCrowdManager instance"));
		return;
	}

	ARecastNavMesh* RecastNavData = Cast<ARecastNavMesh>(MyNavData);
	dtNavMesh* NavMeshPtr = RecastNavData->GetRecastMesh();

	if (NavMeshPtr)
	{
		DetourCrowd = dtAllocCrowd();
	}
		
	if (DetourCrowd)
	{
		const URecallGameplaySettings* GameplaySettings = GetDefault<URecallGameplaySettings>();
		
		DetourCrowd->init(GameplaySettings->MaxAgents, GameplaySettings->MaxAgentRadius, NavMeshPtr);
		DetourCrowd->setAgentCheckInterval(GameplaySettings->NavmeshCheckInterval);
		DetourCrowd->setSeparationFilter(GameplaySettings->SeparationDirClamp);
		DetourCrowd->setSingleAreaVisibilityOptimization(bSingleAreaVisibilityOptimization);
		DetourCrowd->setPruneStartedOffmeshConnections(bPruneStartedOffmeshConnections);
		DetourCrowd->setEarlyReachTestOptimization(bEarlyReachTestOptimization);
		DetourCrowd->setPathOffsetRadiusMultiplier(GameplaySettings->PathOffsetRadiusMultiplier);

		DetourCrowd->initAvoidance(GameplaySettings->MaxAvoidedAgents, GameplaySettings->MaxAvoidedWalls, FMath::Max(GameplaySettings->SamplingPatterns.Num(), 1));

		for (int32 Idx = 0; Idx < GameplaySettings->SamplingPatterns.Num(); Idx++)
		{
			const FRecallCrowdAvoidanceSamplingPattern& Info = GameplaySettings->SamplingPatterns[Idx];
			if (Info.Angles.Num() > 0 && Info.Angles.Num() == Info.Radii.Num())
			{
				const TArray<FVector::FReal> Angles = UE::LWC::ConvertArrayType<FVector::FReal>(Info.Angles);
				const TArray<FVector::FReal> Radii = UE::LWC::ConvertArrayType<FVector::FReal>(Info.Radii);

				DetourCrowd->setObstacleAvoidancePattern(Idx, Angles.GetData(), Radii.GetData(), Angles.Num());
			}
		}

		UpdateAvoidanceConfig();

		AgentFlags.Reset();
		AgentFlags.AddZeroed(GameplaySettings->MaxAgents);

		for (auto It = ActiveAgents.CreateIterator(); It; ++It)
		{
			AddAgent(It.Key(), It.Value());
		}
	}
}

void URecallCrowdManager::DestroyCrowdManager()
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("URecallCrowdManager::DestroyCrowdManager");
	
	// freeing DetourCrowd with dtFreeCrowd 
	dtFreeCrowd(DetourCrowd);
	DetourCrowd = NULL;
}

dtCrowdAgent* URecallCrowdManager::GetMutableDetourCrowdAgent(const ICrowdAgentInterface* Agent)
{
	return const_cast<dtCrowdAgent*>(this->GetDetourCrowdAgent(Agent));
}

const dtCrowdAgent* URecallCrowdManager::GetDetourCrowdAgent(const ICrowdAgentInterface* Agent) const
{
	const FRecallCrowdAgentData& AgentData = ActiveAgents.FindChecked(Agent);
	return DetourCrowd->getAgent(AgentData.AgentIndex);
}

#if ENABLE_DRAW_DEBUG
UWorld* URecallCrowdManager::GetDebugDrawingWorld() const
{
	UWorld* DebugDrawingWorld = GetWorld();

#if WITH_EDITORONLY_DATA
	// note that being ENetMode::NM_DedicatedServer implies DebugDrawingWorld is a game world, which is exactly what we need
	if (DebugDrawingWorld != nullptr && DebugDrawingWorld->GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		// no point in trying to draw on dedicated server. Let's see if there's a client world we can use for drawing!
		const TIndirectArray<FWorldContext>& WorldContexts = GEngine->GetWorldContexts();
		for (const FWorldContext& Context : WorldContexts)
		{
			if (Context.World()->IsGameWorld() && Context.World()->GetNetMode() != ENetMode::NM_DedicatedServer)
			{
				DebugDrawingWorld = Context.World();
				break;
			}
		}
	}
#endif

	return DebugDrawingWorld;
}

void URecallCrowdManager::DrawDebugCorners(const dtCrowdAgent* CrowdAgent) const
{
	UWorld* DebugDrawingWorld = GetDebugDrawingWorld();

	{
		FVector P0 = Recast2UnrealPoint(CrowdAgent->npos);
		for (int32 Idx = 0; Idx < CrowdAgent->ncorners; Idx++)
		{
			FVector P1 = Recast2UnrealPoint(&CrowdAgent->cornerVerts[Idx * 3]);
			DrawDebugLine(DebugDrawingWorld, P0 + FRecallCrowdDebug::Offset, P1 + FRecallCrowdDebug::Offset, FRecallCrowdDebug::Corner, false, -1.0f, SDPG_World, 2.0f);
			P0 = P1;
		}
	}

	if (CrowdAgent->ncorners > 0 && (CrowdAgent->cornerFlags[CrowdAgent->ncorners - 1] & DT_STRAIGHTPATH_OFFMESH_CONNECTION))
	{
		FVector P0 = Recast2UnrealPoint(&CrowdAgent->cornerVerts[(CrowdAgent->ncorners - 1) * 3]);
		DrawDebugLine(DebugDrawingWorld, P0, P0 + FRecallCrowdDebug::Offset * 2.0f, FRecallCrowdDebug::CornerLink, false, -1.0f, SDPG_World, 2.0f);
	}
}

void URecallCrowdManager::DrawDebugCollisionSegments(const dtCrowdAgent* CrowdAgent) const
{
	UWorld* DebugDrawingWorld = GetDebugDrawingWorld();

	FVector Center = Recast2UnrealPoint(CrowdAgent->boundary.getCenter()) + FRecallCrowdDebug::Offset;
	DrawDebugCylinder(DebugDrawingWorld, Center - FRecallCrowdDebug::Offset, Center, UE_REAL_TO_FLOAT_CLAMPED_MAX(CrowdAgent->params.collisionQueryRange), 32, FRecallCrowdDebug::CollisionRange);

	for (int32 Idx = 0; Idx < CrowdAgent->boundary.getSegmentCount(); Idx++)
	{
		const FVector::FReal* s = CrowdAgent->boundary.getSegment(Idx);
		const int32 SegFlags = CrowdAgent->boundary.getSegmentFlags(Idx);
		const FColor Color = (SegFlags & DT_CROWD_BOUNDARY_IGNORE) ? FRecallCrowdDebug::CollisionSegIgnored :
			(dtTriArea2D(CrowdAgent->npos, s, s + 3) < 0.0f) ? FRecallCrowdDebug::CollisionSeg1 :
			FRecallCrowdDebug::CollisionSeg0;

		FVector Pt0 = Recast2UnrealPoint(s);
		FVector Pt1 = Recast2UnrealPoint(s + 3);

		DrawDebugLine(DebugDrawingWorld, Pt0 + FRecallCrowdDebug::Offset, Pt1 + FRecallCrowdDebug::Offset, Color, false, -1.0f, SDPG_World, 3.5f);
	}
}

void URecallCrowdManager::DrawDebugPath(const dtCrowdAgent* CrowdAgent) const
{
	UWorld* DebugDrawingWorld = GetDebugDrawingWorld();

	ARecastNavMesh* NavMesh = Cast<ARecastNavMesh>(MyNavData);
	if (NavMesh == NULL)
	{
		return;
	}

	NavMesh->BeginBatchQuery();
	
	const dtPolyRef* Path = CrowdAgent->corridor.getPath();
	TArray<FVector> Verts;

	for (int32 Idx = 0; Idx < CrowdAgent->corridor.getPathCount(); Idx++)
	{
		Verts.Reset();
		NavMesh->GetPolyVerts(Path[Idx], Verts);

		uint16 PolyFlags = 0;
		uint16 AreaFlags = 0;
		NavMesh->GetPolyFlags(Path[Idx], PolyFlags, AreaFlags);
		const FColor PolyColor = AreaFlags != 1 ? FRecallCrowdDebug::Path : FRecallCrowdDebug::PathSpecial;

		for (int32 VertIdx = 0; VertIdx < Verts.Num(); VertIdx++)
		{
			const FVector Pt0 = Verts[VertIdx];
			const FVector Pt1 = Verts[(VertIdx + 1) % Verts.Num()];

			DrawDebugLine(DebugDrawingWorld, Pt0 + FRecallCrowdDebug::Offset * 0.5f, Pt1 + FRecallCrowdDebug::Offset * 0.5f, PolyColor, false
				, /*LifeTime*/-1.f, /*DepthPriority*/0
				, /*Thickness*/FRecallCrowdDebug::LineThickness);
		}
	}

	NavMesh->FinishBatchQuery();
}

void URecallCrowdManager::DrawDebugVelocityObstacles(const dtCrowdAgent* CrowdAgent) const
{
	UWorld* DebugDrawingWorld = GetDebugDrawingWorld();

	FVector Center = Recast2UnrealPoint(CrowdAgent->npos) + FRecallCrowdDebug::Offset;
	DrawDebugCylinder(DebugDrawingWorld, Center - FRecallCrowdDebug::Offset, Center, UE_REAL_TO_FLOAT_CLAMPED_MAX(CrowdAgent->params.maxSpeed), 32, FRecallCrowdDebug::AvoidanceRange);

	const FVector::FReal InvQueryMultiplier = 1.0f / (float)CrowdAgent->params.avoidanceQueryMultiplier;
	float BestSampleScore = -1.0f;
	FVector BestSampleLocation = FVector::ZeroVector;

	for (int32 Idx = 0; Idx < DetourAvoidanceDebug->getSampleCount(); Idx++)
	{
		const FVector::FReal* p = DetourAvoidanceDebug->getSampleVelocity(Idx);
		const float sr = UE_REAL_TO_FLOAT_CLAMPED_MAX(DetourAvoidanceDebug->getSampleSize(Idx) * InvQueryMultiplier);
		const float pen = UE_REAL_TO_FLOAT(DetourAvoidanceDebug->getSamplePenalty(Idx));
		const float pen2 = UE_REAL_TO_FLOAT(DetourAvoidanceDebug->getSamplePreferredSidePenalty(Idx));

		FVector SamplePos = Center + Recast2UnrealPoint(p);

		if (BestSampleScore <= -1.0f || pen < BestSampleScore)
		{
			BestSampleScore = pen;
			BestSampleLocation = SamplePos;
		}

		float SamplePenalty = pen * 0.75f + pen2 * 0.25f;
		FColor SampleColor = FColor::MakeRedToGreenColorFromScalar(1.0f - SamplePenalty);
			
		FPlane Plane(0, 0, 1, SamplePos.Z);
		DrawDebugSolidPlane(DebugDrawingWorld, Plane, SamplePos, sr, SampleColor);
	}

	if (BestSampleScore >= 0.0f)
	{
		DrawDebugLine(DebugDrawingWorld, BestSampleLocation + FVector(0, 0, 100), BestSampleLocation + FVector(0, 0, -100), FColor::Green);
	}
}

void URecallCrowdManager::DrawDebugPathOptimization(const dtCrowdAgent* CrowdAgent) const
{
	UWorld* DebugDrawingWorld = GetDebugDrawingWorld();

	FVector Pt0 = Recast2UnrealPoint(DetourAgentDebug->optStart) + FRecallCrowdDebug::Offset * 1.25f;
	FVector Pt1 = Recast2UnrealPoint(DetourAgentDebug->optEnd) + FRecallCrowdDebug::Offset * 1.25f;

	DrawDebugLine(DebugDrawingWorld, Pt0, Pt1, FRecallCrowdDebug::PathOpt, false, -1.0f, SDPG_World, 2.5f);
}

void URecallCrowdManager::DrawDebugNeighbors(const dtCrowdAgent* CrowdAgent) const
{
	UWorld* DebugDrawingWorld = GetDebugDrawingWorld();

	FVector Center = Recast2UnrealPoint(CrowdAgent->npos) + FRecallCrowdDebug::Offset;
	DrawDebugCylinder(DebugDrawingWorld, Center - FRecallCrowdDebug::Offset, Center, UE_REAL_TO_FLOAT_CLAMPED_MAX(CrowdAgent->params.collisionQueryRange), 32, FRecallCrowdDebug::CollisionRange);

	for (int32 Idx = 0; Idx < CrowdAgent->nneis; Idx++)
	{
		const dtCrowdAgent* nei = DetourCrowd->getAgent(CrowdAgent->neis[Idx].idx);
		if (nei)
		{
			FVector Pt0 = Recast2UnrealPoint(nei->npos) + FRecallCrowdDebug::Offset;
			DrawDebugLine(DebugDrawingWorld, Center, Pt0, FRecallCrowdDebug::Neighbor);
		}
	}
}

void URecallCrowdManager::DrawDebugSharedBoundary() const
{
	UWorld* DebugDrawingWorld = GetDebugDrawingWorld();

	FColor Colors[] = { FColorList::Red, FColorList::Orange };

	const dtSharedBoundary* sharedBounds = DetourCrowd->getSharedBoundary();
	for (int32 Idx = 0; Idx < sharedBounds->Data.Num(); Idx++)
	{
		FColor Color = Colors[Idx % UE_ARRAY_COUNT(Colors)];
		const FVector Center = Recast2UnrealPoint(sharedBounds->Data[Idx].Center);
		DrawDebugCylinder(DebugDrawingWorld, Center - FRecallCrowdDebug::Offset, Center, UE_REAL_TO_FLOAT_CLAMPED_MAX(sharedBounds->Data[Idx].Radius), 32, Color);

		for (int32 WallIdx = 0; WallIdx < sharedBounds->Data[Idx].Edges.Num(); WallIdx++)
		{
			const FVector WallV0 = Recast2UnrealPoint(sharedBounds->Data[Idx].Edges[WallIdx].v0) + FRecallCrowdDebug::Offset;
			const FVector WallV1 = Recast2UnrealPoint(sharedBounds->Data[Idx].Edges[WallIdx].v1) + FRecallCrowdDebug::Offset;

			DrawDebugLine(DebugDrawingWorld, WallV0, WallV1, Color);
		}
	}
}
#endif // ENABLE_DRAW_DEBUG

#endif // WITH_RECAST

#if WITH_EDITOR

void URecallCrowdManager::DebugTick() const
{
#if WITH_RECAST
	if (DetourCrowd == NULL || DetourAgentDebug == NULL)
	{
		return;
	}

	for (auto It = ActiveAgents.CreateConstIterator(); It; ++It)
	{
		const FRecallCrowdAgentData& AgentData = It.Value();
		if (AgentData.IsValid())
		{
			UpdateSelectedDebug(It.Key(), AgentData.AgentIndex);
		}
	}

#if ENABLE_DRAW_DEBUG
	// on screen debugging
	const dtCrowdAgent* SelectedAgent = DetourAgentDebug->idx >= 0 ? DetourCrowd->getAgent(DetourAgentDebug->idx) : NULL;
	if (SelectedAgent && FRecallCrowdDebug::DebugSelectedActors)
	{
		if (FRecallCrowdDebug::DrawDebugCorners)
		{
			DrawDebugCorners(SelectedAgent);
		}

		if (FRecallCrowdDebug::DrawDebugCollisionSegments)
		{
			DrawDebugCollisionSegments(SelectedAgent);
		}

		if (FRecallCrowdDebug::DrawDebugPath)
		{
			DrawDebugPath(SelectedAgent);
		}

		if (FRecallCrowdDebug::DrawDebugVelocityObstacles)
		{
			DrawDebugVelocityObstacles(SelectedAgent);
		}

		if (FRecallCrowdDebug::DrawDebugPathOptimization)
		{
			DrawDebugPathOptimization(SelectedAgent);
		}

		if (FRecallCrowdDebug::DrawDebugNeighbors)
		{
			DrawDebugNeighbors(SelectedAgent);
		}
	}

	if (FRecallCrowdDebug::DrawDebugBoundaries)
	{
		DrawDebugSharedBoundary();
	}
#endif // ENABLE_DRAW_DEBUG

	// vislog debugging
	if (FRecallCrowdDebug::DebugVisLog)
	{
		for (auto It = ActiveAgents.CreateConstIterator(); It; ++It)
		{
			const ICrowdAgentInterface* IAgent = It.Key();
			const UObject* AgentOb = IAgent ?  Cast<const UObject>(IAgent) : NULL;
			const AActor* LogOwner = AgentOb ? Cast<const AActor>(AgentOb->GetOuter()) : NULL;

			const FRecallCrowdAgentData& AgentData = It.Value();
			const dtCrowdAgent* CrowdAgent = AgentData.IsValid() ? DetourCrowd->getAgent(AgentData.AgentIndex) : NULL;

			if (CrowdAgent && LogOwner)
			{
				FString LogData = DetourAgentDebug->agentLog.FindRef(AgentData.AgentIndex);
				if (LogData.Len() > 0)
				{
					UE_VLOG(LogOwner, LogRecallCrowd, Log, TEXT("%s"), *LogData);
				}

				{
					FVector P0 = Recast2UnrealPoint(CrowdAgent->npos);
					for (int32 Idx = 0; Idx < CrowdAgent->ncorners; Idx++)
					{
						FVector P1 = Recast2UnrealPoint(&CrowdAgent->cornerVerts[Idx * 3]);
						UE_VLOG_SEGMENT(LogOwner, LogRecallCrowd, Log, P0 + FRecallCrowdDebug::Offset, P1 + FRecallCrowdDebug::Offset, FRecallCrowdDebug::Corner, TEXT(""));
						UE_VLOG_BOX(LogOwner, LogRecallCrowd, Log, FBox::BuildAABB(P1 + FRecallCrowdDebug::Offset, FVector(2, 2, 2)), FRecallCrowdDebug::Corner, TEXT("%d"), CrowdAgent->cornerFlags[Idx]);
						P0 = P1;
					}
				}

				ARecastNavMesh* RecastNavData = Cast<ARecastNavMesh>(MyNavData);
				if (RecastNavData)
				{
					for (int32 Idx = 0; Idx < CrowdAgent->corridor.getPathCount(); Idx++)
					{
						dtPolyRef PolyRef = CrowdAgent->corridor.getPath()[Idx];
						TArray<FVector> PolyPoints;
						RecastNavData->GetPolyVerts(PolyRef, PolyPoints);

						UE_VLOG_CONVEXPOLY(LogOwner, LogRecallCrowd, Verbose, PolyPoints, FColor::Cyan, TEXT(""));
					}
				}

				if (CrowdAgent->ncorners && (CrowdAgent->cornerFlags[CrowdAgent->ncorners - 1] & DT_STRAIGHTPATH_OFFMESH_CONNECTION))
				{
					FVector P0 = Recast2UnrealPoint(&CrowdAgent->cornerVerts[(CrowdAgent->ncorners - 1) * 3]);
					UE_VLOG_SEGMENT(LogOwner, LogRecallCrowd, Log, P0, P0 + FRecallCrowdDebug::Offset * 2.0f, FRecallCrowdDebug::CornerLink, TEXT(""));
				}

				if (CrowdAgent->corridor.hasNextFixedCorner())
				{
					FVector P0 = Recast2UnrealPoint(CrowdAgent->corridor.getNextFixedCorner());
					UE_VLOG_BOX(LogOwner, LogRecallCrowd, Log, FBox::BuildAABB(P0 + FRecallCrowdDebug::Offset, FVector(10, 10, 10)), FRecallCrowdDebug::CornerFixed, TEXT(""));
				}

				if (CrowdAgent->corridor.hasNextFixedCorner2())
				{
					FVector P0 = Recast2UnrealPoint(CrowdAgent->corridor.getNextFixedCorner2());
					UE_VLOG_BOX(LogOwner, LogRecallCrowd, Log, FBox::BuildAABB(P0 + FRecallCrowdDebug::Offset, FVector(10, 10, 10)), FRecallCrowdDebug::CornerFixed, TEXT(""));
				}

				for (int32 Idx = 0; Idx < CrowdAgent->boundary.getSegmentCount(); Idx++)
				{
					const FVector::FReal* s = CrowdAgent->boundary.getSegment(Idx);
					const int32 SegFlags = CrowdAgent->boundary.getSegmentFlags(Idx);
					const FColor Color = (SegFlags & DT_CROWD_BOUNDARY_IGNORE) ? FRecallCrowdDebug::CollisionSegIgnored :
						(dtTriArea2D(CrowdAgent->npos, s, s + 3) < 0.0f) ? FRecallCrowdDebug::CollisionSeg1 :
						FRecallCrowdDebug::CollisionSeg0;

					FVector Pt0 = Recast2UnrealPoint(s);
					FVector Pt1 = Recast2UnrealPoint(s + 3);

					UE_VLOG_SEGMENT_THICK(LogOwner, LogRecallCrowd, Log, Pt0 + FRecallCrowdDebug::Offset, Pt1 + FRecallCrowdDebug::Offset, Color, 3, TEXT(""));
				}
			}
		}
	}

	DetourAgentDebug->agentLog.Reset();
#endif	// WITH_RECAST
}

#endif // WITH_EDITOR

void URecallCrowdManager::UpdateNavData()
{
	if (MyNavData == NULL)
	{
		UNavigationSystemV1* NavSys = Cast<UNavigationSystemV1>(GetOuter());
		if (NavSys)
		{
			for (ANavigationData* NavData : NavSys->NavDataSet)
			{
				if (NavData && IsSuitableNavData(*NavData))
				{
					SetNavData(NavData);
					break;
				}
			}
		}
	}
}

void URecallCrowdManager::SetNavData(ANavigationData* NavData, const bool bFindNewNavDataIfNull)
{
	if (NavData != MyNavData && MyNavData != nullptr)
	{
		// clean up
		ARecastNavMesh* AsRecastNavData = Cast<ARecastNavMesh>(MyNavData);
		if (AsRecastNavData)
		{
			AsRecastNavData->OnNavMeshUpdate.RemoveAll(this);
		}
	}

	if (NavData)
	{
		ARecastNavMesh* AsRecastNavData = Cast<ARecastNavMesh>(NavData);
		if (AsRecastNavData)
		{
			AsRecastNavData->OnNavMeshUpdate.AddUObject(this, &URecallCrowdManager::OnNavMeshUpdate);
		}
	}

	MyNavData = NavData;
	UE_VLOG(this, LogRecallCrowd, Log, TEXT("Setting cached nav data to %s")
		, MyNavData ? *MyNavData->GetFullName() : TEXT("NONE"));

	if (NavData || bFindNewNavDataIfNull)
	{
		OnNavMeshUpdate();
	}
}

void URecallCrowdManager::OnNavMeshUpdate()
{
#if WITH_RECAST
	DestroyCrowdManager();
	CreateCrowdManager();
#endif // WITH_RECAST
}

bool URecallCrowdManager::IsSuitableNavData(const ANavigationData& NavData) const
{
	return NavData.IsSupportingDefaultAgent() && Cast<ARecastNavMesh>(&NavData);
}

void URecallCrowdManager::OnNavDataRegistered(ANavigationData& NavData)
{
	if (MyNavData == nullptr && IsSuitableNavData(NavData))
	{
		SetNavData(&NavData);
	}
}

void URecallCrowdManager::OnNavDataUnregistered(ANavigationData& NavData)
{
	if (MyNavData == &NavData)
	{
		UE_VLOG(this, LogRecallCrowd, Warning, TEXT("%s is being unregistered. Will look for new nav data")
			, *NavData.GetName());
		
		// note that this will cause the manager to look for another nav data instance
		SetNavData(nullptr, /*bFindNewNavDataIfNull=*/true);

		if (MyNavData == nullptr)
		{
			UE_VLOG(this, LogRecallCrowd, Warning, TEXT("Failed to find suitable navigation data instance (no navmesh). Shutting down."));
		}
	}
}

void URecallCrowdManager::UpdateAvoidanceConfig()
{
#if WITH_RECAST
	if (DetourCrowd == NULL)
	{
		return;
	}

	const URecallGameplaySettings* GameplaySettings = GetDefault<URecallGameplaySettings>();
		
	for (int32 Idx = 0; Idx < GameplaySettings->AvoidanceConfig.Num(); Idx++)
	{
		const FRecallCrowdAvoidanceConfig& ConfigInfo = GameplaySettings->AvoidanceConfig[Idx];
		
		dtObstacleAvoidanceParams params;
		params.velBias = ConfigInfo.VelocityBias;
		params.weightDesVel = ConfigInfo.DesiredVelocityWeight;
		params.weightCurVel = ConfigInfo.CurrentVelocityWeight;
		params.weightSide = ConfigInfo.SideBiasWeight;
		params.weightToi = ConfigInfo.ImpactTimeWeight;
		params.horizTime = ConfigInfo.ImpactTimeRange;
		params.patternIdx = ConfigInfo.CustomPatternIdx;
		params.adaptiveDivs = ConfigInfo.AdaptiveDivisions;
		params.adaptiveRings = ConfigInfo.AdaptiveRings;
		params.adaptiveDepth = ConfigInfo.AdaptiveDepth;

		DetourCrowd->setObstacleAvoidanceParams(Idx, &params);
	}
#endif // WITH_RECAST
}

void URecallCrowdManager::PostProximityUpdate()
{
	// empty in base class
}

void URecallCrowdManager::PostMovePointUpdate()
{
#if WITH_RECAST
	const uint8 UpdateDestinationFlag = 1;

	// special case when following last segment of full path to actor: replace end point with actor's location
	for (auto& AgentTuple : ActiveAgents)
	{
		FRecallCrowdAgent* PathComp = static_cast<FRecallCrowdAgent*>(AgentTuple.Key);
		const FRecallCrowdAgentData& AgentData = AgentTuple.Value;
		FVector NewGoalPosition;

		const bool bUpdateTargetPos = PathComp ? PathComp->ShouldTrackMovingGoal(NewGoalPosition) : false;
		if (bUpdateTargetPos && AgentFlags.IsValidIndex(AgentData.AgentIndex))
		{
			PathComp->UpdateDestinationForMovingGoal(NewGoalPosition);

			const dtCrowdAgent* Agent = DetourCrowd->getAgent(AgentData.AgentIndex);
			dtCrowdAgent* MutableAgent = (dtCrowdAgent*)Agent;
			const FVector RcTargetPos = Unreal2RecastPoint(NewGoalPosition);
		
			dtVcopy(MutableAgent->targetPos, &RcTargetPos.X);
			AgentFlags[AgentData.AgentIndex] |= UpdateDestinationFlag;
		}
	}

	dtCrowdAgent** ActiveDetourAgents = DetourCrowd->getActiveAgents();
	for (int32 Idx = 0; Idx < DetourCrowd->getNumActiveAgents(); Idx++)
	{
		dtCrowdAgent* Agent = ActiveDetourAgents[Idx];
		if (Agent->state == DT_CROWDAGENT_STATE_WALKING &&
			Agent->ncorners == 1 && Agent->corridor.getPathCount() < 5 &&
			(Agent->cornerFlags[0] & DT_STRAIGHTPATH_OFFMESH_CONNECTION) == 0)
		{
			const int32 AgentIndex = DetourCrowd->getAgentIndex(Agent);

			if (AgentFlags.IsValidIndex(AgentIndex) && (AgentFlags[AgentIndex] & UpdateDestinationFlag) != 0)
			{
				dtVcopy(Agent->cornerVerts, Agent->targetPos);
			}
		}
	}

	for (auto& AgentTuple : ActiveAgents)
	{
		const FRecallCrowdAgentData& AgentData = AgentTuple.Value;
		if (AgentFlags.IsValidIndex(AgentData.AgentIndex))
		{
			AgentFlags[AgentData.AgentIndex] &= ~UpdateDestinationFlag;
		}
	}
#endif
}

UWorld* URecallCrowdManager::GetWorld() const
{
	URecallCrowdSubsystem* NavSys = Cast<URecallCrowdSubsystem>(GetOuter());
	return NavSys ? NavSys->GetWorld() : NULL;
}
