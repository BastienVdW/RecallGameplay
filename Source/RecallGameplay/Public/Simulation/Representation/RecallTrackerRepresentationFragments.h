// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Mass/EntityElementTypes.h"

#include "RecallTrackerRepresentationFragments.generated.h"

USTRUCT()
struct FRecallTrackerRepresentationConstSharedFragment : public FMassConstSharedFragment
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, meta=(MustImplement="/Script/RecallGameplayCore.RecallTrackerEntityInterface"))
	TSubclassOf<UUserWidget> TrackerWidgetClass;
};
