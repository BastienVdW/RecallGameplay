// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "MassEntityTraitBase.h"
#include "Data/Interact/RecallInteractTypes.h"

#include "RecallInteractTraits.generated.h"

/**
* Trait for entities that can interact with other entities
*/
UCLASS(meta=(DisplayName="RE Interactor"))
class RECALLINTERACTMODULE_API URecallInteractorTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

public:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;

protected:
	UPROPERTY(EditAnywhere, meta=(GetOptions="GetSensorNames"))
	FName SensorName = NAME_None;

private:
	UFUNCTION()
	TArray<FName> GetSensorNames() const;
};

/**
* Trait for entities that can be interacted with
*/
UCLASS(meta=(DisplayName="RE Interactable"))
class RECALLINTERACTMODULE_API URecallInteractableTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

public:
	URecallInteractableTrait();

public:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;

protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<class URecallInteractAsset> InteractAsset;

	UPROPERTY(EditAnywhere, meta=(ShowOnlyInnerProperties, EditCondition="InteractAsset == nullptr"))
	FRecallInteractionParameters Interaction;

	/**
	 * Component tag to detect interaction positions on actor.
	 * - If NAME_None, no position detection (backward compatible)
	 * - If set, searches actor for components with this tag
	 * - Each component's world location becomes an interaction position
	 */
	UPROPERTY(EditAnywhere, Category="Interaction|MultiPosition", meta=(
		Tooltip="Tag to identify scene components as interaction positions. Leave empty for single-position interact."))
	FName InteractionPositionTag = NAME_None;
};
