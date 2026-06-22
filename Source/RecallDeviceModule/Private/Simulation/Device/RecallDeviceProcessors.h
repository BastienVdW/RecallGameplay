// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "MassExtendedProcessor.h"

#include "RecallDeviceProcessors.generated.h"

/**
* This processor handle building a device.
*/
UCLASS()
class RECALLDEVICEMODULE_API URecallDeviceBuildProcessor : public UMassExtendedProcessor
{
	GENERATED_BODY()

public:
	URecallDeviceBuildProcessor();

	void InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager) override final;
	
protected:
	virtual void ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager) override final;
	virtual void Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context) override final;

private:
	FMassExtendedEntityQuery EntityQuery;
};
