// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "System/EQS/RecallEnvQueryTypes.h"

#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "Utility/Simulation/RecallSimulationUtils.h"

DEFINE_LOG_CATEGORY(LogRecallEnvQuery);

//----------------------------------------------------------------------//
// URecallEnvQueryRunnerTask
//----------------------------------------------------------------------//
static FEnvQueryRequest GenerateEnvQueryRequest(
	UWorld* World,
	const FRecallEnvQueryRequest& Request)
{
	FEnvQueryRequest QueryRequest = FEnvQueryRequest(Request.EnvQuery, World);

	QueryRequest.SetFloatParam(RECALL_ENV_QUERY_OWNER_LOCATION_X, 				Request.OwnerLocation.X);
	QueryRequest.SetFloatParam(RECALL_ENV_QUERY_OWNER_LOCATION_Y, 				Request.OwnerLocation.Y);
	QueryRequest.SetFloatParam(RECALL_ENV_QUERY_OWNER_LOCATION_Z, 				Request.OwnerLocation.Z);
																					
	QueryRequest.SetFloatParam(RECALL_ENV_QUERY_OWNER_ROTATION_YAW,				Request.OwnerRotation.Yaw);
	QueryRequest.SetFloatParam(RECALL_ENV_QUERY_OWNER_ROTATION_PITCH,			Request.OwnerRotation.Pitch);
	QueryRequest.SetFloatParam(RECALL_ENV_QUERY_OWNER_ROTATION_ROLL,				Request.OwnerRotation.Roll);
																					
	QueryRequest.SetFloatParam(RECALL_ENV_QUERY_VIEW_TARGET_LOCATION_X, 			Request.ViewTargetLocation.X);
	QueryRequest.SetFloatParam(RECALL_ENV_QUERY_VIEW_TARGET_LOCATION_Y, 			Request.ViewTargetLocation.Y);
	QueryRequest.SetFloatParam(RECALL_ENV_QUERY_VIEW_TARGET_LOCATION_Z, 			Request.ViewTargetLocation.Z);

	QueryRequest.SetFloatParam(RECALL_ENV_QUERY_VIEW_TARGET_ROTATION_YAW,		Request.ViewTargetRotation.Yaw);
	QueryRequest.SetFloatParam(RECALL_ENV_QUERY_VIEW_TARGET_ROTATION_PITCH,		Request.ViewTargetRotation.Pitch);
	QueryRequest.SetFloatParam(RECALL_ENV_QUERY_VIEW_TARGET_ROTATION_ROLL,		Request.ViewTargetRotation.Roll);
	
	QueryRequest.SetFloatParam(RECALL_ENV_QUERY_TARGET_LOCATION_X, 				Request.TargetLocation.X);
	QueryRequest.SetFloatParam(RECALL_ENV_QUERY_TARGET_LOCATION_Y, 				Request.TargetLocation.Y);
	QueryRequest.SetFloatParam(RECALL_ENV_QUERY_TARGET_LOCATION_Z, 				Request.TargetLocation.Z);
																					
	QueryRequest.SetFloatParam(RECALL_ENV_QUERY_TARGET_ROTATION_YAW,				Request.TargetRotation.Yaw);
	QueryRequest.SetFloatParam(RECALL_ENV_QUERY_TARGET_ROTATION_PITCH,			Request.TargetRotation.Pitch);
	QueryRequest.SetFloatParam(RECALL_ENV_QUERY_TARGET_ROTATION_ROLL,			Request.TargetRotation.Roll);

	if (Request.CloseObstacles.Num())
	{
		QueryRequest.SetIntParam(TEXT("ObstacleCount"), Request.CloseObstacles.Num());

		for (int32 ObstacleIndex = 0; ObstacleIndex < Request.CloseObstacles.Num(); ++ObstacleIndex)
		{
			const FVector& ObstacleLocation = Request.CloseObstacles[ObstacleIndex];
		
			QueryRequest.SetFloatParam(
				*FString::Printf(TEXT("ObstacleLocationX_%d"), ObstacleIndex), ObstacleLocation.X);
			QueryRequest.SetFloatParam(
				*FString::Printf(TEXT("ObstacleLocationY_%d"), ObstacleIndex), ObstacleLocation.Y);
			QueryRequest.SetFloatParam(
				*FString::Printf(TEXT("ObstacleLocationZ_%d"), ObstacleIndex), ObstacleLocation.Z);
		}
	}
	
	return QueryRequest;
}

static TSharedPtr<FEnvQueryResult> ExecuteInstantEnvQuery(
	UWorld* World,
	const FRecallEnvQueryRequest& Request)
{
	UEnvQueryManager* EnvQueryManager = UEnvQueryManager::GetCurrent(World);
	if (!ensureAlwaysMsgf(EnvQueryManager, TEXT("Invalid EnvQueryManager")))
	{
		return nullptr;
	}

	const FEnvQueryRequest QueryRequest = GenerateEnvQueryRequest(World, Request);
	return EnvQueryManager->RunInstantQuery(QueryRequest, EEnvQueryRunMode::AllMatching);
}

static int32 PrepareEnvQuery(UWorld* World,
	const FRecallEnvQueryRequest& Request, TSharedPtr<FEnvQueryInstance>& QueryInstance, FQueryFinishedSignature FinishDelegate)
{
	UEnvQueryManager* EnvQueryManager = UEnvQueryManager::GetCurrent(World);
	if (!ensureAlwaysMsgf(EnvQueryManager, TEXT("Invalid EnvQueryManager")))
	{
		return INDEX_NONE;
	}

	const FEnvQueryRequest QueryRequest = GenerateEnvQueryRequest(World, Request);
	QueryInstance = EnvQueryManager->PrepareQueryInstance(QueryRequest, EEnvQueryRunMode::AllMatching);
	return EnvQueryManager->RunQuery(QueryInstance, FinishDelegate);
}

