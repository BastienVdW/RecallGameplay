// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallInteractAttributeCommandTypes.h"

#include "MassExtendedEntityView.h"
#include "Simulation/Attribute/RecallAttributeFragments.h"

//----------------------------------------------------------------------//
// FRecallInteractAttributeCommand
//----------------------------------------------------------------------//
void FRecallInteractAttributeCommand::OnExecute(const FRecallInteractContext& Context) const
{
	const FMassExtendedEntityHandle TargetEntity = Context.GetTargetEntity(Target);
	const FMassExtendedEntityView TargetView(Context.GetEntityManagerChecked(), TargetEntity);
	FRecallAttributeFragment* AttributeFragmentPtr = TargetView.GetFragmentDataPtr<FRecallAttributeFragment>();
	if (AttributeFragmentPtr== nullptr)
	{
		return;
	}
	
	const float OldValue = AttributeFragmentPtr->AttributeSet.GetValue(Attribute);
	const float NewValue = OldValue + Modifier * Context.DeltaTime;
	
	AttributeFragmentPtr->AttributeSet.SetValue(Attribute, NewValue);
}

//----------------------------------------------------------------------//
// FRecallInteractAttributeModCommand
//----------------------------------------------------------------------//
void FRecallInteractAttributeModCommand::OnExecute(const FRecallInteractContext& Context) const
{
	const FMassExtendedEntityHandle TargetEntity = Context.GetTargetEntity(Target);
	const FMassExtendedEntityView TargetView(Context.GetEntityManagerChecked(), TargetEntity);
	FRecallAttributeFragment* AttributeFragmentPtr = TargetView.GetFragmentDataPtr<FRecallAttributeFragment>();
	if (AttributeFragmentPtr == nullptr)
	{
		return;
	}

	switch (Action)
	{
	case ERecallInteractAttributeModAction::Add:
		AttributeFragmentPtr->AttributeSet.AddMod(AttributeModClass);
		break;

	case ERecallInteractAttributeModAction::Remove:
		AttributeFragmentPtr->AttributeSet.RemoveModByClass(AttributeModClass);
		break;

	default:
		unimplemented();
		break;
	}
}
