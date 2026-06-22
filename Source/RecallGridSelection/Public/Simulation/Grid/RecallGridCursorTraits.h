// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "MassEntityTraitBase.h"
#include "Representation/Actor/RecallActorMeshRepresentationTypes.h"

#include "RecallGridCursorTraits.generated.h"

/*
* Trait attached to the grid cursor entity.
*/
UCLASS(meta=(DisplayName="RE Grid Selection Cursor"))
class RECALLGRIDSELECTION_API URecallGridCursorTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

public:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;
};

/*
* Trait attached to the controller entity owning a cursor.
*/
UCLASS(meta=(DisplayName="RE Grid Selection Cursor Owner"))
class RECALLGRIDSELECTION_API URecallGridCursorOwnerTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

public:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;

protected:
	UPROPERTY(EditAnywhere, meta=(AllowedClasses="/Script/MassSpawner.MassEntityConfigAsset"))
	FSoftObjectPath GridSelectionEntityConfig;

	UPROPERTY(EditAnywhere)
	FActorRepresentationDesc CursorActorConfig;
};
