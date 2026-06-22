// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "MassExtendedEntityTypes.h"
#include "MassExtendedEntityHandle.h"
#include "Simulation/Representation/RecallActorRepresentationFragments.h"
#include "Representation/Actor/RecallActorMeshRepresentationTypes.h"

#include "RecallGridCursorFragments.generated.h"

USTRUCT()
struct RECALLGRIDSELECTION_API FRecallGridSelectionFragment : public FMassExtendedFragment
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	int32 GridCellIndex = INDEX_NONE;
};

USTRUCT()
struct RECALLGRIDSELECTION_API FRecallGridCursorOwnerFragment : public FMassExtendedFragment
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	FMassExtendedEntityHandle GridSelectionEntity;

	UPROPERTY(VisibleAnywhere)
	FRecallActorHandle CursorActorHandle;

	UPROPERTY(VisibleAnywhere)
	bool bUseMousePosition = false;
	
	UPROPERTY(VisibleAnywhere)
	FVector2D CursorPosition = FVector2D::ZeroVector;
};

USTRUCT()
struct RECALLGRIDSELECTION_API FRecallGridCursorOwnerConstSharedFragment : public FMassExtendedConstSharedFragment
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, meta=(AllowedClasses="/Script/MassExtendedSpawner.MassExtendedEntityConfigAsset"))
	FSoftObjectPath GridSelectionEntityConfig;

	UPROPERTY(VisibleAnywhere)
	FActorRepresentationDesc CursorActorConfig;

	UPROPERTY(VisibleAnywhere)
	bool bAllowDeselectGridCell = false;
};
