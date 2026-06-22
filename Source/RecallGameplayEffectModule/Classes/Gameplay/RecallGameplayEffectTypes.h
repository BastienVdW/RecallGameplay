// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"

UENUM()
enum class ERecallGameplayEffectStackRule : uint8
{
	/**
	 * All the stacked effects are shared and will overwrite each other.
	 */
	Shared,

	/**
	 * Effects are stacked per owner, each owner has its own stack per target.
	 */
	PerOwner,
};

UENUM()
enum class ERecallGameplayEffectTickRule : uint8
{
	/**
	 * This gameplay effect is never ticked.
	 */
	None,
	
	/**
	 * This is done each frame by the gameplay effect system.
	 */
	Default,

	/**
	 * Tick has to be done manually by the system handling these effects.
	 */
	Custom,
};

/** Gameplay effect duration policies */
UENUM()
enum class ERecallGameplayEffectDurationType : uint8
{
	/** This effect applies instantly */
	Instant,
	/** This effect lasts forever */
	Infinite,
	/** The duration of this effect will be specified by a magnitude */
	HasDuration
};
