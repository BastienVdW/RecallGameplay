// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "MassEntityTraitBase.h"

#include "RecallCinematicTraits.generated.h"

/*
* Trait so this entity can trigger level sequences
*/
UCLASS(meta=(DisplayName="RE Cinematic"))
class RECALLGAMEPLAY_API URecallCinematicTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

public:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;

protected:
	UPROPERTY(EditAnywhere, meta = (AllowedClasses = "/Script/LevelSequence.LevelSequenceActor"))
	FSoftClassPath LevelSequenceActorClass;

	UPROPERTY(VisibleAnywhere)
	FName DefaultInstigatorBinding = TEXT("Instigator");
};
