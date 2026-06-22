// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "ExtendedCommonUserWidget.h"
#include "Observer/Representation/RecallRepresentationReactInterface.h"
#include "Representation/Tracker/RecallTrackerEntityInterface.h"
#include "GameplayTagContainer.h"
#include "MassExtendedEntityHandle.h"

#include "RecallAttributeGroupWidget.generated.h"

#define RECALL_ATTRIBUTE_GROUP_WIDGET_COUNT 4

struct FRecallAttributeRepresentation;

/**
 * Widget that contains multiple attributes.
 */
UCLASS(Abstract)
class RECALLGAMEPLAYREPRESENTATION_API URecallAttributeGroupWidget :
	public UExtendedCommonUserWidget,
	public IRecallTrackerEntityInterface,
	public IRecallRepresentationReactInterface
{
	GENERATED_BODY()

public:
	void SetEntity(const FMassExtendedEntityHandle& Entity);
	void SetAttributes(const TMap<FGameplayTag, FRecallAttributeRepresentation>& AttributeMap) const;

	// UUserWidget implementation Begin
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	// UUserWidget implementation End

	// IRecallTrackerEntityInterface implementation Begin
protected:
	virtual void SetTrackedEntity_Implementation(const FMassExtendedEntityHandle& Entity) override;
	// IRecallTrackerEntityInterface implementation End

protected:
	// IRecallRepresentationReactInterface implementation begin
	virtual void OnRender() override;
	// IRecallRepresentationReactInterface implementation end
	
protected:
	/**
	 * Attributes to display, if empty then display all the attributes.
	 */
	UPROPERTY(EditAnywhere, Category=Attribute)
	FGameplayTagContainer Attributes;
	
	/**
	 * If true, hides attributes that are at their maximum value.
	 */
	UPROPERTY(EditAnywhere, Category=Attribute)
	bool bHideFullAttribute = false;
	
	/**
	 * If true, automatically tracks the owning player's entity when no specific entity is set.
	 */
	UPROPERTY(EditAnywhere, Category=Attribute)
	bool bDefaultToOwningPlayerEntity = true;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<class URecallAttributeWidget> Attribute1;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<class URecallAttributeWidget> Attribute2;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<class URecallAttributeWidget> Attribute3;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<class URecallAttributeWidget> Attribute4;

	UPROPERTY(Transient, BlueprintReadOnly, meta=(ExposeOnSpawn))
	FMassExtendedEntityHandle TrackedEntity;
	
	URecallAttributeWidget* GetAttributeWidget(int32 Index) const;
};
