// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "MassExtendedEntityTypes.h"

#include "RecallTrackTypes.generated.h"

USTRUCT()
struct RECALLTRACK_API FRecallTrackSegment
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	FVector Location = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere)
	FQuat Rotation = FQuat::Identity;
	
	UPROPERTY(VisibleAnywhere)
	TArray<FVector3f> Vertices;
	
	UPROPERTY(VisibleAnywhere)
	TArray<int32> Triangles;

	bool operator==(const FRecallTrackSegment& other) const
	{
		return Location == other.Location
			&& Rotation == other.Rotation
			&& Vertices == other.Vertices
			&& Triangles == other.Triangles;
	}

	bool operator!=(const FRecallTrackSegment& other) const
	{
		return !(*this == other);
	}
};
