// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "MassExtendedProcessor.h"

#include "RecallGameRuleCommandProcessors.generated.h"

/**
 * Processor that runs at FrameEnd to clean up any remaining unprocessed commands
 * This prevents commands from carrying over between frames and provides debugging info
 */
UCLASS()
class URecallGameRuleCommandCleanupProcessor : public UMassExtendedProcessor
{
    GENERATED_BODY()

public:
    URecallGameRuleCommandCleanupProcessor();

protected:
    virtual void ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager) override;
    virtual void Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context) override;
    virtual bool ShouldAllowQueryBasedPruning(const bool bRuntimeMode) const override { return false; }
};
