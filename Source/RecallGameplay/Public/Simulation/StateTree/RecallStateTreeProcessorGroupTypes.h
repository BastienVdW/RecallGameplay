// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"

namespace Recall::StateTree::ProcessorGroupNames
{

	const FName StateTreeActivation			= FName(TEXT("RecallStateTreeActivation"));
	const FName StateTreeStart				= FName(TEXT("RecallStateTreeStart"));
	const FName StateTreeUpdate				= FName(TEXT("RecallStateTreeUpdate"));

} // namespace Recall::StateTree::ProcessorGroupNames
