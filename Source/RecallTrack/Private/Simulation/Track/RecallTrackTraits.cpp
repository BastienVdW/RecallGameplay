// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "Simulation/Track/RecallTrackTraits.h"

#include "Actor/RecallTrackEntityActor.h"
#include "Component/RecallEntityComponent.h"
#include "MassEntityTemplateRegistry.h"
#include "Simulation/Track/RecallTrackFragments.h"

//----------------------------------------------------------------------//
// URecallTrackTrait
//----------------------------------------------------------------------//
void URecallTrackTrait::PostLoad()
{
	Super::PostLoad();

#if WITH_EDITOR
	if (!TrackActor)
	{
		if (const URecallEntityComponent* EntityComponent = Cast<URecallEntityComponent>(GetOuter()))
		{
			TrackActor = Cast<ARecallTrackEntityActor>(EntityComponent->GetOwner());
			if (TrackActor)
			{
				Modify();
			}
		}
	}
#endif // WITH_EDITOR
}

void URecallTrackTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	Super::BuildTemplate(BuildContext, World);
	
	FMassEntityManager& EntityManager = UE::Mass::Utils::GetEntityManagerChecked(World);
	
	FRecallTrackFragment& TrackFragment = BuildContext.AddFragment_GetRef<FRecallTrackFragment>();

	if (TrackActor)
	{
		TrackFragment.TrackAssetName = TrackActor->GetEntityAssetName();
	}
	
	FRecallTrackConstSharedFragment TrackConstSharedFragment;
	TrackConstSharedFragment.Params = Params;
	TrackConstSharedFragment.MeshShapeSettings = MeshShapeSettings;
	
	BuildContext.AddConstSharedFragment(EntityManager.GetOrCreateConstSharedFragment(TrackConstSharedFragment));
}
