// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallInteractGameplayEffectCommandTypes.h"

#include "Gameplay/RecallGameplayEffectContext.h"
#include "MassExtendedEntityView.h"
#include "Simulation/GameplayEffect/RecallGameplayEffectFragments.h"

//----------------------------------------------------------------------//
// FRecallInteractGameplayEffectCommand
//----------------------------------------------------------------------//
void FRecallInteractGameplayEffectCommand::OnBegin(const FRecallInteractContext& Context) const
{
	if (Action != ERecallInteractGameplayEffectAction::WhileActive)
	{
		return;
	}

	const FMassExtendedEntityHandle TargetEntity = Context.GetTargetEntity(Target);
	const FMassExtendedEntityView TargetView(Context.GetEntityManagerChecked(), TargetEntity);

	FRecallGameplayEffectFragment* GameplayEffectFragmentPtr = TargetView.GetFragmentDataPtr<FRecallGameplayEffectFragment>();
	if (GameplayEffectFragmentPtr == nullptr)
	{
		UE_LOG(LogRecallInteractCommand, Warning,
			TEXT("%hs Entity does not have a 'MS Gameplay Effect' trait attached"), __FUNCTION__);
		return;
	}

	const FRecallGameplayEffectContext EffectContext{ Context.GetEntityManagerChecked(), Context.InstigatorEntity };
	GameplayEffectFragmentPtr->Container.AddEffect(EffectContext, GameplayEffect);
}

void FRecallInteractGameplayEffectCommand::OnEnd(const FRecallInteractContext& Context) const
{
	if (Action != ERecallInteractGameplayEffectAction::WhileActive)
	{
		return;
	}

	const FMassExtendedEntityHandle TargetEntity = Context.GetTargetEntity(Target);
	const FMassExtendedEntityView TargetView(Context.GetEntityManagerChecked(), TargetEntity);

	FRecallGameplayEffectFragment* GameplayEffectFragmentPtr = TargetView.GetFragmentDataPtr<FRecallGameplayEffectFragment>();
	if (GameplayEffectFragmentPtr == nullptr)
	{
		UE_LOG(LogRecallInteractCommand, Warning,
			TEXT("%hs Entity does not have a 'MS Gameplay Effect' trait attached"), __FUNCTION__);
		return;
	}

	const FRecallGameplayEffectContext EffectContext{ Context.GetEntityManagerChecked(), Context.InstigatorEntity };
	GameplayEffectFragmentPtr->Container.RemoveEffectsByClass(EffectContext, GameplayEffect);
	GameplayEffectFragmentPtr->Container.RemoveEffectsByTag(EffectContext, RemoveEffectTag);
}

void FRecallInteractGameplayEffectCommand::OnExecute(const FRecallInteractContext& Context) const
{
	const FMassExtendedEntityHandle TargetEntity = Context.GetTargetEntity(Target);
	const FMassExtendedEntityView TargetView(Context.GetEntityManagerChecked(), TargetEntity);

	FRecallGameplayEffectFragment* GameplayEffectFragmentPtr = TargetView.GetFragmentDataPtr<FRecallGameplayEffectFragment>();
	if (GameplayEffectFragmentPtr == nullptr)
	{
		UE_LOG(LogRecallInteractCommand, Warning,
			TEXT("%hs Entity does not have a 'MS Gameplay Effect' trait attached"), __FUNCTION__);
		return;
	}

	const FRecallGameplayEffectContext EffectContext{ Context.GetEntityManagerChecked(), Context.InstigatorEntity };

	switch (Action)
	{
	case ERecallInteractGameplayEffectAction::Add:
		GameplayEffectFragmentPtr->Container.AddEffect(EffectContext, GameplayEffect);
		break;

	case ERecallInteractGameplayEffectAction::Remove:
		GameplayEffectFragmentPtr->Container.RemoveEffectsByClass(EffectContext, GameplayEffect);
		GameplayEffectFragmentPtr->Container.RemoveEffectsByTag(EffectContext, RemoveEffectTag);
		break;

	case ERecallInteractGameplayEffectAction::WhileActive:
		// WhileActive is handled in OnBegin/OnEnd, skip in OnExecute
		break;

	default:
		unimplemented();
		break;
	}
}
