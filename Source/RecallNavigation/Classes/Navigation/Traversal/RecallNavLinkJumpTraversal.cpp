// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallNavLinkJumpTraversal.h"

#include "Kismet/GameplayStatics.h"
#include "Physics/RecallPhysicsObjects.h"
#include "Simulation/Navigation/RecallNavLinkFragments.h"
#include "Simulation/Transform/RecallTransformFragments.h"
#include "Utility/Math/RecallMathUtils.h"

void FRecallNavLinkJumpTraversal::OnEnter(const FRecallNavLinkTraversalContext& Context) const
{
	if (!ensure(Context.PhysicsBody.IsValid()))
	{
		return;
	}
	
	const FVector& StartPos = Context.TransformFragment.Position;
	const FVector& EndPos = Context.NavLinkTraversalFragment.EndLocation;
	
	FVector LaunchVelocityPerSecond = FVector::ZeroVector;
	if (UGameplayStatics::SuggestProjectileVelocity_CustomArc(Context.GetWorld(),
		LaunchVelocityPerSecond, StartPos, EndPos, 0, ArcParam))
	{
		const FVector LaunchVelocityPerFrame = Recall::Math::Utils::UnitsPerSecondToPerFrame(LaunchVelocityPerSecond);
		Context.PhysicsBody.SetLinearVelocity(LaunchVelocityPerFrame);
	}
}

bool FRecallNavLinkJumpTraversal::OnTick(const FRecallNavLinkTraversalContext& Context) const
{
	const uint32 JumpDuration = Recall::Math::Utils::SecondsToFrames(JumpDurationSeconds);
	if (Context.NavLinkTraversalFragment.TraversalDuration < JumpDuration)
	{
		return true;
	}
	
	return false;
}

void FRecallNavLinkJumpTraversal::OnExit(const FRecallNavLinkTraversalContext& Context) const
{
}
