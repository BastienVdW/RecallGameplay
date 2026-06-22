// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "Attribute/RecallAttributeSetTypes.h"

#include "RecallAttributeDefinitionTypes.h"
#include "RecallAttributeModTypes.h"

UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Health,		"Attribute.Health",			"Health Attribute");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Stamina, 		"Attribute.Stamina",		"Stamina Attribute");

//----------------------------------------------------------------------//
// FRecallAttribute
//----------------------------------------------------------------------//
FRecallAttribute::FRecallAttribute(const FRecallAttributeDefinition& Definition)
	: BaseValue(Definition.DefaultValue)
{
}

float FRecallAttribute::GetBaseValue() const
{
	return BaseValue;
}

void FRecallAttribute::SetBaseValue(float NewBaseValue)
{
	BaseValue = NewBaseValue;
}

//----------------------------------------------------------------------//
// FRecallAttributeSet
//----------------------------------------------------------------------//
FRecallAttributeSet::FRecallAttributeSet(const TObjectPtr<const URecallAttributeSetAsset>& InAttributeSet)
	: AttributeSet(InAttributeSet)
{
	for (const FGameplayTag& AttributeTag : GetAttributeSetChecked().GetAttributeTags())
	{
		const FRecallAttributeDefinition& AttributeDefinition = GetAttributeSetChecked().GetAttributeChecked(AttributeTag);
		
		Attributes.Add(AttributeTag, AttributeDefinition);
		
		SetValue(AttributeTag, AttributeDefinition.DefaultValue);
	}
					
	for (const TSubclassOf<URecallAttributeMod>& ModClass : GetAttributeSetChecked().GetDefaultModClasses())
	{
		AddMod(ModClass);
	}
}

bool FRecallAttributeSet::HasAttribute(const FGameplayTag& Attribute) const
{
	return Attributes.Contains(Attribute);
}

void FRecallAttributeSet::GetAttributeTagArray(TArray<FGameplayTag>& InOutGameplayTags) const
{
	Attributes.GetKeys(InOutGameplayTags);
}

float FRecallAttributeSet::GetMinValue(const FGameplayTag& Attribute) const
{
	if (HasAttribute(Attribute))
	{
		const FRecallAttributeDefinition& AttributeDefinition = GetAttributeSetChecked().GetAttributeChecked(Attribute);
		return AttributeDefinition.MinValue.GetValue(*this);
	}
	else
	{		
		return 0.0f;
	}
}

float FRecallAttributeSet::GetMaxValue(const FGameplayTag& Attribute) const
{
	if (HasAttribute(Attribute))
	{
		const FRecallAttributeDefinition& AttributeDefinition = GetAttributeSetChecked().GetAttributeChecked(Attribute);
		return AttributeDefinition.MaxValue.GetValue(*this);
	}
	else
	{		
		return 0.0f;
	}
}

float FRecallAttributeSet::GetValueAsPercent(const FGameplayTag& Attribute) const
{
	if (!HasAttribute(Attribute))
	{
		return 0.0f;
	}
	
	const FRecallAttributeDefinition& AttributeDefinition = GetAttributeSetChecked().GetAttributeChecked(Attribute);

	const float Value = GetValue(Attribute);
	const float MinValue = AttributeDefinition.MinValue.GetValue(*this);
	const float MaxValue = AttributeDefinition.MaxValue.GetValue(*this);

	const float Range = MaxValue - MinValue;
	if (Range > 0.0f)
	{
		return (Value - MinValue) / Range;
	}
	else
	{
		return 0.0f;
	}
}

float FRecallAttributeSet::GetBaseValue(const FGameplayTag& Attribute, float DefaultValue /*= 0.0f*/) const
{
	float BaseValue = DefaultValue;
	
	if (const FRecallAttribute* AttributePtr = Attributes.Find(Attribute))
	{
		BaseValue = AttributePtr->GetBaseValue();
	}

	ApplyMods_Internal(Attribute, BaseValue, true);

	return BaseValue;
}

float FRecallAttributeSet::GetValue(const FGameplayTag& Attribute, float DefaultValue /*= 0.0f*/) const
{
	float Value = GetBaseValue(Attribute, DefaultValue);
	ApplyMods_Internal(Attribute, Value, false);

	return Value;
}

