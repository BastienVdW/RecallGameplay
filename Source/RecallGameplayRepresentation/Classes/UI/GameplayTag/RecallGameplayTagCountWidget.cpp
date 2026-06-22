// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallGameplayTagCountWidget.h"

#include "RecallFrontendUtils.h"
#include "Utility/Representation/RecallGameplayRepresentationFunctionLibrary.h"

void URecallGameplayTagCountWidget::NativeConstruct()
{
	Super::NativeConstruct();

	OnGameplayEffectTagCountChanged(GameplayTag, GameplayTagCount);
	
	Recall::Frontend::Utils::RegisterGlobalObserver<IRecallRepresentationReactInterface>(this);
}

void URecallGameplayTagCountWidget::NativeDestruct()
{
	Super::NativeDestruct();

	Recall::Frontend::Utils::UnregisterAllGlobalObservers(this);
}

void URecallGameplayTagCountWidget::SetEntity(const FMassEntityHandle& InEntity)
{
	Entity = InEntity;
}

void URecallGameplayTagCountWidget::SetGameplayTag(const FGameplayTag& Tag)
{
	GameplayTag = Tag;
}

void URecallGameplayTagCountWidget::OnRender()
{
	switch (Owner)
	{
	case ERecallGameplayTagCountOwner::Faction:
		SetGameplayTagCountFromFaction();
		break;

	case ERecallGameplayTagCountOwner::Controller:
		SetGameplayTagCountFromController();
		break;

	default:
		unimplemented();
		break;
	}
}

void URecallGameplayTagCountWidget::SetGameplayTagCountFromController()
{
	if (!Entity.IsSet())
	{
		if (ControllerGameplayTag.IsValid())
		{
			Entity = URecallGameplayRepresentationFunctionLibrary::GetEntityByTags(
				this, ControllerGameplayTag.GetSingleTagContainer());
		}
		else
		{
			const FString PlayerID = URecallGameplayRepresentationFunctionLibrary::GetLocalPlayerID(
				this, GetOwningLocalPlayerIndex());
			Entity = URecallGameplayRepresentationFunctionLibrary::GetPlayerEntity(
				this, PlayerID);
		}
	}
	
	const int32 NewGameplayTagCount = URecallGameplayRepresentationFunctionLibrary::GetGameplayTagCount(
		this, GameplayTag, Entity);
	SetGameplayTagCount(NewGameplayTagCount);
}

void URecallGameplayTagCountWidget::SetGameplayTagCountFromFaction()
{
	const FGameplayTagContainer FactionTags = bUseControllerFaction ?
		URecallGameplayRepresentationFunctionLibrary::GetFactionGameplayTags(
			this, GetOwningLocalPlayerIndex()) : FactionGameplayTags;
	const int32 NewGameplayTagCount = URecallGameplayRepresentationFunctionLibrary::GetFactionGameplayTagCount(
		this, FactionTags, GameplayTag);
	SetGameplayTagCount(NewGameplayTagCount);
}

void URecallGameplayTagCountWidget::SetGameplayTagCount(int32 Count)
{
	if (Count != GameplayTagCount)
	{
		GameplayTagCount = Count;
		OnGameplayEffectTagCountChanged(GameplayTag, GameplayTagCount);
	}
}
