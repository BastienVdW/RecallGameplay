// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "System/Representation/RecallActorInterpolationSubsystem.h"

#include "Kismet/KismetMathLibrary.h"
#include "Settings/RecallSimulationSettings.h"
#include "Utility/Simulation/RecallSimulationUtils.h"

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
static bool bDebugOnScreenActorMovementInterpolation = false;
static FAutoConsoleVariableRef CVarRecallDebugOnScreenActorMovementInterpolation(
	TEXT("recall.representation.OnScreenActorMovementInterpolation"),
	bDebugOnScreenActorMovementInterpolation,
	TEXT("Log On Screen Actor Movement Interpolation")
);
#endif // UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT

void URecallActorInterpolationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void URecallActorInterpolationSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void URecallActorInterpolationSubsystem::Reset()
{
	ReserInterpolationCache();
}

void URecallActorInterpolationSubsystem::Restore(const FRecallSnapshotContext& Context,
	const FInstancedStruct& InSnapshot)
{
	if (Context.IsSnapshot())
	{
		ReserInterpolationCache();	
	}
}

void URecallActorInterpolationSubsystem::ReserInterpolationCache()
{
	ActorInterpolationCache.Reset();
	LastDeltaFrame = 0.0f;
	AccumulatedDeltaFrame = 0.0f;
	SmoothedSpeedScale = 1.0f;
	SpeedScale = 1.0f;
	DeltaFrame = 0.0f;
}

void URecallActorInterpolationSubsystem::UpdateDeltaFrame(float DeltaTime)
{
	UpdateSmoothedSpeedScale(DeltaTime);

	LastDeltaFrame = Recall::Simulation::Utils::GetRepresentationDeltaFrame(GetWorld());
	AccumulatedDeltaFrame += LastDeltaFrame;

	SpeedScale = FMath::Min(SmoothedSpeedScale, AccumulatedDeltaFrame);
	DeltaFrame = AccumulatedDeltaFrame;

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	if (bDebugOnScreenActorMovementInterpolation)
	{
		LogOnScreenActorMovementInterpolation();
	}
#endif // UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT

	AccumulatedDeltaFrame = FMath::Max(0.0f, AccumulatedDeltaFrame - SpeedScale);
}

void URecallActorInterpolationSubsystem::ClearInterpolationCache(const FRecallActorHandle& ActorHandle)
{
	ActorInterpolationCache.Remove(ActorHandle);
}

void URecallActorInterpolationSubsystem::InterpolateVector(const FRecallActorHandle& ActorHandle,
                                                             const FName& Name, FVector& Value)
{
	FRecallActorInterpolationCache& Cache = ActorInterpolationCache.FindOrAdd(ActorHandle);

	if (!FMath::IsNearlyZero(DeltaFrame))
	{
		if (const FVector* PrevValue = Cache.VectorMap.Find(Name))
		{
			const FVector Movement = (Value - *PrevValue) / DeltaFrame * SpeedScale;

			Value = *PrevValue + Movement;
		}
	}
	
	Cache.VectorMap.Add(Name, Value);
}

void URecallActorInterpolationSubsystem::InterpolateQuat(const FRecallActorHandle& ActorHandle, const FName& Name,
	FQuat& Value)
{
	FRecallActorInterpolationCache& Cache = ActorInterpolationCache.FindOrAdd(ActorHandle);

	if (!FMath::IsNearlyZero(DeltaFrame))
	{
		if (const FQuat* PrevValue = Cache.QuatMap.Find(Name))
		{
			FQuat Rotate = (Value * PrevValue->Inverse()) / DeltaFrame * SpeedScale;
			Rotate.Normalize();

			Value = Rotate * *PrevValue;
		}
	}
	
	Cache.QuatMap.Add(Name, Value);
}

void URecallActorInterpolationSubsystem::UpdateSmoothedSpeedScale(float DeltaTime)
{
	const URecallSimulationSettings* SimulationSettings = GetDefault<URecallSimulationSettings>();
	
	const float SpeedScaleInterpAlpha = FMath::SmoothStep(
		static_cast<float>(SimulationSettings->MovementInterpolationFrameBuffer.Min), 
		static_cast<float>(SimulationSettings->MovementInterpolationFrameBuffer.Max), AccumulatedDeltaFrame);
	
	const float TargetSpeedScale = FMath::Lerp(1.0f, SimulationSettings->MovementInterpolationMaximumSpeed, SpeedScaleInterpAlpha);
	
	SmoothedSpeedScale = UKismetMathLibrary::FInterpTo(SmoothedSpeedScale, TargetSpeedScale,
		DeltaTime, SimulationSettings->MovementInterpolationSpeedScaleInterpSpeed);
}

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
void URecallActorInterpolationSubsystem::LogOnScreenActorMovementInterpolation() const
{
	if (GEngine)
	{
		if (FMath::IsNearlyZero(AccumulatedDeltaFrame))
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
				TEXT("AccumulatedDeltaFrame is null, could cause stutter"));
		}

		FString DebugString;
		DebugString += FString::Printf(TEXT("DeltaFrame: %f\n"), DeltaFrame);
		DebugString += FString::Printf(TEXT("SpeedScale: %f\n"), SpeedScale);
		DebugString += FString::Printf(TEXT("SmoothedSpeedScale: %f\n"), SmoothedSpeedScale);
		
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::White, DebugString);
	}
}
#endif // UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
