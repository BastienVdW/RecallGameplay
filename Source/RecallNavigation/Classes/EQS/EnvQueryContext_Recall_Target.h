// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "EnvironmentQuery/EnvQueryContext.h"

#include "EnvQueryContext_Recall_Target.generated.h"

/**
 * Target location for the env query request.
 */
UCLASS(MinimalAPI)
class UEnvQueryContext_Recall_Target : public UEnvQueryContext
{
	GENERATED_BODY()

public:
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
};
