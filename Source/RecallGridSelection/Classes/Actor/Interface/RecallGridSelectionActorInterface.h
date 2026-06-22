// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "UObject/Interface.h"

#include "RecallGridSelectionActorInterface.generated.h"

UINTERFACE()
class RECALLGRIDSELECTION_API URecallGridSelectionActorInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class RECALLGRIDSELECTION_API IRecallGridSelectionActorInterface
{
	GENERATED_IINTERFACE_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category="Grid Selection")
	void SetSelectedCellIsEmpty(bool bIsEmpty);
};
