// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "System/Interface/RecallSimulationReactSystemInterface.h"
#include "Mass/ExternalSubsystemTraits.h"
#include "System/Actor/RecallActorTypes.h"

#include "RecallActorInterpolationSubsystem.generated.h"

USTRUCT()
struct FRecallActorInterpolationCache
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	TMap<FName, FVector> VectorMap;
	
	UPROPERTY(Transient)
	TMap<FName, FQuat> QuatMap;
};

UCLASS(MinimalAPI)
class URecallActorInterpolationSubsystem : 
	public UWorldSubsystem,
	public IRecallSimulationReactSystemInterface
{
	GENERATED_BODY()

public:
	RECALLGAMEPLAY_API void InterpolateVector(const FRecallActorHandle& ActorHandle, const FName& Name, FVector& Value);
	RECALLGAMEPLAY_API void InterpolateQuat(const FRecallActorHandle& ActorHandle, const FName& Name, FQuat& Value);
	
	RECALLGAMEPLAY_API void ClearInterpolationCache(const FRecallActorHandle& ActorHandle);
	
	void UpdateDeltaFrame(float DeltaTime);
	
protected:
	// UWorldSubsystem implementation Begin
	virtual void Initialize(FSubsystemCollectionBase& Collection) override final;
	virtual void Deinitialize() override final;
	// UWorldSubsystem implementation End
	
	// IRecallSimulationReactSystemInterface implementation Begin
	virtual void Reset() override final;
	virtual void Restore(const FRecallSnapshotContext& Context, const FInstancedStruct& InSnapshot) override final;
	// IRecallSimulationReactSystemInterface implementation End

private:
	UPROPERTY(Transient)
	TMap<FRecallActorHandle, FRecallActorInterpolationCache> ActorInterpolationCache;
	UPROPERTY(Transient)
	float LastDeltaFrame = 0.0f;
	UPROPERTY(Transient)
	float AccumulatedDeltaFrame = 0.0f;
	UPROPERTY(Transient)
	float SmoothedSpeedScale = 1.0f;
	
	UPROPERTY(Transient)
	float SpeedScale = 1.0f;
	UPROPERTY(Transient)
	float DeltaFrame = 0.0f;
	
	void ReserInterpolationCache();
	
	void UpdateSmoothedSpeedScale(float DeltaTime);
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	void LogOnScreenActorMovementInterpolation() const;
#endif // UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
};

template<>
struct TMassExternalSubsystemTraits<URecallActorInterpolationSubsystem> final
{
	enum
	{
		GameThreadOnly = false
	};
};
