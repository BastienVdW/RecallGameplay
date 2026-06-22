// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "MassProcessor.h"
#include "MassObserverProcessor.h"
#include "RecallSignalProcessorBase.h"

#include "RecallVoxelProcessors.generated.h"

UCLASS()
class URecallVoxelConstructor : public UMassObserverProcessor
{
	GENERATED_BODY()

public:
	URecallVoxelConstructor();

	virtual void InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& InEntityManager) override final;

protected:
	virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override final;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override final;

private:
	FMassEntityQuery EntityQuery;
};

UCLASS()
class URecallVoxelDestructor : public UMassObserverProcessor
{
	GENERATED_BODY()

public:
	URecallVoxelDestructor();

	virtual void InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& InEntityManager) override final;

protected:
	virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override final;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override final;

private:
	FMassEntityQuery EntityQuery;
};

/*
UCLASS()
class RECALLGAMEPLAY_API URecallVoxelSignalProcessor : public URecallSignalProcessorBase
{
	GENERATED_BODY()

public:
	URecallVoxelSignalProcessor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& InEntityManager) override final;

protected:
	virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override final;
	void SignalEntities(FMassEntityManager& EntityManager, FMassExecutionContext& Context, FRecallSignalNameLookup& EntitySignals) override final;
};

UCLASS()
class URecallVoxelProcessor : public UMassProcessor
{
	GENERATED_BODY()

public:
	URecallVoxelProcessor();

	virtual void InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& InEntityManager) override final;
	bool ShouldAllowQueryBasedPruning(const bool bRuntimeMode = true) const override final;

protected:
	virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override final;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override final;

private:
	FMassEntityQuery EntityQuery;

};
*/
