// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "ExtendedCommonUserWidget.h"
#include "GameplayTagContainer.h"
#include "Representation/Attribute/RecallAttributeRepresentationTypes.h"

#include "RecallAttributeWidget.generated.h"

UENUM(BlueprintType, meta=(BitFlags, UseEnumValuesAsMaskValuesInEditor="true"))
enum class ERecallAttributeVisibility : uint8
{
	None				= 0,

	HideEmpty			= 1 << 0,
	HideFull			= 1 << 1,

	All					= HideEmpty | HideFull,
};
ENUM_CLASS_FLAGS(ERecallAttributeVisibility)

constexpr bool EnumHasAnyFlags(uint8 Flags, ERecallAttributeVisibility Contains) { return (Flags & static_cast<uint8>(Contains)) != 0; }

/**
 * Widget that contains an attribute.
 */
UCLASS(Abstract)
class RECALLGAMEPLAYREPRESENTATION_API URecallAttributeWidget :
	public UExtendedCommonUserWidget
{
	GENERATED_BODY()

public:
	URecallAttributeWidget();
	
	// UUserWidget implementation Begin
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	// UUserWidget implementation End

public:
	bool DoesSupportAttribute(const FGameplayTag& InAttribute) const;
	bool IsAttributeVisible(const FRecallAttributeRepresentation& InRepresentation) const;
	void SetAttribute(const FGameplayTag& InAttribute, const FRecallAttributeRepresentation& InRepresentation);

	UFUNCTION(BlueprintCallable)
	void SetAttributeText(UPARAM(meta=(GameplayTagFilter="Attribute")) FGameplayTag InAttribute, float CurrentValue, float MinValue, float MaxValue, float Percent);

protected:
	/**
	 * All the supported attributes for this widget.
	 * Child attributes will also be accepted.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Attribute, meta=(ExposeOnSpawn, GameplayTagFilter="Attribute"))
	FGameplayTagContainer Attributes;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Attribute, meta=(Bitmask, BitmaskEnum="/Script/RecallGameplayRepresentation.ERecallAttributeVisibility"))
	uint8 AttributeVisibility = 0;
	
	/**
	 * Text of how to format this attribute. Some args will be passed as parameters such as:
	 * AttributeName: The tag of the attribute as a string.
	 * CurrentValue: The current value of the attribute.
	 * MinValue: The minimum value of the attribute.
	 * MaxValue: The maximum value of the attribute.
	 * Percent: The percent between the min and max value for this attribute.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Attribute)
	FText AttributeText;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<class UProgressBar> ProgressBar_Attribute;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<class UCommonRichTextBlock> CommonRichTextBlock_Attribute;

};
