// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallInteractContextTypes.h"

#include "MassExtendedExecutionContext.h"

//----------------------------------------------------------------------//
// FRecallInteractCommandContext
//----------------------------------------------------------------------//
UWorld* FRecallInteractContext::GetWorld() const
{
	return ExecutionContext.GetWorld();
}

UGameInstance* FRecallInteractContext::GetGameInstance() const
{
	if (const UWorld* World = GetWorld())
	{
		return GetWorld()->GetGameInstance();
	}
	return nullptr;
}

FMassExtendedEntityManager& FRecallInteractContext::GetEntityManagerChecked() const
{
	return ExecutionContext.GetEntityManagerChecked();
}

const FMassExtendedEntityHandle& FRecallInteractContext::GetTargetEntity(ERecallInteractTarget Target) const
{
	switch (Target)
	{
	case ERecallInteractTarget::Instigator:
		return InstigatorEntity;

	case ERecallInteractTarget::Owner:
		return InteractableEntity;

	default:
		unimplemented();
		return InteractableEntity;
	}
}
