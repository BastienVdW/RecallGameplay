// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "Simulation/Interact/RecallInteractFragments.h"

#include "Data/Interact/RecallInteractAsset.h"

int32 FRecallInteractableFragment::GetInteractEventCount(const FRecallInteractableConstSharedFragment* ConstSharedFragmentPtr) const
{
	checkf(ConstSharedFragmentPtr != nullptr, TEXT("ConstSharedFragmentPtr is required"));

	// Count events from OverrideInteraction first
	if (OverrideInteraction.IsValid())
	{
		const FRecallInteractionParameters& OverrideParams = OverrideInteraction.Get<FRecallInteractionParameters>();
		return OverrideParams.NumEvent();
	}
	// Count events from InteractAsset hierarchy
	else if (const URecallInteractAsset* CurrentAsset = ConstSharedFragmentPtr->GetInteractAsset())
	{
		int32 TotalEventCount = 0;

		while (CurrentAsset != nullptr)
		{
			TotalEventCount += CurrentAsset->Interaction.NumEvent();
			CurrentAsset = CurrentAsset->ParentAsset.Get();
		}

		return TotalEventCount;
	}
	// Count events from DefaultInteraction
	else
	{
		return ConstSharedFragmentPtr->GetDefaultInteraction().NumEvent();
	}
}

const FRecallInteractionEvent& FRecallInteractableFragment::GetInteractEventChecked(int32 FlatIndex, 
	const FRecallInteractableConstSharedFragment* ConstSharedFragmentPtr) const
{
	checkf(ConstSharedFragmentPtr != nullptr, TEXT("ConstSharedFragmentPtr is required"));
	
	// Check OverrideInteraction first
	if (OverrideInteraction.IsValid())
	{
		const FRecallInteractionParameters& OverrideParams = OverrideInteraction.Get<FRecallInteractionParameters>();
		return OverrideParams.GetEventChecked(FlatIndex);
	}
	// Check InteractAsset hierarchy
	else if (const URecallInteractAsset* CurrentAsset = ConstSharedFragmentPtr->GetInteractAsset())
	{
		int32 CurrentOffset = 0;

		while (CurrentAsset != nullptr)
		{
			if (FlatIndex < CurrentOffset + CurrentAsset->Interaction.NumEvent())
			{
				return CurrentAsset->Interaction.GetEventChecked(FlatIndex - CurrentOffset);
			}
			CurrentOffset += CurrentAsset->Interaction.NumEvent();
			CurrentAsset = CurrentAsset->ParentAsset.Get();
		}

		checkNoEntry();
		return CurrentAsset->Interaction.GetEventChecked(0);
	}
	// Check DefaultInteraction last
	else
	{
		const FRecallInteractionParameters& DefaultParams = ConstSharedFragmentPtr->GetDefaultInteraction();
		return DefaultParams.GetEventChecked(FlatIndex);
	}
}

FName FRecallInteractableFragment::GetIconSocketName(const FRecallInteractableConstSharedFragment* ConstSharedFragmentPtr) const
{
	checkf(ConstSharedFragmentPtr != nullptr, TEXT("ConstSharedFragmentPtr is required"));

	// Check OverrideInteraction first
	if (OverrideInteraction.IsValid())
	{
		const FRecallInteractionParameters& OverrideParams = OverrideInteraction.Get<FRecallInteractionParameters>();
		return OverrideParams.IconSocketName;
	}
	// Check InteractAsset hierarchy
	else if (const URecallInteractAsset* CurrentAsset = ConstSharedFragmentPtr->GetInteractAsset())
	{
		while (CurrentAsset != nullptr)
		{
			if (CurrentAsset->Interaction.IconSocketName != NAME_None)
			{
				return CurrentAsset->Interaction.IconSocketName;
			}
			CurrentAsset = CurrentAsset->ParentAsset.Get();
		}
		
		return NAME_None;
	}
	// Check DefaultInteraction last
	else
	{
		return ConstSharedFragmentPtr->GetDefaultInteraction().IconSocketName;
	}
}

const FRecallInteractionParameters& FRecallInteractableFragment::GetInteraction(
	const FRecallInteractableConstSharedFragment* ConstSharedFragmentPtr) const
{
	if (OverrideInteraction.IsValid())
	{
		return OverrideInteraction.Get<FRecallInteractionParameters>();
	}

	checkf(ConstSharedFragmentPtr != nullptr, TEXT("No entry"));

	if (ConstSharedFragmentPtr->GetInteractAsset())
	{
		return ConstSharedFragmentPtr->GetInteractAsset()->Interaction;
	}
	
	return ConstSharedFragmentPtr->GetDefaultInteraction();
}

const FRecallInteractionEventData& FRecallInteractableFragment::GetInteractionEventDataChecked(int32 EventIndex,
	const FRecallInteractableConstSharedFragment* ConstSharedFragmentPtr) const
{
	checkf(EventIndex >= 0 && EventIndex < GetInteractEventCount(ConstSharedFragmentPtr), TEXT("Invalid event index %d"), EventIndex);
	InteractionEventData.SetNum(EventIndex + 1, EAllowShrinking::No);
	return InteractionEventData[EventIndex];
}

FRecallInteractionEventData& FRecallInteractableFragment::GetMutableInteractionEventDataChecked(int32 EventIndex,
	const FRecallInteractableConstSharedFragment* ConstSharedFragmentPtr)
{
	return const_cast<FRecallInteractionEventData&>(GetInteractionEventDataChecked(EventIndex, ConstSharedFragmentPtr));
}
