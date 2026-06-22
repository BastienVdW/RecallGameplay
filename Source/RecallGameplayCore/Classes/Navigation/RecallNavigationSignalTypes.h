// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"

namespace Recall::Navigation::Signals
{

	const FName MoveAtDone = FName(TEXT("RecallMoveAtDone"));
	const FName MoveAtAbort = FName(TEXT("RecallMoveAtAbort"));

	const FName EnvQueryDone = FName(TEXT("RecallEnvQueryDone"));

} // namespace Recall::Navigation::Signals
