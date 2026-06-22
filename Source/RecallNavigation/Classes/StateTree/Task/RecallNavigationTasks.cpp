// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallNavigationTasks.h"

#include "MassExtendedEntityView.h"
#include "Desync/RecallDesyncLog.h"
#include "RecallSignalSubsystem.h"
#include "StateTreeLinker.h"
#include "Simulation/Controller/RecallControllerFragments.h"
#include "Simulation/Movement/RecallMovementFragments.h"
#include "Simulation/Navigation/RecallNavigationFragments.h"
#include "Simulation/Navigation/RecallPathFollowingFragments.h"
#include "Simulation/StateTree/RecallStateTreeSignalTypes.h"
#include "Simulation/Transform/RecallTransformFragments.h"
#include "StateTree/RecallStateTreeExecutionContext.h"
#include "System/EQS/RecallEnvQuerySubsystem.h"
#include "System/Obstacle/RecallNavigationObstacleSubsystem.h"
#include "Utility/Navigation/RecallNavigationObstacleUtils.h"

struct FRecallStateTreeExecutionContext;
//----------------------------------------------------------------------//
// FRecallMoveAtTask
//----------------------------------------------------------------------//
bool FRecallMoveAtTask::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(PathFollowerFragmentHandle);
	return true;
}

EStateTreeRunStatus FRecallMoveAtTask::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.RefreshPathTimer = -1.0f;

#if RECALL_DESYNC_LOG
	RECALL_DESYNC_LOG_VEC(Context.GetWorld(), MoveAtLocation, InstanceData.Location);
#endif // RECALL_DESYNC_LOG

	FRecallPathFollowerFragment& PathFollower = Context.GetExternalData(PathFollowerFragmentHandle);

	if (InstanceData.Location.IsZero())
	{
		UE_LOG(LogRecallNavigation, Warning, TEXT("%hs Initial MoveAt Location is zero"), __FUNCTION__);

		if (!bRefreshPath)
		{
			return EStateTreeRunStatus::Failed;
		}
	}
	else
	{
		// Request path through generic path follower fragment (allows any navigation system)
		PathFollower.RequestPath(InstanceData.Location);
	}

	if (bRefreshPath)
	{
		StartRefreshPathDelay(Context);
	}

	return Super::EnterState(Context, Transition);
}

void FRecallMoveAtTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	if (bStopOnExit)
	{
		FRecallPathFollowerFragment& PathFollower = Context.GetExternalData(PathFollowerFragmentHandle);
		PathFollower.Stop();
	}

	return Super::ExitState(Context, Transition);
}

EStateTreeRunStatus FRecallMoveAtTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	FRecallPathFollowerFragment& PathFollower = Context.GetExternalData(PathFollowerFragmentHandle);

	if (bExitOnReachDestination)
	{
		// Movement failed - navigation system clears path on failure
		if (!PathFollower.HasPath() && !PathFollower.IsPathRequested())
		{
			// Path generation was requested but failed (empty path indicates failure)
			return EStateTreeRunStatus::Failed;
		}
		// Movement ended
		if (!PathFollower.IsMoving())
		{
			return EStateTreeRunStatus::Succeeded;
		}
	}

	// Request path refresh if enabled
	if (bRefreshPath)
	{
		if (InstanceData.RefreshPathTimer > DeltaTime)
		{
			InstanceData.RefreshPathTimer -= DeltaTime;
		}
		else
		{
			PathFollower.RequestPath(InstanceData.Location);
			StartRefreshPathDelay(Context);
		}
	}

	return Super::Tick(Context, DeltaTime);
}

