// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "MassEntityTraitBase.h"
#include "System/Navigation/RecallNavigationTypes.h"
#include "Simulation/Navigation/RecallPathFollowingSettings.h"
#include "Simulation/Crowd/RecallCrowdAgentCollisionSettings.h"

#include "RecallNavigationTraits.generated.h"

/*
* Base trait for navigation agents
* Adds common path following fragments that work with any navigation system
*/
UCLASS(Abstract, Blueprintable, meta=(DisplayName="RE Navigation Agent Base"))
class RECALLNAVIGATION_API URecallNavigationAgentBaseTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

public:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;

protected:
	UPROPERTY(EditAnywhere)
	bool bNavigationObstacle = true;

	/** Path following settings (destination threshold, agent radius) */
	UPROPERTY(EditAnywhere)
	FRecallPathFollowingSettings PathFollowingSettings;

	UPROPERTY(EditAnywhere)
	bool bCrowdAgent = true;

	/** Crowd agent collision settings (agent height for collision) */
	UPROPERTY(EditAnywhere, meta=(EditCondition=bCrowdAgent))
	FRecallCrowdAgentCollisionSettings CrowdAgentCollisionSettings;
};

/*
* Trait to move an entity on the NavMesh navigation system
*/
UCLASS(meta=(DisplayName="RE Nav Mesh Agent"))
class RECALLNAVIGATION_API URecallNavigationAgentTrait : public URecallNavigationAgentBaseTrait
{
	GENERATED_BODY()

public:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;

protected:
	UPROPERTY(EditAnywhere)
	FRecallNavigationAgentSettings AgentSettings;
};
