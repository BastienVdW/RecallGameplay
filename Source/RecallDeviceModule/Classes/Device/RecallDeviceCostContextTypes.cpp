// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallDeviceCostContextTypes.h"

#include "MassExtendedEntityManager.h"
#include "MassExtendedEntityView.h"

//----------------------------------------------------------------------//
// FRecallDeviceCostContext
//----------------------------------------------------------------------//
UWorld* FRecallDeviceCostContext::GetWorld() const
{
	return EntityManager.GetWorld();
}

FMassExtendedEntityView FRecallDeviceCostContext::GetEntityView() const
{
	return FMassExtendedEntityView(GetEntityManager(), Entity);
}

const FMassExtendedEntityManager& FRecallDeviceCostContext::GetEntityManager() const
{
	return EntityManager;
}
