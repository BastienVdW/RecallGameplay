// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "MassExtendedEntityHandle.h"

#include "RecallGridRegistry.generated.h"

USTRUCT()
struct RECALLGRIDSELECTION_API FRecallGridCellEntry
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere)
	FMassExtendedEntityHandle Entity;
	
	UPROPERTY(VisibleAnywhere)
	int32 CellSerialNumber = 0;
};

USTRUCT()
struct RECALLGRIDSELECTION_API FRecallGridRegistry
{
	GENERATED_BODY()

public:
	int32 ReserveCell(int32 CellIndex);
	void RegisterCell(int32 CellIndex, const FMassExtendedEntityHandle& Entity);
	void UnregisterCell(int32 CellIndex);
	bool IsEmptyCell(int32 CellIndex) const;
	FMassExtendedEntityHandle GetCellEntity(int32 CellIndex) const;
	int32 GetCellReservationNumber(int32 CellIndex) const;
	void ResetGrid();
	
protected:
	UPROPERTY()
	TMap<int32, FRecallGridCellEntry> Cells;
	
	UPROPERTY(VisibleAnywhere)
	int32 CellSerialNumberGenerator = 0;
};
