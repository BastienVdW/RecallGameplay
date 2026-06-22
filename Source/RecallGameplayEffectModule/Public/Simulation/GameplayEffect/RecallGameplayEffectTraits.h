// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "MassExtendedEntityTraitBase.h"

#include "RecallGameplayEffectTraits.generated.h"

UCLASS(meta=(DisplayName="RE Gameplay Effect"))
class RECALLGAMEPLAYEFFECTMODULE_API URecallGameplayEffectTrait : public UMassExtendedEntityTraitBase
{
	GENERATED_BODY()

public:
	virtual void BuildTemplate(FMassExtendedEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;
};

UCLASS(meta=(DisplayName="RE Gameplay Effect Area"))
class RECALLGAMEPLAYEFFECTMODULE_API URecallGameplayEffectAreaTrait : public UMassExtendedEntityTraitBase
{
	GENERATED_BODY()

public:
	virtual void BuildTemplate(FMassExtendedEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;

protected:
	/**
	 * Effect applied by this area.
	 */
	UPROPERTY(EditAnywhere)
	TSubclassOf<URecallGameplayEffect> EffectClass;
};
