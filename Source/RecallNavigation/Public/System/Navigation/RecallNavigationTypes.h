// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "AI/Navigation/NavigationTypes.h"
#include "Helper/RecallGamethreadQueue.h"
#include "AI/Navigation/NavigationTypes.h"

#include "RecallNavigationTypes.generated.h"

RECALLNAVIGATION_API DECLARE_LOG_CATEGORY_EXTERN(LogRecallNavigation, Log, All);

USTRUCT()
struct RECALLNAVIGATION_API FRecallNavigationAgentSettings
{
	GENERATED_BODY()

	/** Whether to use async path requests (recommended for performance) */
	UPROPERTY(EditAnywhere)
	bool bPathAsync = true;
};

#define RECALL_NAVIGATION_INVALID_HANDLE_SERIAL_NUMBER 0

USTRUCT()
struct RECALLNAVIGATION_API FRecallNavigationHandle
{
	GENERATED_BODY()

	FRecallNavigationHandle() = default;
	FRecallNavigationHandle(uint32 InSerialNumber) : SerialNumber(InSerialNumber) {}

	UPROPERTY(VisibleAnywhere)
	uint32 SerialNumber = RECALL_NAVIGATION_INVALID_HANDLE_SERIAL_NUMBER;

	FORCEINLINE bool IsValid() const { return SerialNumber != RECALL_NAVIGATION_INVALID_HANDLE_SERIAL_NUMBER; }

	FORCEINLINE void Reset()
	{
		SerialNumber = RECALL_NAVIGATION_INVALID_HANDLE_SERIAL_NUMBER;
	}

	bool operator==(const FRecallNavigationHandle Other) const
	{
		return SerialNumber == Other.SerialNumber;
	}

	bool operator!=(const FRecallNavigationHandle Other) const
	{
		return !operator==(Other);
	}

	friend uint32 GetTypeHash(const FRecallNavigationHandle& Handle)
	{
		return Handle.SerialNumber;
	}
};

USTRUCT()
struct RECALLNAVIGATION_API FRecallNavigationPathPoint
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Position = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere)
	FNavLinkId CustomNavLinkId = FNavLinkId::Invalid;
	
	UPROPERTY(VisibleAnywhere)
	bool bIsNavLink = false;
};

USTRUCT()
struct RECALLNAVIGATION_API FRecallNavigationData : public FRecallGamethreadRunnerData
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere)
	FVector Start = FVector::ZeroVector;
	UPROPERTY(VisibleAnywhere)
	FVector End = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere)
	float Radius = -1.f;
	UPROPERTY(VisibleAnywhere)
	float Height = -1.f;

	bool operator==(const FRecallNavigationData& Other) const
	{
		return AsyncStartFrame == Other.AsyncStartFrame
			&& AsyncEndFrame == Other.AsyncEndFrame
			&& Start == Other.Start
			&& End == Other.End
			&& Radius == Other.Radius
			&& Height == Other.Height;
	}

	bool operator!=(const FRecallNavigationData& Other) const
	{
		return !operator==(Other);
	}
};

UCLASS(Within=RecallNavigationGamethreadQueue)
class URecallNavigationRunnerTask : public URecallGamethreadRunnerTask
{
	GENERATED_BODY()

public:
	static bool RunSync(UWorld* World, const FRecallNavigationData& Data, TArray<FRecallNavigationPathPoint>& OutPathPoints);

	const TArray<FRecallNavigationPathPoint>& GetPathPoints() const { return PathPoints; }

protected:
	virtual void OnRun() override final;
	virtual void OnForceStop() override final;
	virtual void OnForceFinish() override final;

	virtual bool IsIdenticalData(const TSharedPtr<FRecallGamethreadRunnerData>& OtherData) const override final
	{
		const TSharedPtr<FRecallNavigationData> OtherNavigationData = StaticCastSharedPtr<FRecallNavigationData>(OtherData);
		check(OtherNavigationData.IsValid());
		return GetRunnerDataRef<FRecallNavigationData>() == (*OtherNavigationData.Get());
	}
	
protected:
	UPROPERTY()
	uint32 RequestID = 0;
	UPROPERTY()
	TArray<FRecallNavigationPathPoint> PathPoints;

	void OnFinished(uint32 QueryID, ENavigationQueryResult::Type QueryResult, FNavPathSharedPtr Path);
};

/**
* Manager our navigation on the gamethread
*/
UCLASS(Within=RecallNavigationSubsystem)
class URecallNavigationGamethreadQueue : public URecallGamethreadQueue
{
	GENERATED_BODY()

public:
	URecallNavigationGamethreadQueue();

	void UpdateNavigationRunners(const TMap<FRecallNavigationHandle, FRecallNavigationData>& NavigationDataMap);
	TArray<FRecallNavigationPathPoint> GetPathPoints(const FRecallNavigationHandle& Handle) const;

private:
	TMap<uint32, TSharedPtr<FRecallGamethreadRunnerData>> CreateDataMap(const TMap<FRecallNavigationHandle, FRecallNavigationData>& NavigationDataMap) const;
};
