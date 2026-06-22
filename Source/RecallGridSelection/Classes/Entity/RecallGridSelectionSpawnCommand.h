// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "System/Entity/RecallEntityAsyncSpawnTypes.h"

#include "RecallGridSelectionSpawnCommand.generated.h"

USTRUCT()
struct RECALLGRIDSELECTION_API FRecallGridSelectionSpawnCommand : public FRecallEntityAsyncSpawnCommand
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	FMassEntityHandle SelectionOwnerEntity;
	
	// FRecallEntityAsyncSpawnCommand implementation Begin
public:	
	virtual void OnSpawn(FMassEntityManager& System, const TArray<FMassEntityHandle>& Entities) const override;
	// FRecallEntityAsyncSpawnCommand implementation End
};
