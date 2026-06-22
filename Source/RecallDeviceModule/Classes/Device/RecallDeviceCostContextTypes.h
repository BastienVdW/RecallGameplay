// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"

struct FMassExtendedEntityHandle;
struct FMassExtendedEntityManager;
struct FMassExtendedEntityView;

struct RECALLDEVICEMODULE_API FRecallDeviceCostContext
{
public:
	const FMassExtendedEntityManager& EntityManager;
	const FMassExtendedEntityHandle& Entity;

public:
	UWorld* GetWorld() const;
	FMassExtendedEntityView GetEntityView() const;
	const FMassExtendedEntityManager& GetEntityManager() const;
};
