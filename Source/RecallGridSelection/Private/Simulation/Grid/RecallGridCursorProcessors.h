// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "MassExtendedObserverProcessor.h"

#include "RecallGridCursorProcessors.generated.h"

UCLASS()
class URecallGridCursorOwnerInitializer : public UMassExtendedObserverProcessor
{
	GENERATED_BODY()

	URecallGridCursorOwnerInitializer();

public:
	virtual void InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager) override final;

protected:
	virtual void ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager) override final;
	virtual void Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context) override final;

private:
	FMassExtendedEntityQuery EntityQuery;
};

UCLASS()
class URecallGridCursorOwnerDeinitializer : public UMassExtendedObserverProcessor
{
	GENERATED_BODY()

	URecallGridCursorOwnerDeinitializer();

public:
	virtual void InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager) override final;

protected:
	virtual void ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager) override final;
	virtual void Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context) override final;

private:
	FMassExtendedEntityQuery EntityQuery;
};

UCLASS()
class URecallGridSelectionInitializer : public UMassExtendedObserverProcessor
{
	GENERATED_BODY()

	URecallGridSelectionInitializer();

public:
	virtual void InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager) override final;

protected:
	virtual void ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager) override final;
	virtual void Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context) override final;

private:
	FMassExtendedEntityQuery EntityQuery;
};

UCLASS()
class URecallGridCursorOwnerInputProcessor : public UMassExtendedProcessor
{
	GENERATED_BODY()

	URecallGridCursorOwnerInputProcessor();

public:
	virtual void InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager) override final;

protected:
	virtual void ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager) override final;
	virtual void Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context) override final;

private:
	FMassExtendedEntityQuery EntityQuery;
};

UCLASS()
class URecallGridSelectionPositionProcessor : public UMassExtendedProcessor
{
	GENERATED_BODY()

	URecallGridSelectionPositionProcessor();

public:
	virtual void InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager) override final;

protected:
	virtual void ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager) override final;
	virtual void Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context) override final;

private:
	FMassExtendedEntityQuery EntityQuery;
};

UCLASS()
class URecallGridSelectionRepresentationProcessor : public UMassExtendedProcessor
{
	GENERATED_BODY()

	URecallGridSelectionRepresentationProcessor();

public:
	virtual void InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager) override final;

protected:
	virtual void ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager) override final;
	virtual void Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context) override final;

private:
	FMassExtendedEntityQuery EntityQuery;
};

UCLASS()
class URecallGridCursorPositionProcessor : public UMassExtendedProcessor
{
	GENERATED_BODY()

	URecallGridCursorPositionProcessor();

public:
	virtual void InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager) override final;

protected:
	virtual void ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager) override final;
	virtual void Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context) override final;

private:
	FMassExtendedEntityQuery EntityQuery;
};

UCLASS()
class URecallGridCursorRepresentationProcessor : public UMassExtendedProcessor
{
	GENERATED_BODY()

	URecallGridCursorRepresentationProcessor();

public:
	virtual void InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager) override final;

protected:
	virtual void ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager) override final;
	virtual void Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context) override final;

private:
	FMassExtendedEntityQuery EntityQuery;
};
