// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "MassExtendedObserverProcessor.h"

#include "RecallCrowdProcessors.generated.h"

UCLASS()
class URecallCrowdAgentConstructor : public UMassExtendedObserverProcessor
{
	GENERATED_BODY()

public:
	URecallCrowdAgentConstructor();

	virtual void InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager) override final;

protected:
	virtual void ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager) override final;
	virtual void Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context) override final;

private:
	FMassExtendedEntityQuery EntityQuery;

};

UCLASS()
class URecallCrowdAgentDestructor : public UMassExtendedObserverProcessor
{
	GENERATED_BODY()

public:
	URecallCrowdAgentDestructor();

	virtual void InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager) override final;

protected:
	virtual void ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager) override final;
	virtual void Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context) override final;

private:
	FMassExtendedEntityQuery EntityQuery;

};

UCLASS()
class URecallCrowdAgentNavLinkTraversalConstructor : public UMassExtendedObserverProcessor
{
	GENERATED_BODY()

public:
	URecallCrowdAgentNavLinkTraversalConstructor();

	virtual void InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager) override final;

protected:
	virtual void ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager) override final;
	virtual void Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context) override final;

private:
	FMassExtendedEntityQuery EntityQuery;

};

UCLASS()
class URecallCrowdAgentNavLinkTraversalDestructor : public UMassExtendedObserverProcessor
{
	GENERATED_BODY()

public:
	URecallCrowdAgentNavLinkTraversalDestructor();

	virtual void InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager) override final;

protected:
	virtual void ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager) override final;
	virtual void Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context) override final;

private:
	FMassExtendedEntityQuery EntityQuery;

};

UCLASS()
class URecallCrowdAgentSetBlackboardProcessor : public UMassExtendedProcessor
{
	GENERATED_BODY()

	URecallCrowdAgentSetBlackboardProcessor();

public:
	virtual void InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager) override final;

protected:
	virtual void ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager) override final;
	virtual void Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context) override final;

private:
	FMassExtendedEntityQuery EntityQuery;
};

UCLASS()
class URecallCrowdAgentGetBlackboardProcessor : public UMassExtendedProcessor
{
	GENERATED_BODY()

	URecallCrowdAgentGetBlackboardProcessor();

public:
	virtual void InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager) override final;

protected:
	virtual void ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager) override final;
	virtual void Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context) override final;

private:
	FMassExtendedEntityQuery EntityQuery;
};

UCLASS()
class URecallCrowdUpdateProcessor : public UMassExtendedProcessor
{
	GENERATED_BODY()

	URecallCrowdUpdateProcessor();

public:
	virtual void InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager) override final;
	virtual bool ShouldAllowQueryBasedPruning(const bool bRuntimeMode = true) const override final;

protected:
	virtual void ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager) override final;
	virtual void Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context) override final;
};
