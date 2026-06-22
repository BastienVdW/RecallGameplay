// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallAttributeGroupWidget.h"

#include "RecallAttributeWidget.h"
#include "RecallFrontendUtils.h"
#include "Player/Interface/RecallPlayerStateInterface.h"
#include "Utility/Player/RecallPlayerUtils.h"
#include "Utility/Representation/RecallGameplayRepresentationFunctionLibrary.h"

void URecallAttributeGroupWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	Recall::Frontend::Utils::RegisterGlobalObserver<IRecallRepresentationReactInterface>(this);
}

void URecallAttributeGroupWidget::NativeDestruct()
{
	Super::NativeDestruct();
	
	Recall::Frontend::Utils::UnregisterAllGlobalObservers(this);
	TrackedEntity.Reset();
}

void URecallAttributeGroupWidget::SetTrackedEntity_Implementation(const FMassExtendedEntityHandle& Entity)
{
	if (TrackedEntity == Entity)
	{
		return;
	}
	
	TrackedEntity = Entity;
}

void URecallAttributeGroupWidget::OnRender()
{
	if (!TrackedEntity.IsSet())
	{
		if (!bDefaultToOwningPlayerEntity)
		{
			return;
		}
		
		const IRecallPlayerStateInterface* PlayerState = Cast<IRecallPlayerStateInterface>(GetOwningPlayerState());
		if (!PlayerState || !Recall::Player::Utils::FindPlayerEntityInWorld(GetWorld(), PlayerState->GetSimPlayerId(), TrackedEntity))
		{
			return;
		}
	}
	
	const FRecallAttributeTrackerRepresentation AttributeRepresentation = URecallGameplayRepresentationFunctionLibrary::GetAttributeTrackerRepresentation(
		this, TrackedEntity);
	
	SetAttributes(AttributeRepresentation.Attributes);
}

void URecallAttributeGroupWidget::SetEntity(const FMassExtendedEntityHandle& Entity)
{
	IRecallTrackerEntityInterface::Execute_SetTrackedEntity(this, Entity);
}

void URecallAttributeGroupWidget::SetAttributes(
	const TMap<FGameplayTag, FRecallAttributeRepresentation>& AttributeMap) const
{
	TArray<FGameplayTag> Tags;

	if (Attributes.IsEmpty())
	{
		AttributeMap.GetKeys(Tags);
	}
	else
	{
		Attributes.GetGameplayTagArray(Tags);
	}

	for (int32 WidgetIndex = 0; WidgetIndex < RECALL_ATTRIBUTE_GROUP_WIDGET_COUNT; WidgetIndex++)
	{
		URecallAttributeWidget* AttributeWidget = GetAttributeWidget(WidgetIndex);
		if (!IsValid(AttributeWidget))
		{
			continue;
		}
		
		const int32 TagIndex = Tags.IndexOfByPredicate([AttributeWidget](const FGameplayTag& Tag)
		{
			return AttributeWidget->DoesSupportAttribute(Tag);	
		});

		if (TagIndex != INDEX_NONE)
		{
			const FGameplayTag& AttributeTag = Tags[TagIndex];
			const FRecallAttributeRepresentation AttributeRepresentation = AttributeMap.FindChecked(AttributeTag);

			if (AttributeWidget->IsAttributeVisible(AttributeRepresentation))
			{
				AttributeWidget->SetAttribute(AttributeTag, AttributeRepresentation);
				AttributeWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
			else
			{
				AttributeWidget->SetVisibility(ESlateVisibility::Collapsed);
			}
			
			Tags.RemoveAt(TagIndex);
		}
		else
		{
			AttributeWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

URecallAttributeWidget* URecallAttributeGroupWidget::GetAttributeWidget(int32 Index) const
{
	switch (Index)
	{
	case 0:
		return Attribute1;
		
	case 1:
		return Attribute2;
		
	case 2:
		return Attribute3;
		
	case 3:
		return Attribute4;

	default:
		checkNoEntry();
		return nullptr;
	}
}
