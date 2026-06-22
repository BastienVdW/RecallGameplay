// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "RecallSignalProcessorBase.h"

#include "RecallCollectProcessors.generated.h"

/**
 * Processor to handle collectable being collected by collect point
 */
UCLASS()
class URecallCollectPointSignalProcessor : public URecallSignalProcessorBase
{
	GENERATED_BODY()

public:
	URecallCollectPointSignalProcessor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager) override final;

protected:
	virtual void ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager) override final;
	virtual void SignalEntities(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context, FRecallSignalNameLookup& EntitySignals) override final;
};

/**
 * Processor to handle signals for collectable
 */
UCLASS()
class URecallCollectableSignalProcessor : public URecallSignalProcessorBase
{
	GENERATED_BODY()

public:
	URecallCollectableSignalProcessor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager) override final;

protected:
	virtual void ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager) override final;
	virtual void SignalEntities(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context, FRecallSignalNameLookup& EntitySignals) override final;
};
