// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "RecallDialogReactInterface.generated.h"

struct FRecallDialogTableRow;

UINTERFACE()
class RECALLGAMEPLAYCORE_API URecallDialogReactInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class RECALLGAMEPLAYCORE_API IRecallDialogReactInterface
{
	GENERATED_IINTERFACE_BODY()
	
public:
	virtual void PlayDialog(const FRecallDialogTableRow& Dialog) = 0;
	virtual void StopDialog() = 0;

};
