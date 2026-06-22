// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Mass/EntityHandle.h"
#include "StateTreeExecutionContext.h"

class URecallSignalSubsystem;
struct FMassExecutionContext;
struct FMassEntityManager;

/**
 * Extends FStateTreeExecutionContext to provide additional data to Evaluators and Tasks related to RecallSimulation
 */
struct RECALLGAMEPLAY_API FRecallStateTreeExecutionContext : public FStateTreeExecutionContext
{
public:
	// @todo: refactor subsystems out of the exec context.
	FRecallStateTreeExecutionContext(UObject& InOwner, const UStateTree& InStateTree, FStateTreeInstanceData& InInstanceData, 
		FMassEntityManager& InEntityManager, URecallSignalSubsystem& InSignalSystem, FMassExecutionContext& InContext, const FMassEntityHandle& InEntity);

	FMassEntityManager& GetEntityManager() const { check(EntityManager); return *EntityManager; }
	FMassExecutionContext& GetMassExecutionContext() const { return *EntitySubsystemExecutionContext; }

	URecallSignalSubsystem& GetSignalSystem() const { check(SignalSubsystem); return *SignalSubsystem; }

	const FMassEntityHandle& GetEntity() const { return Entity; }

	const FRandomStream& GetRandomStream() const { return GetExecState().RandomStream; }

protected:

	/** Prefix that will be used by STATETREE_LOG and STATETREE_CLOG, using Entity description. */
	// virtual FString GetInstanceDescription() const override { return FString::Printf(TEXT("Entity [%s]: "), *Entity.DebugGetDescription()); }

	virtual void BeginDelayedTransition(const FStateTreeTransitionDelayedState& DelayedState) override;

	FMassEntityManager* EntityManager = nullptr;
	URecallSignalSubsystem* SignalSubsystem = nullptr;
	FMassExecutionContext* EntitySubsystemExecutionContext = nullptr;
	FMassEntityHandle Entity;
};
