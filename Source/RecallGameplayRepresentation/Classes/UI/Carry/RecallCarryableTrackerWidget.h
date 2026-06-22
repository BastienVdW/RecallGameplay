// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "ExtendedCommonUserWidget.h"
#include "Observer/Representation/RecallRepresentationReactInterface.h"
#include "Representation/Tracker/RecallTrackerEntityInterface.h"
#include "MassExtendedEntityTypes.h"

#include "RecallCarryableTrackerWidget.generated.h"

/**
 * Widget to display info about carryable entities.
 */
UCLASS(Abstract)
class RECALLGAMEPLAYREPRESENTATION_API URecallCarryableTrackerWidget :
	public UExtendedCommonUserWidget,
	public IRecallTrackerEntityInterface,
	public IRecallRepresentationReactInterface
{
	GENERATED_BODY()

public:
	URecallCarryableTrackerWidget();

public:
	UFUNCTION(BlueprintCallable)
	void SetCarryableInfo(int32 CarrierCount, const FInt32Range& CarrierRange);

	// UUserWidget implementation Begin
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	// UUserWidget implementation End

	// IRecallTrackerEntityInterface implementation Begin
public:
	virtual void SetTrackedEntity_Implementation(const FMassExtendedEntityHandle& Entity) override;
	// IRecallTrackerEntityInterface implementation End

public:
	// IRecallRepresentationReactInterface implementation begin
public:
	virtual void OnRender() override;
	// IRecallRepresentationReactInterface implementation end
	
protected:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<class UCommonRichTextBlock> CommonRichTextBlock_Info;

	/**
	 * Format info text about carryable.
	 * {CarrierCount}: How many entities are carrying the tracked entity.
	 * {CarrierMin}: Minimum amount of entities required to carry the tracked entity.
	 * {CarrierMax}: Maximum amount of entities allowed to carry the tracked entity.
	 */
	UPROPERTY(EditAnywhere)
	FText CarryableText;

private:
	UPROPERTY(Transient)
	FMassExtendedEntityHandle TrackedEntity;
};
