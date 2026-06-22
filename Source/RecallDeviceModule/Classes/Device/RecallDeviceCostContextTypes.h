// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"

struct FMassEntityHandle;
struct FMassEntityManager;
struct FMassEntityView;

struct RECALLDEVICEMODULE_API FRecallDeviceCostContext
{
public:
	const FMassEntityManager& EntityManager;
	const FMassEntityHandle& Entity;

public:
	UWorld* GetWorld() const;
	FMassEntityView GetEntityView() const;
	const FMassEntityManager& GetEntityManager() const;
};
