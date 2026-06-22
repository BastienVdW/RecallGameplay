// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallCarryableTrackerWidget.h"

#include "CommonRichTextBlock.h"
#include "MassExtendedEntityTypes.h"
#include "RecallFrontendUtils.h"
#include "Representation/Carry/RecallCarryRepresentationTypes.h"
#include "Utility/Representation/RecallGameplayRepresentationFunctionLibrary.h"

#define LOCTEXT_NAMESPACE "URecallCarryableTrackerWidget"

URecallCarryableTrackerWidget::URecallCarryableTrackerWidget()
	: Super()
{
	CarryableText = NSLOCTEXT("URecallCarryableTrackerWidget", "CarryableText", "{CarrierCount} / {CarrierMin}-{CarrierMax}");
}

void URecallCarryableTrackerWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	Recall::Frontend::Utils::RegisterGlobalObserver<IRecallRepresentationReactInterface>(this);
}

void URecallCarryableTrackerWidget::NativeDestruct()
{
	Super::NativeDestruct();
	
	Recall::Frontend::Utils::UnregisterAllGlobalObservers(this);
}

void URecallCarryableTrackerWidget::SetTrackedEntity_Implementation(const FMassExtendedEntityHandle& Entity)
{
	TrackedEntity = Entity;
}

void URecallCarryableTrackerWidget::OnRender()
{
	const FRecallCarryableRepresentation Representation = URecallGameplayRepresentationFunctionLibrary::GetCarryableRepresentation(
		this, TrackedEntity);
	SetCarryableInfo(Representation.CarrierCount, Representation.CarrierRange);
}

void URecallCarryableTrackerWidget::SetCarryableInfo(int32 CarrierCount, const FInt32Range& CarrierRange)
{
	if (CommonRichTextBlock_Info)
	{
		FFormatNamedArguments Args;
		Args.Add("CarrierCount", CarrierCount);
		Args.Add("CarrierMin", CarrierRange.GetLowerBound().GetValue());
		Args.Add("CarrierMax", CarrierRange.GetUpperBound().GetValue());

		const FText ResultText = FText::Format(CarryableText, Args);

		CommonRichTextBlock_Info->SetText(ResultText);
	}
}

#undef LOCTEXT_NAMESPACE