void FRecallMoveAtTask::StartRefreshPathDelay(FStateTreeExecutionContext& Context) const
{
	const FRecallStateTreeExecutionContext& MassContext = static_cast<const FRecallStateTreeExecutionContext&>(Context);
	const FRandomStream& RandomStream = MassContext.GetRandomStream();
	
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	
	InstanceData.RefreshPathTimer = RandomStream.FRandRange(
				FMath::Max(0.0f, RefreshPathDuration - RefreshPathRandomDeviation), (RefreshPathDuration + RefreshPathRandomDeviation));
	
	MassContext.GetSignalSystem().DelaySignalEntity(Recall::StateTree::Signals::TickRequired,
		MassContext.GetEntity(), InstanceData.RefreshPathTimer);
}

//----------------------------------------------------------------------//
// FRecallEnvQueryTask
//----------------------------------------------------------------------//
bool FRecallEnvQueryTask::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(TransformFragmentHandle);
	Linker.LinkExternalData(EnvQuerySystemHandle);
	Linker.LinkExternalData(NavigationObstacleSystemHandle);
	return true;
}

EStateTreeRunStatus FRecallEnvQueryTask::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	if (!ensureMsgf(EnvQuery, TEXT("EnvQuery should be set")))
	{
		return EStateTreeRunStatus::Failed;
	}
	
	const FRecallEnvQueryRequest EnvQueryRequest = GenerateEnvQueryRequest(Context);

#if RECALL_DESYNC_LOG
	RECALL_DESYNC_LOG_VEC(Context.GetWorld(), OwnerLocation, EnvQueryRequest.OwnerLocation);
	RECALL_DESYNC_LOG_VEC(Context.GetWorld(), TargetLocation, EnvQueryRequest.TargetLocation);
#endif // RECALL_DESYNC_LOG
	
	const FRecallStateTreeExecutionContext& MassContext = static_cast<FRecallStateTreeExecutionContext&>(Context);
	URecallEnvQuerySubsystem& EnvQuerySystem = Context.GetExternalData(EnvQuerySystemHandle);
	
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.EnvQueryHandle = EnvQuerySystem.RequestEnvQuery(MassContext.GetEntity(), EnvQueryRequest);
	if (!ensureAlwaysMsgf(InstanceData.EnvQueryHandle.IsValid(),
		TEXT("%hs EnvQuery request failed"), __FUNCTION__))
	{
		return EStateTreeRunStatus::Failed;
	}

	if (InstanceData.ShouldUseRelativeLocation())
	{
		InstanceData.RelativeTransform = FTransform(EnvQueryRequest.TargetRotation, EnvQueryRequest.TargetLocation);
	}
	
	return Super::EnterState(Context, Transition);
}

void FRecallEnvQueryTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	URecallEnvQuerySubsystem& EnvQuerySystem = Context.GetExternalData(EnvQuerySystemHandle);
	EnvQuerySystem.ReleaseEnvQuery(InstanceData.EnvQueryHandle);

	return Super::ExitState(Context, Transition);
}

EStateTreeRunStatus FRecallEnvQueryTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	URecallEnvQuerySubsystem& EnvQuerySystem = Context.GetExternalData(EnvQuerySystemHandle);

	if (!EnvQuerySystem.IsEnvQueryFinished(InstanceData.EnvQueryHandle))
	{
		return Super::Tick(Context, DeltaTime);
	}

	const FRecallStateTreeExecutionContext& MassContext = static_cast<FRecallStateTreeExecutionContext&>(Context);
	const FRandomStream& RandomStream = MassContext.GetRandomStream();

	FVector* QueryResult = InstanceData.QueryResult.GetMutablePtr(Context);
	if (!ensure(QueryResult != nullptr))
	{
		return EStateTreeRunStatus::Failed;
	}

	if (!EnvQuerySystem.GetEnvQueryBestResult(RandomStream, InstanceData.EnvQueryHandle, *QueryResult))
	{
		return EStateTreeRunStatus::Failed;
	}
	
	if (InstanceData.ShouldUseRelativeLocation())
	{
		(*QueryResult) = InstanceData.RelativeTransform.InverseTransformPosition(*QueryResult);
	}

