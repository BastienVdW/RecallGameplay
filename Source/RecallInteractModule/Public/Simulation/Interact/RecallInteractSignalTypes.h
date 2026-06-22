// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"

namespace Recall::Interact::Signals
{
	const FName Begin				= FName(TEXT("RecallInteractBegin"));
	const FName End					= FName(TEXT("RecallInteractEnd"));

	// Triggered when in progress - at a fixed rate
	const FName InProgressTick		= FName(TEXT("RecallInteractInProgressTick"));

	// Triggered when the interaction is complete
	const FName Execute				= FName(TEXT("RecallInteractExecute"));

	// Tick while the interactable is overlapping with the interactor
	const FName OverlapTick			= FName(TEXT("RecallInteractOverlapTick"));
} // namespace Recall::Interact::Signals
