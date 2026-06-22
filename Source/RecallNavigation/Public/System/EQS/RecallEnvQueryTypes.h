// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "Mass/EntityHandle.h"
#include "Helper/RecallGamethreadQueue.h"

#include "RecallEnvQueryTypes.generated.h"

RECALLNAVIGATION_API DECLARE_LOG_CATEGORY_EXTERN(LogRecallEnvQuery, Log, All);

#define RECALL_ENV_QUERY_INVALID_HANDLE_SERIAL_NUMBER 0

#define RECALL_ENV_QUERY_OWNER_LOCATION_X 			TEXT("OwnerLocationX")
#define RECALL_ENV_QUERY_OWNER_LOCATION_Y 			TEXT("OwnerLocationY")
#define RECALL_ENV_QUERY_OWNER_LOCATION_Z 			TEXT("OwnerLocationZ")
														
#define RECALL_ENV_QUERY_OWNER_ROTATION_YAW			TEXT("OwnerRotationYaw")
#define RECALL_ENV_QUERY_OWNER_ROTATION_PITCH		TEXT("OwnerRotationPitch")
#define RECALL_ENV_QUERY_OWNER_ROTATION_ROLL			TEXT("OwnerRotationRoll")

#define RECALL_ENV_QUERY_VIEW_TARGET_LOCATION_X 		TEXT("ViewTargetLocationX")
#define RECALL_ENV_QUERY_VIEW_TARGET_LOCATION_Y 		TEXT("ViewTargetLocationY")
#define RECALL_ENV_QUERY_VIEW_TARGET_LOCATION_Z 		TEXT("ViewTargetLocationZ")

#define RECALL_ENV_QUERY_VIEW_TARGET_ROTATION_YAW	TEXT("ViewTargetRotationYaw")
#define RECALL_ENV_QUERY_VIEW_TARGET_ROTATION_PITCH	TEXT("ViewTargetRotationPitch")
#define RECALL_ENV_QUERY_VIEW_TARGET_ROTATION_ROLL	TEXT("ViewTargetRotationRoll")

#define RECALL_ENV_QUERY_TARGET_LOCATION_X 			TEXT("TargetLocationX")
#define RECALL_ENV_QUERY_TARGET_LOCATION_Y 			TEXT("TargetLocationY")
#define RECALL_ENV_QUERY_TARGET_LOCATION_Z 			TEXT("TargetLocationZ")
														
#define RECALL_ENV_QUERY_TARGET_ROTATION_YAW			TEXT("TargetRotationYaw")
#define RECALL_ENV_QUERY_TARGET_ROTATION_PITCH		TEXT("TargetRotationPitch")
#define RECALL_ENV_QUERY_TARGET_ROTATION_ROLL		TEXT("TargetRotationRoll")

USTRUCT()
struct RECALLNAVIGATION_API FRecallEnvQueryHandle
{
	GENERATED_BODY()

	FRecallEnvQueryHandle() = default;
	FRecallEnvQueryHandle(uint32 InSerialNumber) : SerialNumber(InSerialNumber) {}

	UPROPERTY(VisibleAnywhere)
	uint32 SerialNumber = RECALL_ENV_QUERY_INVALID_HANDLE_SERIAL_NUMBER;

	FORCEINLINE bool IsValid() const { return SerialNumber != RECALL_ENV_QUERY_INVALID_HANDLE_SERIAL_NUMBER; }

	FORCEINLINE void Reset()
	{
		SerialNumber = RECALL_ENV_QUERY_INVALID_HANDLE_SERIAL_NUMBER;
	}

	bool operator==(const FRecallEnvQueryHandle Other) const
	{
		return SerialNumber == Other.SerialNumber;
	}

	bool operator!=(const FRecallEnvQueryHandle Other) const
	{
		return !operator==(Other);
	}

	friend uint32 GetTypeHash(const FRecallEnvQueryHandle& Handle)
	{
		return Handle.SerialNumber;
	}
};

USTRUCT()
struct FRecallEnvQueryEntityInfo
{
	GENERATED_BODY()

	FRecallEnvQueryEntityInfo() = default;
	FRecallEnvQueryEntityInfo(const FTransform& Transform) : CachedTransform(Transform) {};

	/** Cached Transform from time of acquisition allows EnvQueryItemType_MassEntityHandle to implement UEnvQueryItemType_VectorBase */
	FTransform CachedTransform;
};

