// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"

namespace Recall::Cinematic::Signals
{

	/*
	* Signals to control our cinematic player
	*/
	namespace Player
	{
		const FName Play = FName(TEXT("RecallCinematicPlay"));
		const FName PlayLoop = FName(TEXT("RecallCinematicPlayLoop"));

		const FName Stop = FName(TEXT("RecallCinematicStop"));
	} // namespace Player

	/*
	* Callback signals triggered by the cinematic player
	*/
	namespace Callback
	{
		const FName OnStart = FName(TEXT("RecallCinematicOnStart"));
		const FName OnEnd = FName(TEXT("RecallCinematicOnEnd"));
	} // namespace Callback

} // namespace Recall::Cinematic::Signals
