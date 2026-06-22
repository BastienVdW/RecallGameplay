// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

#include "RecallAttributeSetTypes.generated.h"

RECALLATTRIBUTEMODULE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Health);
RECALLATTRIBUTEMODULE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Stamina);

#define RECALL_ATTRIBUTE_MOD_HANDLE_NULL 0

struct FRecallAttributeDefinition;
struct FRecallAttributeSetDefinition;
class URecallAttributeMod;
class URecallAttributeSetAsset;

USTRUCT()
struct RECALLATTRIBUTEMODULE_API FRecallAttribute
{
	GENERATED_BODY()

public:
	FRecallAttribute() = default;
	FRecallAttribute(const FRecallAttributeDefinition& Definition);

public:
	float GetBaseValue() const;
	void SetBaseValue(float NewBaseValue);
	
protected:
	UPROPERTY(VisibleAnywhere)
	float BaseValue = 0.0f;
};

USTRUCT()
struct RECALLATTRIBUTEMODULE_API FRecallAttributeModHandle
{
	GENERATED_BODY()

	/**
	 * Handle serial number used to reference this mod handle.
	 */
	UPROPERTY(VisibleAnywhere)
	uint32 SerialNumber = RECALL_ATTRIBUTE_MOD_HANDLE_NULL;

	/**
	 * Attribute mod associated to this handle.
	 */
	UPROPERTY(VisibleAnywhere)
	TSubclassOf<URecallAttributeMod> ModClass;
};

USTRUCT()
struct RECALLATTRIBUTEMODULE_API FRecallAttributeSet
{
	GENERATED_BODY()

public:
	FRecallAttributeSet() = default;
	FRecallAttributeSet(const TObjectPtr<const URecallAttributeSetAsset>& InAttributeSet);

	/**
	 * Return true if this set contains this attribute.
	 */
	bool HasAttribute(const FGameplayTag& Attribute) const;

	/**
	 * Return an array with all the attributes.
	 */
	void GetAttributeTagArray(TArray<FGameplayTag>& InOutGameplayTags) const;

	/**
	 * Return the base value for this attribute.
	 */
	float GetBaseValue(const FGameplayTag& Attribute, float DefaultValue = 0.0f) const;
	
	/**
	 * Return the value for this attribute.
	 * Default value is returned if attribute does not exist.
	 */
	float GetValue(const FGameplayTag& Attribute, float DefaultValue = 0.0f) const;

	/**
	 * Set the attribute value.
	 * The value will be clamped between the lower and higher bounds.
	 */
	void SetValue(const FGameplayTag& Attribute, float NewValue);

	/**
	 * Modify an attribute value by adding the modifier.
	 */
	void ModifyValue(const FGameplayTag& Attribute, float Modifier);

	/**
	 * Return the lower bound for this attribute value.
	 */
	float GetMinValue(const FGameplayTag& Attribute) const;
	
	/**
	 * Return the higher bound for this attribute value.
	 */
	float GetMaxValue(const FGameplayTag& Attribute) const;

	/**
	 * Return value as a percent between 0.0 and 1.0 (can potentially go above 1.0 if max value is ignored).
	 */
	float GetValueAsPercent(const FGameplayTag& Attribute) const;

	/**
	 * Add an attribute modifier and return a unique handle to keep track of it.
	 */
	uint32 AddMod(const TSubclassOf<URecallAttributeMod>& ModClass, bool bInstant = false);

	/**
	 * Remove an attribute modifier by using the unique handle that was returned when adding it.
	 */
	void RemoveModByHandle(uint32 Handle);

	/**
	 * Remove all the attribute modifiers that match this class.
	 */
	void RemoveModByClass(const TSubclassOf<URecallAttributeMod>& ModClass);
	
protected:
	/**
	 * Keep track of the attribute set asset for quick access.
	 */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<const URecallAttributeSetAsset> AttributeSet;
	
	/**
	 * Attributes are stored by tag.
	 */
	UPROPERTY(VisibleAnywhere, meta=(GameplayTagFilter="Attribute"))
	TMap<FGameplayTag, FRecallAttribute> Attributes;

	/**
	 * List of attribute modifiers.
	 */
	UPROPERTY(VisibleAnywhere)
	TArray<FRecallAttributeModHandle> ModHandles;

	/**
	 * Serial number generator for mod handles.
	 */
	UPROPERTY(VisibleAnywhere)
	uint32 ModHandleSerialNumberGenerator = 0;

private:
	const URecallAttributeSetAsset& GetAttributeSetChecked() const;
	void ApplyMods_Internal(const FGameplayTag& Attribute, float& Value, bool bIsBaseValue) const;
	uint32 AddMod_Internal(const TSubclassOf<URecallAttributeMod>& ModClass);
};
