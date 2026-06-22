// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "MassObserverProcessor.h"
#include "RecallSignalProcessorBase.h"

#include "RecallStateTreeProcessors.generated.h"

UCLASS()
class URecallStateTreeFragmentDestructor : public UMassObserverProcessor
{
	GENERATED_BODY()

	URecallStateTreeFragmentDestructor();

protected:
	virtual void InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& InEntityManager) override;
	virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

	FMassEntityQuery EntityQuery;
};

UCLASS()
class URecallStateTreeActivationProcessor : public UMassProcessor
{
	GENERATED_BODY()

	URecallStateTreeActivationProcessor();

protected:
	virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
	FMassEntityQuery EntityQuery;
};

UCLASS()
class URecallStateTreeStartProcessor : public URecallSignalProcessorBase
{
	GENERATED_BODY()

public:
	URecallStateTreeStartProcessor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& InEntityManager) override final;

protected:
	virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override final;
	virtual void SignalEntities(FMassEntityManager& EntityManager, FMassExecutionContext& Context, FRecallSignalNameLookup& EntitySignals) override final;

private:
	TSharedPtr<struct FRecallStateTreeStartCacheManager> CacheManager;
};

UCLASS()
class URecallStateTreeUpdateProcessor : public URecallSignalProcessorBase
{
	GENERATED_BODY()

public:
	URecallStateTreeUpdateProcessor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& InEntityManager) override final;

protected:
	virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override final;
	void SignalEntities(FMassEntityManager& EntityManager, FMassExecutionContext& Context, FRecallSignalNameLookup& EntitySignals) override final;

private:
	TSharedPtr<struct FRecallStateTreeUpdateCacheManager> CacheManager;
};

/**
* Processor to debug state tree on Render.
*/
UCLASS()
class URecallStateTreeDebugRepresentationProcessor : public UMassProcessor
{
	GENERATED_BODY()
		
	URecallStateTreeDebugRepresentationProcessor();

protected:
	virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
	FMassEntityQuery EntityQuery;
};
