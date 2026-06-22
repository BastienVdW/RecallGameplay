// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "BaseGeneratedNavLinksProxy.h"
#include "StructUtils/InstancedStruct.h"

#include "RecallNavLinkProxy.generated.h"

UCLASS(Blueprintable, MinimalAPI)
class URecallNavLinkProxy : public UBaseGeneratedNavLinksProxy
{
	GENERATED_UCLASS_BODY()

public:
	// BEGIN INavLinkCustomInterface
	virtual bool OnLinkMoveStarted(class UObject* PathComp, const FVector& DestPoint) override;
	// END INavLinkCustomInterface

	UPROPERTY(EditAnywhere, meta=(BaseStruct="/Script/RecallGameplay.RecallNavLinkTraversalBase", ExcludeBaseStruct))
	FInstancedStruct NavLinkTraversal;
};