void FRecallAttributeSet::SetValue(const FGameplayTag& Attribute, float NewValue)
{
	FRecallAttribute* AttributePtr = Attributes.Find(Attribute);
	if (AttributePtr == nullptr)
	{
		UE_LOG(LogRecallAttribute, Log,
			TEXT("%hs Entity does not have attribute: %s"), __FUNCTION__, *Attribute.ToString());
		return;
	}
	
	const FRecallAttributeDefinition& AttributeDefinition = GetAttributeSetChecked().GetAttributeChecked(Attribute);

	const float MinValue = AttributeDefinition.MinValue.GetValue(*this);
	const float MaxValue = AttributeDefinition.MaxValue.GetValue(*this);

	AttributePtr->SetBaseValue(FMath::Clamp(NewValue, MinValue, MaxValue));
}

void FRecallAttributeSet::ModifyValue(const FGameplayTag& Attribute, float Modifier)
{	
	const float OldValue = GetValue(Attribute);
	SetValue(Attribute, OldValue + Modifier);
}

uint32 FRecallAttributeSet::AddMod(const TSubclassOf<URecallAttributeMod>& ModClass, bool bInstant)
{
	if (bInstant)
	{
		const URecallAttributeMod* ModCDO = ModClass ? ModClass->GetDefaultObject<URecallAttributeMod>() : nullptr;
		if (ModCDO)
		{
			ModCDO->ApplyModInstant(*this);
		}
		
		return RECALL_ATTRIBUTE_MOD_HANDLE_NULL;
	}
	else
	{
		return AddMod_Internal(ModClass);
	}
}

uint32 FRecallAttributeSet::AddMod_Internal(const TSubclassOf<URecallAttributeMod>& ModClass)
{
	if (!ModClass)
	{
		return RECALL_ATTRIBUTE_MOD_HANDLE_NULL;
	}
	
	const URecallAttributeMod* ModCDO = ModClass->GetDefaultObject<URecallAttributeMod>();
	if (!ModCDO)
	{
		return RECALL_ATTRIBUTE_MOD_HANDLE_NULL;
	}
	
	int32 InsertIndex = 0;

	const int32 Priority = ModCDO->GetModPriority();

	for (; InsertIndex < ModHandles.Num(); ++InsertIndex)
	{
		const FRecallAttributeModHandle& Handle = ModHandles[InsertIndex];
		checkf(Handle.ModClass,
			TEXT("%hs Mod reference has been GC or not properly serialized"), __FUNCTION__);
		if (ModCDO->GetModPriority() > Priority)
		{
			break;
		}
	}
		
	FRecallAttributeModHandle NewHandle;
	NewHandle.SerialNumber = ++ModHandleSerialNumberGenerator;
	NewHandle.ModClass = ModClass;
	
	ModHandles.Insert(NewHandle, InsertIndex);
	return NewHandle.SerialNumber;
}

void FRecallAttributeSet::RemoveModByHandle(uint32 Handle)
{
	if (Handle == RECALL_ATTRIBUTE_MOD_HANDLE_NULL)
	{
		return;
	}

	ModHandles.RemoveAll([Handle](const FRecallAttributeModHandle& ModHandle)
	{
		return ModHandle.SerialNumber == Handle;
	});
}

void FRecallAttributeSet::RemoveModByClass(const TSubclassOf<URecallAttributeMod>& ModClass)
{
	if (!ModClass)
	{
		return;
	}

	ModHandles.RemoveAll([ModClass](const FRecallAttributeModHandle& ModHandle)
	{
		return ModHandle.ModClass == ModClass;
	});
}

const URecallAttributeSetAsset& FRecallAttributeSet::GetAttributeSetChecked() const
{
	checkf(AttributeSet, TEXT("%hs Invalid attribute set"), __FUNCTION__);
	return *AttributeSet;
}

void FRecallAttributeSet::ApplyMods_Internal(const FGameplayTag& Attribute, float& Value,
	bool bIsBaseValue) const
{
	for (const FRecallAttributeModHandle& ModHandle : ModHandles)
	{
		checkf(ModHandle.ModClass,
			TEXT("%hs Mod reference has been GC or not properly serialized"), __FUNCTION__);

		const URecallAttributeMod* ModCDO = ModHandle.ModClass->GetDefaultObject<URecallAttributeMod>();
		if (bIsBaseValue)
		{
			ModCDO->ApplyModBaseValue(*this, Attribute, Value);
		}
		else
		{				
			ModCDO->ApplyModValue(*this, Attribute, Value);
		}
	}
}
