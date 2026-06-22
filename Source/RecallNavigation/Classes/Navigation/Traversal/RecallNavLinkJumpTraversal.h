// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Navigation/RecallNavLinkTraversal.h"

#include "RecallNavLinkJumpTraversal.generated.h"

USTRUCT()
struct RECALLNAVIGATION_API FRecallNavLinkJumpTraversal : public FRecallNavLinkTraversalBase
{
	GENERATED_BODY()
	
public:
	virtual void OnEnter(const FRecallNavLinkTraversalContext& Context) const override;
	virtual bool OnTick(const FRecallNavLinkTraversalContext& Context) const override;
	virtual void OnExit(const FRecallNavLinkTraversalContext& Context) const override;

protected:
	UPROPERTY(EditAnywhere, meta=(Units=Seconds, ClampMin=0.0))
	float JumpDurationSeconds = 1.5f;

	UPROPERTY(EditAnywhere, meta=(ClampMin=0.0, ClampMax=1.0))
	float ArcParam = 0.25f;
};
