// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallDeviceCostContextTypes.h"

#include "MassEntityManager.h"
#include "MassEntityView.h"

//----------------------------------------------------------------------//
// FRecallDeviceCostContext
//----------------------------------------------------------------------//
UWorld* FRecallDeviceCostContext::GetWorld() const
{
	return EntityManager.GetWorld();
}

FMassEntityView FRecallDeviceCostContext::GetEntityView() const
{
	return FMassEntityView(GetEntityManager(), Entity);
}

const FMassEntityManager& FRecallDeviceCostContext::GetEntityManager() const
{
	return EntityManager;
}
