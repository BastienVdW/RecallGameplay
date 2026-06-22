// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "MassExtendedEntityTypes.h"
#include "Physics/RecallPhysicsTypes.h"
#include "Physics/Common/RecallPhysicsCommonShapeTypes.h"

#include "RecallTrackFragments.generated.h"

USTRUCT()
struct RECALLTRACK_API FRecallTrackFragment : public FMassExtendedFragment
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	FName TrackAssetName = NAME_None;
};

USTRUCT()
struct RECALLTRACK_API FRecallTrackConstSharedFragment : public FMassExtendedConstSharedFragment
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	FRecallPhysicsBodyParameters Params;
	
	UPROPERTY(VisibleAnywhere)
	FRecallPhysicsMeshShapeSettings MeshShapeSettings;
};