#if RECALL_DESYNC_LOG
	RECALL_DESYNC_LOG_VEC(Context.GetWorld(), EnvQueryTargetLocation, InstanceData.TargetLocation);
	RECALL_DESYNC_LOG_VEC(Context.GetWorld(), EnvQueryResult, (*QueryResult));
#endif // RECALL_DESYNC_LOG

	return EStateTreeRunStatus::Succeeded;
}

FRecallEnvQueryRequest FRecallEnvQueryTask::GenerateEnvQueryRequest(
	FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	const FRecallStateTreeExecutionContext& MassContext = static_cast<FRecallStateTreeExecutionContext&>(Context);
	const FRecallTransformFragment& TransformFragment = Context.GetExternalData(TransformFragmentHandle);
	
	FRecallEnvQueryRequest EnvQueryRequest;
	EnvQueryRequest.EnvQuery = EnvQuery;
	EnvQueryRequest.OwnerLocation = TransformFragment.Position;
	EnvQueryRequest.OwnerRotation = TransformFragment.Rotation.Rotator();
	EnvQueryRequest.MinimumRelativeScore = MinimumScore;

	switch (InstanceData.Target)
	{
	case ERecallEnvQueryTarget::Location:
		{			
			EnvQueryRequest.TargetLocation = InstanceData.TargetLocation;
			EnvQueryRequest.TargetRotation = FRotator::ZeroRotator;
			EnvQueryRequest.ViewTargetLocation = TransformFragment.Position;
			EnvQueryRequest.ViewTargetRotation = TransformFragment.Rotation.Rotator();
		}
		break;

	case ERecallEnvQueryTarget::Entity:
		{
			const FMassExtendedEntityManager& EntityManager = MassContext.GetEntityManager();
			if (EntityManager.IsEntityValid(InstanceData.Entity))
			{
				const FMassExtendedEntityView TargetView(EntityManager, InstanceData.Entity);
				if (const auto* TargetTransformFragmentPtr = TargetView.GetFragmentDataPtr<FRecallTransformFragment>())
				{
					EnvQueryRequest.TargetLocation = TargetTransformFragmentPtr->Position;
					EnvQueryRequest.TargetRotation = TargetTransformFragmentPtr->Rotation.Rotator();
				}
				
				if (const auto* ControllerFragmentPtr = TargetView.GetFragmentDataPtr<FRecallControllerFragment>())
				{
					if (EntityManager.IsEntityValid(ControllerFragmentPtr->ViewTargetEntity))
					{
						const FMassExtendedEntityView ViewTargetView(EntityManager, ControllerFragmentPtr->ViewTargetEntity);
						if (const auto* ViewTargetTransformFragmentPtr = ViewTargetView.GetFragmentDataPtr<FRecallTransformFragment>())
						{
							EnvQueryRequest.ViewTargetLocation = ViewTargetTransformFragmentPtr->Position;
							EnvQueryRequest.ViewTargetRotation = ViewTargetTransformFragmentPtr->Rotation.Rotator();
						}
					}
				}
			}
		}
		break;

	default:
		unimplemented();
		break;
	}

	const URecallNavigationObstacleSubsystem& NavigationObstacleSystem = Context.GetExternalData(NavigationObstacleSystemHandle);
	const FRecallNavigationObstacleHashGrid2D& HashGrid = NavigationObstacleSystem.GetObstacleGrid();

	TArray<FRecallNavigationObstacleItem, TFixedAllocator<MaxObstacleResults>> CloseEntities;
	Recall::Navigation::Obstacle::Utils::FindCloseObstacles(InstanceData.TargetLocation,
		ObstacleSearchRadius, HashGrid, CloseEntities);

	EnvQueryRequest.CloseObstacles.Reserve(CloseEntities.Num());
	Algo::Transform(CloseEntities, EnvQueryRequest.CloseObstacles,
		[](const FRecallNavigationObstacleItem& CloseEntity)
	{
		return CloseEntity.Location;
	});
	
	return EnvQueryRequest;
}
