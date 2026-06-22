// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "System/Grid/RecallGridRegistry.h"

int32 FRecallGridRegistry::ReserveCell(int32 CellIndex)
{
	if (!ensure(IsEmptyCell(CellIndex)) && CellIndex >= 0)
	{
		return 0;
	}
	
	FRecallGridCellEntry& CellEntry = Cells.Add(CellIndex);
	CellEntry.CellSerialNumber = ++CellSerialNumberGenerator;
	return CellEntry.CellSerialNumber;
}

void FRecallGridRegistry::RegisterCell(int32 CellIndex, const FMassEntityHandle& Entity)
{
	if (!ensure(CellIndex >= 0 && Entity.IsValid()))
	{
		return;
	}
	
	FRecallGridCellEntry& CellEntry = Cells.FindOrAdd(CellIndex);
	if (!ensureMsgf(!CellEntry.Entity.IsSet(),
		TEXT("%hs Cell is already registered"), __FUNCTION__))
	{
		return;
	}

	CellEntry.Entity = Entity;
}

void FRecallGridRegistry::UnregisterCell(int32 CellIndex)
{
	ensure(Cells.Remove(CellIndex) == 1);
}

bool FRecallGridRegistry::IsEmptyCell(int32 CellIndex) const
{
	return !Cells.Contains(CellIndex);
}

FMassEntityHandle FRecallGridRegistry::GetCellEntity(int32 CellIndex) const
{
	if (const FRecallGridCellEntry* CellPtr = Cells.Find(CellIndex))
	{
		return CellPtr->Entity;
	}
	return FMassEntityHandle();
}

int32 FRecallGridRegistry::GetCellReservationNumber(int32 CellIndex) const
{
	if (const FRecallGridCellEntry* CellPtr = Cells.Find(CellIndex))
	{
		return CellPtr->CellSerialNumber;
	}
	return 0;
}

void FRecallGridRegistry::ResetGrid()
{
	Cells.Reset();
}
