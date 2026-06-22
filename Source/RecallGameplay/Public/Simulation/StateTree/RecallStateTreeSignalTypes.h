// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"

namespace Recall::StateTree::Signals
{

	const FName StateTreeStart				= FName(TEXT("RecallStateTreeStart"));
	const FName StateTreeStop				= FName(TEXT("RecallStateTreeStop"));
	const FName StateTreeActivate			= FName(TEXT("RecallStateTreeActivate"));
	const FName NewStateTreeTaskRequired	= FName(TEXT("RecallNewStateTreeTaskRequired"));
	const FName EventReceived				= FName(TEXT("RecallStateTreeEventReceived"));
	const FName TickRequired				= FName(TEXT("RecallStateTreeTickRequired"));

	// Transitions
	const FName DelayedTransitionWakeup		= FName(TEXT("RecallStateTreeDelayedTransitionWakeup"));

} // namespace Recall::StateTree::Signals