USTRUCT()
struct RECALLNAVIGATION_API FRecallEnvQueryRequest
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UEnvQuery> EnvQuery;

	UPROPERTY(VisibleAnywhere)
	FVector OwnerLocation = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere)
	FRotator OwnerRotation = FRotator::ZeroRotator;

	UPROPERTY(VisibleAnywhere)
	FVector ViewTargetLocation = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere)
	FRotator ViewTargetRotation = FRotator::ZeroRotator;
	
	UPROPERTY(VisibleAnywhere)
	FVector TargetLocation = FVector::ZeroVector;
	
	UPROPERTY(VisibleAnywhere)
	FRotator TargetRotation = FRotator::ZeroRotator;

	UPROPERTY(VisibleAnywhere)
	float MinimumRelativeScore = 0.95f;
	
	UPROPERTY(VisibleAnywhere)
	TArray<FVector> CloseObstacles;

	bool operator==(const FRecallEnvQueryRequest Other) const
	{
		return EnvQuery == Other.EnvQuery
			&& OwnerLocation == Other.OwnerLocation
			&& OwnerRotation == Other.OwnerRotation
			&& ViewTargetLocation == Other.ViewTargetLocation
			&& ViewTargetRotation == Other.ViewTargetRotation
			&& TargetLocation == Other.TargetLocation
			&& TargetRotation == Other.TargetRotation
			&& MinimumRelativeScore == Other.MinimumRelativeScore
			&& CloseObstacles == Other.CloseObstacles;
	}

	bool operator!=(const FRecallEnvQueryRequest Other) const
	{
		return !operator==(Other);
	}
};

USTRUCT()
struct RECALLNAVIGATION_API FRecallEnvQueryData : public FRecallGamethreadRunnerData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	FMassEntityHandle OwnerEntity;

	UPROPERTY(VisibleAnywhere)
	FRecallEnvQueryRequest Request;

	bool operator==(const FRecallEnvQueryData Other) const
	{
		return OwnerEntity == Other.OwnerEntity
			&& Request == Other.Request
			&& AsyncStartFrame == Other.AsyncStartFrame
			&& AsyncEndFrame == Other.AsyncEndFrame;
	}

	bool operator!=(const FRecallEnvQueryData Other) const
	{
		return !operator==(Other);
	}
};

USTRUCT()
struct RECALLNAVIGATION_API FRecallEnvQueryItem
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	float Score = 0.0f;

	UPROPERTY(Transient)
	FVector Location = FVector::ZeroVector;
};

USTRUCT()
struct RECALLNAVIGATION_API FRecallEnvQueryDebugCache
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	uint32 LastUpdateFrame = 0;
	
	UPROPERTY(Transient)
	FVector QueryResult = FVector::ZeroVector;
	
	UPROPERTY(Transient)
	TArray<FRecallEnvQueryItem> Items;
};

UCLASS(Within=RecallEnvQueryGamethreadQueue)
class URecallEnvQueryRunnerTask : public URecallGamethreadRunnerTask
{
	GENERATED_BODY()

public:
	const TArray<FRecallEnvQueryItem>& GetItems() const;
	TArray<FRecallEnvQueryItem> GetSelectedItems(float MinimumRelativeScore = 0.95f) const;

protected:
	virtual void OnRun() override final;
	virtual void OnForceStop() override final;
	virtual void OnForceFinish() override final;
	
	virtual bool IsIdenticalData(const TSharedPtr<FRecallGamethreadRunnerData>& OtherData) const override final
	{
		const TSharedPtr<FRecallEnvQueryData> OtherEnvQueryData = StaticCastSharedPtr<FRecallEnvQueryData>(OtherData);
		check(OtherEnvQueryData.IsValid());
		return GetRunnerDataRef<FRecallEnvQueryData>() == (*OtherEnvQueryData.Get());
	}
	
protected:
	UPROPERTY(Transient)
	int32 QueryID = INDEX_NONE;
	UPROPERTY(Transient)
	TArray<FRecallEnvQueryItem> Items;

	TSharedPtr<struct FEnvQueryInstance> QueryInstance;

	void QueryFinishedSignature(TSharedPtr<struct FEnvQueryResult> Result);
};

USTRUCT()
struct FRecallEnvQueryRunner
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	TObjectPtr<URecallEnvQueryRunnerTask> Task;
};

/**
* Manager our queries on the game-thread
*/
UCLASS(Within=RecallEnvQuerySubsystem)
class URecallEnvQueryGamethreadQueue : public URecallGamethreadQueue
{
	GENERATED_BODY()

public:
	URecallEnvQueryGamethreadQueue();
	
	TArray<FRecallEnvQueryItem> GetSelectedItems(const FRecallEnvQueryHandle& Handle, TArray<FRecallEnvQueryItem>* OutItems = nullptr) const;

	/* Update our env query runners on the game-thread to catch up with the new requests */
	void UpdateEnvQueryRunners(const TMap<FRecallEnvQueryHandle, FRecallEnvQueryData>& EnvQueryDataMap);
	
private:
	TMap<uint32, TSharedPtr<FRecallGamethreadRunnerData>> CreateDataMap(
	const TMap<FRecallEnvQueryHandle, FRecallEnvQueryData>& NavigationDataMap) const;
};
