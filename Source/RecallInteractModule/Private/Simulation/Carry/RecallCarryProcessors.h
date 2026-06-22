// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "MassExtendedProcessor.h"
#include "MassExtendedObserverProcessor.h"
#include "RecallSignalProcessorBase.h"

#include "RecallCarryProcessors.generated.h"

UCLASS()
class URecallCarryableDestructor : public UMassExtendedObserverProcessor
{
	GENERATED_BODY()

public:
	URecallCarryableDestructor();

	void InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager) override final;

protected:
	virtual void ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager) override final;
	virtual void Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context) override final;

private:
	FMassExtendedEntityQuery EntityQuery;
};

UCLASS()
class URecallCarryableSignalProcessor : public URecallSignalProcessorBase
{
	GENERATED_BODY()

public:
	URecallCarryableSignalProcessor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager) override final;

protected:
	virtual void ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager) override final;
	virtual void SignalEntities(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context, FRecallSignalNameLookup& EntitySignals) override final;
};

UCLASS()
class URecallCarryableRepresentationProcessor : public UMassExtendedProcessor
{
	GENERATED_BODY()

public:
	URecallCarryableRepresentationProcessor();

	void InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager) override final;

protected:
	virtual void ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager) override final;
	virtual void Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context) override final;

private:
	FMassExtendedEntityQuery EntityQuery;
};
