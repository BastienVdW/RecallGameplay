// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Interact/RecallInteractCommandTypes.h"
#include "UI/Event/Device/RecallDeviceUIEventTypes.h"

#include "RecallInteractDeviceCommandTypes.generated.h"

/**
 * Play the Device associated with this entity.
 * The entity must have the "RE Device" trait.
 */
USTRUCT(DisplayName="Build Device")
struct RECALLDEVICEMODULE_API FRecallInteractDeviceBuildCommand : public FRecallInteractCommand
{
	GENERATED_BODY()

public:
	virtual void OnBegin(const FRecallInteractContext& Context) const override;
	virtual void OnEnd(const FRecallInteractContext& Context) const override;
	virtual void OnExecute(const FRecallInteractContext& Context) const override;
	virtual bool CanEndInteraction(const FRecallInteractContext& Context) const override;

protected:
	UPROPERTY(EditAnywhere, meta=(Categories="UI.Event"), DisplayName="Begin UI Event Tag")
	FGameplayTag BeginUIEventTag = UIEvent_OpenDeviceSelection;
	
	UPROPERTY(EditAnywhere, meta=(Categories="UI.Event"), DisplayName="End UI Event Tag")
	FGameplayTag EndUIEventTag = UIEvent_CloseDeviceSelection;
};
