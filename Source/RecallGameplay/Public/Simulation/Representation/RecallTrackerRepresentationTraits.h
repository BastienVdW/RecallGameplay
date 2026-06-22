// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "MassEntityTraitBase.h"

#include "RecallTrackerRepresentationTraits.generated.h"

/**
 * Trait for an entity that can be tracked by the UI.
 */
UCLASS(meta=(DisplayName="RE Tracker UI"))
class URecallTrackerRepresentationTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

public:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;

protected:
	UPROPERTY(EditAnywhere, meta=(MustImplement="/Script/RecallGameplayCore.RecallTrackerEntityInterface"))
	TSubclassOf<UUserWidget> TrackerWidgetClass;
};
