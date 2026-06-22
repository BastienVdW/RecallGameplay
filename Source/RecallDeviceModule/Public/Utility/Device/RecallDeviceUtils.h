// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"

struct FMassEntityHandle;
struct FMassEntityManager;
struct FMassExecutionContext;
struct FRecallPhysicsBodyFragment;
struct FRecallTransformFragment;
class URecallDeviceAsset;
class URecallEntitySubsystem;
class URecallPhysicsSubsystem;
class URecallRepresentationEventSubsystem;
class UMassEntityConfigAsset;

namespace Recall::Device::Utils
{

RECALLDEVICEMODULE_API FVector GetDevicePosition(const UMassEntityConfigAsset* EntityConfigAsset,
	const FRecallTransformFragment& TransformFragment, const FRecallPhysicsBodyFragment* BodyFragmentPtr = nullptr,
	bool bSnapToGrid = true, float GridSize = 100.0f);	
RECALLDEVICEMODULE_API extern bool CheckDevicePosition(const FMassEntityManager& EntityManager, const URecallPhysicsSubsystem& PhysicsSystem,
	const FMassEntityHandle& DeviceEntity, const FVector& Position);
	
RECALLDEVICEMODULE_API extern void RequestChangeDeviceColor(const FMassEntityManager& EntityManager,
	const FMassEntityHandle& DeviceEntity, const FName& ParameterName, const FColor& Color,
	URecallRepresentationEventSubsystem& RepresentationEventSystem);
RECALLDEVICEMODULE_API extern void SpawnDevicePlaceEntity(const FMassExecutionContext& Context, const FMassEntityHandle& OwnerEntity,
	const UMassEntityConfigAsset* DeviceEntityConfig, const FVector& DevicePosition, const FName& ColorParameterName, const FColor& Color,
	URecallEntitySubsystem& EntitySystem, URecallRepresentationEventSubsystem& RepresentationEventSystem);

RECALLDEVICEMODULE_API bool EvaluateDeviceCost(const UWorld* World, const FMassEntityHandle& OwnerEntity,
	const TObjectPtr<const URecallDeviceAsset>& DeviceAsset);
RECALLDEVICEMODULE_API void ConsumeDeviceCost(const UWorld* World, const FMassEntityHandle& OwnerEntity,
	const TObjectPtr<const URecallDeviceAsset>& DeviceAsset);
	
} // namespace Recall::Device::Utils
