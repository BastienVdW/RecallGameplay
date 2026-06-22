// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "RecallSignalProcessorBase.h"

#include "RecallGameRuleProcessors.generated.h"

/**
 * Manage the flow defined in URecallGameRuleAsset, if any.
 */
UCLASS()
class URecallGameRuleProcessor : public UMassExtendedProcessor
{
	GENERATED_BODY()

public:
	URecallGameRuleProcessor();

	void InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager) override final;
	bool ShouldAllowQueryBasedPruning(const bool bRuntimeMode = true) const override final;
	
protected:
	virtual void ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager) override final;
	virtual void Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context) override final;
};
