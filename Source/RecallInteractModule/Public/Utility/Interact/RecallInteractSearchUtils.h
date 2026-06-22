// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"

struct FMassExtendedEntityManager;
struct FMassExtendedEntityView;
struct FMassExtendedEntityHandle;
struct FMassExtendedExecutionContext;
struct FRecallInteractorFragment;
struct FRecallInteractorSharedFragment;
struct FRecallTransformFragment;
struct FRecallPhysicsSensorFragment;

struct FRecallUpdateClosestInteractableEntityContext
{
	FMassExtendedExecutionContext& ExecutionContext;
	const FMassExtendedEntityHandle& InteractorEntity;
	const FRecallTransformFragment& TransformFragment;
	const FRecallPhysicsSensorFragment& SensorFragment;
	FRecallInteractorFragment& InteractorFragment;
	const FRecallInteractorSharedFragment& InteractorSharedFragment;
};

namespace Recall::Interact::Search::Utils
{

/// Updates the closest interactable entity for an interactor based on overlapping entities and interaction conditions.
/// @param Context Context containing all necessary data for updating closest interactable entity
RECALLINTERACTMODULE_API extern void UpdateClosestInteractableEntity(const FRecallUpdateClosestInteractableEntityContext& Context);

} // namespace Recall::Interact::Search::Utils
