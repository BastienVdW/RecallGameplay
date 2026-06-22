// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "System/Navigation/RecallNavigationTypes.h"

#include "RecallNavigationAgent.h"
#include "NavigationSystem.h"
#include "NavMesh/NavMeshPath.h"
#include "Utility/Simulation/RecallSimulationUtils.h"

DEFINE_LOG_CATEGORY(LogRecallNavigation);

//----------------------------------------------------------------------//
// URecallNavigationRunnerTask
//----------------------------------------------------------------------//
static bool ExtractPathPoints(FNavPathSharedPtr Path, TArray<FRecallNavigationPathPoint>& OutPathPoints)
{
	OutPathPoints.Reset();

	if (!Path.IsValid())
	{
		return false;
	}

	const int32 PathPointsCount = Path->GetPathPoints().Num();
	if (PathPointsCount > 1)
	{
		OutPathPoints.SetNum(PathPointsCount);

		for (int32 PathIndex = 0; PathIndex < PathPointsCount; ++PathIndex)
		{
			FRecallNavigationPathPoint& NewPathPoint = OutPathPoints[PathIndex];
			NewPathPoint.Position = Path->GetPathPoints()[PathIndex].Location;
			NewPathPoint.CustomNavLinkId = Path->GetPathPoints()[PathIndex].CustomNavLinkId;
			NewPathPoint.bIsNavLink = FNavMeshNodeFlags(Path->GetPathPoints()[PathIndex].Flags).IsNavLink();
		}

		return true;
	}

	return false;
}

bool URecallNavigationRunnerTask::RunSync(UWorld* World, const FRecallNavigationData& Data, TArray<FRecallNavigationPathPoint>& OutPathPoints)
{
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
	if (!ensure(NavSys))
	{
		return false;
	}

	const FNavAgentProperties AgentProperties(Data.Radius, Data.Height);
	const ANavigationData* NavData = NavSys->GetNavDataForProps(AgentProperties, Data.Start);
	const FRecallNavAgent Agent(Data.Start);
	const FPathFindingQuery Query(Agent, *NavData, Data.Start, Data.End);

	const FPathFindingResult Result = NavSys->FindPathSync(AgentProperties, Query, EPathFindingMode::Regular);
	return ExtractPathPoints(Result.Path, OutPathPoints);
}

void URecallNavigationRunnerTask::OnRun()
{
	check(IsInGameThread());

	if (RequestID != INVALID_NAVQUERYID)
	{
		return;
	}

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!ensure(NavSys))
	{
		return;
	}

	const FRecallNavigationData& NavigationData = GetRunnerDataRef<FRecallNavigationData>();
	const FNavAgentProperties AgentProperties(NavigationData.Radius, NavigationData.Height);
	const ANavigationData* NavData = NavSys->GetNavDataForProps(AgentProperties, NavigationData.Start);
	const FRecallNavAgent Agent(NavigationData.Start);
	const FPathFindingQuery Query(Agent, *NavData, NavigationData.Start, NavigationData.End);

	FNavPathQueryDelegate ResultDelegate;
	ResultDelegate.BindUObject(this, &ThisClass::OnFinished);

	RequestID = NavSys->FindPathAsync(AgentProperties, Query, ResultDelegate, EPathFindingMode::Regular);
}

void URecallNavigationRunnerTask::OnFinished(uint32 QueryID, ENavigationQueryResult::Type QueryResult, FNavPathSharedPtr Path)
{
	if (IsFinished())
	{
		return;
	}

	if (QueryResult == ENavigationQueryResult::Success)
	{
		ExtractPathPoints(Path, PathPoints);
	}
	else
	{
		UE_LOG(LogRecallNavigation, Warning, TEXT("FindPathAsync failed"));
	}

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	const uint32 Frame = Recall::Simulation::Utils::GetFrame(GetWorld());
	const FRecallNavigationData& NavigationData = GetRunnerDataRef<FRecallNavigationData>();
	UE_LOG(LogRecallNavigation, Verbose, TEXT("Finished FindPathAsync after %d frames"), Frame - NavigationData.AsyncStartFrame);
#endif // UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT

	RequestID = INVALID_NAVQUERYID;
	MarkAsFinished();
}

void URecallNavigationRunnerTask::OnForceStop()
{
	check(IsInGameThread());
	
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!IsValid(NavSys))
	{
		return;
	}

	if (RequestID != INVALID_NAVQUERYID)
	{
		NavSys->AbortAsyncFindPathRequest(RequestID);
	}
}

void URecallNavigationRunnerTask::OnForceFinish()
{
	check(IsInGameThread());

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!IsValid(NavSys))
	{
		return;
	}

	const FRecallNavigationData& NavigationData = GetRunnerDataRef<FRecallNavigationData>();
	const FNavAgentProperties AgentProperties(NavigationData.Radius, NavigationData.Height);	
	const ANavigationData* NavData = NavSys->GetNavDataForProps(AgentProperties, NavigationData.Start);
	const FRecallNavAgent Agent(NavigationData.Start);
	const FPathFindingQuery Query(Agent, *NavData, NavigationData.Start, NavigationData.End);
	const FPathFindingResult QueryResult = NavSys->FindPathSync(Query);

	if (QueryResult.IsSuccessful())
	{
		ExtractPathPoints(QueryResult.Path, PathPoints);
	}
	else
	{
		UE_LOG(LogRecallNavigation, Log,
			TEXT("%hs FindPathSync failed"), __FUNCTION__);
	}
}

//----------------------------------------------------------------------//
// URecallNavigationGamethreadQueue
//----------------------------------------------------------------------//
URecallNavigationGamethreadQueue::URecallNavigationGamethreadQueue()
	: Super()
{
	RunnerTaskClass = URecallNavigationRunnerTask::StaticClass();
}

void URecallNavigationGamethreadQueue::UpdateNavigationRunners(const TMap<FRecallNavigationHandle, FRecallNavigationData>& NavigationDataMap)
{
	return UpdateRunners(CreateDataMap(NavigationDataMap));
}

TMap<uint32, TSharedPtr<FRecallGamethreadRunnerData>> URecallNavigationGamethreadQueue::CreateDataMap(
	const TMap<FRecallNavigationHandle, FRecallNavigationData>& NavigationDataMap) const
{
	TMap<uint32, TSharedPtr<FRecallGamethreadRunnerData>> DataMap;
	DataMap.Reserve(NavigationDataMap.Num());

	for (const TPair<FRecallNavigationHandle, FRecallNavigationData>& NavigationData : NavigationDataMap)
	{
		DataMap.Add(NavigationData.Key.SerialNumber, MakeShared<FRecallNavigationData>(NavigationData.Value));
	}

	return DataMap;
}

TArray<FRecallNavigationPathPoint> URecallNavigationGamethreadQueue::GetPathPoints(const FRecallNavigationHandle& Handle) const
{
	const URecallNavigationRunnerTask* RunnerTask = GetRunnerTask<URecallNavigationRunnerTask>(Handle.SerialNumber);
	if (!ensureAlwaysMsgf(RunnerTask, TEXT("No task")))
	{
		return {};
	}

	if (!ensureAlwaysMsgf(RunnerTask->IsFinished(), TEXT("Failed to finish Navigation")))
	{
		return {};
	}

	return RunnerTask->GetPathPoints();
}
