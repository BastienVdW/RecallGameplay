// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallInteractGameplayTagCommandTypes.h"

#include "Mass/EntityElementTypes.h"
#include "MassEntityView.h"
#include "Simulation/GameplayTag/RecallGameplayTagFragments.h"

void FRecallInteractGameplayTagCommand::OnExecute(const FRecallInteractContext& Context) const
{
	const FMassEntityHandle TargetEntity = Context.GetTargetEntity(Target);
	const FMassEntityView TargetView(Context.GetEntityManagerChecked(), TargetEntity);

	FRecallGameplayTagFragment* InstigatorGameplayTagFragmentPtr = TargetView.GetFragmentDataPtr<FRecallGameplayTagFragment>();
	if (InstigatorGameplayTagFragmentPtr == nullptr)
	{
		return;
	}

	switch (Operation)
	{
	case ERecallInteractGameplayTagOperation::Add:
		InstigatorGameplayTagFragmentPtr->GameplayTagCountMap.AddTags(GameplayTags);
		break;
		
	case ERecallInteractGameplayTagOperation::Remove:
		InstigatorGameplayTagFragmentPtr->GameplayTagCountMap.RemoveTags(GameplayTags);
		break;

	default:
		unimplemented();
		break;
	}
}
