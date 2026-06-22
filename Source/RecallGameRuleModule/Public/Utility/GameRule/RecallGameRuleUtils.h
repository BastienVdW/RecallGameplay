// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"

/**
 * Utility methods to access game rule system variables and data.
 */
namespace Recall::GameRule::Utils
{

/**
 * Get the match duration from the current game rule asset.
 */
RECALLGAMERULEMODULE_API extern float GetMatchDuration(const UObject* WorldContextObject);

/**
 * Get the pre-match duration from the current game rule asset.
 */
RECALLGAMERULEMODULE_API extern float GetPreMatchDuration(const UObject* WorldContextObject);

} // namespace Recall::GameRule::Utils