// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"

#include "RecallNavLinkTraversal.generated.h"

struct FRecallNavLinkTraversalContext
{
	struct FMassExtendedExecutionContext& ExecutionContext;
	const struct FMassExtendedEntityHandle& Entity;
	const struct FRecallTransformFragment& TransformFragment;
	const struct FRecallNavLinkTraversalFragment& NavLinkTraversalFragment;
	const TWeakPtr<class FRecallPhysicsBody> PhysicsBody;

	UWorld* GetWorld() const;
};

USTRUCT()
struct RECALLNAVIGATION_API FRecallNavLinkTraversalBase
{
	GENERATED_BODY()
	
public:
	virtual ~FRecallNavLinkTraversalBase() = default;

	/**
	 * Callback when the traversal begins.
	 */
	virtual void OnEnter(const FRecallNavLinkTraversalContext& Context) const {}
	
	/**
	 * Callback when the traversal ticks.
	 * @return true to continue the traversal, false to abort it.
	 */
	virtual bool OnTick(const FRecallNavLinkTraversalContext& Context) const { return false; }
	
	/**
	 * Callback when the traversal ends.
	 */
	virtual void OnExit(const FRecallNavLinkTraversalContext& Context) const {}
};
