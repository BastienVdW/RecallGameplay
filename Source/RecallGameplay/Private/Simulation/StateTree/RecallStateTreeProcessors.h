// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "MassExtendedObserverProcessor.h"
#include "RecallSignalProcessorBase.h"

#include "RecallStateTreeProcessors.generated.h"

UCLASS()
class URecallStateTreeFragmentDestructor : public UMassExtendedObserverProcessor
{
	GENERATED_BODY()

	URecallStateTreeFragmentDestructor();

protected:
	virtual void InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager) override;
	virtual void ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager) override;
	virtual void Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context) override;

	FMassExtendedEntityQuery EntityQuery;
};

UCLASS()
class URecallStateTreeActivationProcessor : public UMassExtendedProcessor
{
	GENERATED_BODY()

	URecallStateTreeActivationProcessor();

protected:
	virtual void ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager) override;
	virtual void Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context) override;

private:
	FMassExtendedEntityQuery EntityQuery;
};

UCLASS()
class URecallStateTreeStartProcessor : public URecallSignalProcessorBase
{
	GENERATED_BODY()

public:
	URecallStateTreeStartProcessor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager) override final;

protected:
	virtual void ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager) override final;
	virtual void SignalEntities(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context, FRecallSignalNameLookup& EntitySignals) override final;

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
	virtual void InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager) override final;

protected:
	virtual void ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager) override final;
	void SignalEntities(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context, FRecallSignalNameLookup& EntitySignals) override final;

private:
	TSharedPtr<struct FRecallStateTreeUpdateCacheManager> CacheManager;
};

/**
* Processor to debug state tree on Render.
*/
UCLASS()
class URecallStateTreeDebugRepresentationProcessor : public UMassExtendedProcessor
{
	GENERATED_BODY()
		
	URecallStateTreeDebugRepresentationProcessor();

protected:
	virtual void ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager) override;
	virtual void Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context) override;

private:
	FMassExtendedEntityQuery EntityQuery;
};
