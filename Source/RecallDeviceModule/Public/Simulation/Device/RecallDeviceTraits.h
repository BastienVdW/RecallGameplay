// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "MassExtendedEntityTraitBase.h"

#include "RecallDeviceTraits.generated.h"

/**
* Trait for entities that can place devices.
*/
UCLASS(meta=(DisplayName="RE Device Placer"))
class RECALLDEVICEMODULE_API URecallDevicePlacerTrait : public UMassExtendedEntityTraitBase
{
	GENERATED_BODY()

public:
	virtual void BuildTemplate(FMassExtendedEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;
};