void URecallEnvQueryRunnerTask::OnRun()
{
	check(IsInGameThread());

	if (QueryID != INDEX_NONE)
	{
		return;
	}

	const FRecallEnvQueryData& EnvQueryData = GetRunnerDataRef<FRecallEnvQueryData>();

	FQueryFinishedSignature FinishDelegate;
	FinishDelegate.BindUObject(this, &ThisClass::QueryFinishedSignature);
	
	QueryID = PrepareEnvQuery(GetWorld(), EnvQueryData.Request, QueryInstance, FinishDelegate);
}

void URecallEnvQueryRunnerTask::QueryFinishedSignature(TSharedPtr<FEnvQueryResult> Result)
{
	check(Result.IsValid());
	
	if (IsFinished() || Result->IsAborted())
	{
		return;
	}

	if (Result->IsSuccessful())
	{
		const UEnvQueryItemType_Point* DefTypeOb = Result->ItemType->GetDefaultObject<UEnvQueryItemType_Point>();
		check(DefTypeOb != nullptr);

		check(Items.IsEmpty());
		Items.Reserve(Result->Items.Num());

		for (const FEnvQueryItem& Item : Result->Items)
		{
			FRecallEnvQueryItem& NewItem = Items.AddDefaulted_GetRef();
			NewItem.Score = Item.Score;
			NewItem.Location = DefTypeOb->GetValue(Result->RawData.GetData() + Item.DataOffset);
		}

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		const uint32 Frame = Recall::Simulation::Utils::GetFrame(GetWorld());
		const FRecallEnvQueryData& EnvQueryData = GetRunnerDataRef<FRecallEnvQueryData>();
		UE_LOG(LogRecallEnvQuery, Verbose, TEXT("Finished RunQuery after %d frames"), Frame - EnvQueryData.AsyncStartFrame);
#endif // UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	}

	QueryID = INDEX_NONE;
	MarkAsFinished();
}

void URecallEnvQueryRunnerTask::OnForceStop()
{
	UEnvQueryManager* EnvQueryManager = UEnvQueryManager::GetCurrent(GetWorld());
	if (EnvQueryManager != nullptr)
	{
		if (QueryID != INDEX_NONE)
		{
			EnvQueryManager->AbortQuery(QueryID);
			QueryID = INDEX_NONE;
		}
	}
}

void URecallEnvQueryRunnerTask::OnForceFinish()
{
	const FRecallEnvQueryData& EnvQueryData = GetRunnerDataRef<FRecallEnvQueryData>();
	QueryFinishedSignature(ExecuteInstantEnvQuery(GetWorld(), EnvQueryData.Request));
}

const TArray<FRecallEnvQueryItem>& URecallEnvQueryRunnerTask::GetItems() const
{
	return Items;
}

TArray<FRecallEnvQueryItem> URecallEnvQueryRunnerTask::GetSelectedItems(float MinimumRelativeScore /*= 0.95f*/) const
{
	TArray<FRecallEnvQueryItem> SelectedItems;

	for (const FRecallEnvQueryItem& Item : GetItems())
	{
		if (Item.Score >= MinimumRelativeScore)
		{
			SelectedItems.Add(Item);
		}
		else
		{
			break;
		}
	}

	return SelectedItems;
}

//----------------------------------------------------------------------//
// URecallEnvQueryGamethreadQueue
//----------------------------------------------------------------------//
URecallEnvQueryGamethreadQueue::URecallEnvQueryGamethreadQueue()
	: Super()
{
	RunnerTaskClass =  URecallEnvQueryRunnerTask::StaticClass();
}

void URecallEnvQueryGamethreadQueue::UpdateEnvQueryRunners(const TMap<FRecallEnvQueryHandle, FRecallEnvQueryData>& EnvQueryDataMap)
{
	return UpdateRunners(CreateDataMap(EnvQueryDataMap));
}

TMap<uint32, TSharedPtr<FRecallGamethreadRunnerData>> URecallEnvQueryGamethreadQueue::CreateDataMap(
	const TMap<FRecallEnvQueryHandle, FRecallEnvQueryData>& EnvQueryDataMap) const
{
	TMap<uint32, TSharedPtr<FRecallGamethreadRunnerData>> DataMap;
	DataMap.Reserve(EnvQueryDataMap.Num());

	for (const TPair<FRecallEnvQueryHandle, FRecallEnvQueryData>& EnvQueryData : EnvQueryDataMap)
	{
		DataMap.Add(EnvQueryData.Key.SerialNumber, MakeShared<FRecallEnvQueryData>(EnvQueryData.Value));
	}

	return DataMap;
}

TArray<FRecallEnvQueryItem> URecallEnvQueryGamethreadQueue::GetSelectedItems(const FRecallEnvQueryHandle& Handle, TArray<FRecallEnvQueryItem>* OutItems /*= nullptr*/) const
{
	const URecallEnvQueryRunnerTask* RunnerTask = GetRunnerTask<URecallEnvQueryRunnerTask>(Handle.SerialNumber);
	if (!ensureAlwaysMsgf(RunnerTask, TEXT("No task")))
	{
		return {};
	}

	if (!ensureAlwaysMsgf(RunnerTask->IsFinished(), TEXT("Failed to finish EnvQuery")))
	{
		return {};
	}

	if (OutItems != nullptr)
	{
		(*OutItems) = RunnerTask->GetItems();
	}

	return RunnerTask->GetSelectedItems();
}
