// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "Collection/VariableCollectionTypes.h"

#include "RecallAttributeDefinitionTypes.generated.h"

RECALLATTRIBUTEMODULE_API DECLARE_LOG_CATEGORY_EXTERN(LogRecallAttribute, Log, All);

struct FRecallAttributeSet;

USTRUCT(DisplayName="Attribute Bound Definition")
struct RECALLATTRIBUTEMODULE_API FRecallAttributeBoundDefinition
{
	GENERATED_BODY()

public:
	FRecallAttributeBoundDefinition() = default;
	FRecallAttributeBoundDefinition(float InDefaultValue);

public:
	float GetValue(const FRecallAttributeSet& AttributeSet) const;

protected:
	UPROPERTY(EditAnywhere)
	FVariableCollectionFloat DefaultValue = 0.0f;
	
	/**
	 * (Optional) Use another attribute as bound.
	 */
	UPROPERTY(EditAnywhere, meta=(GameplayTagFilter="Attribute"))
	FGameplayTag Attribute = FGameplayTag::EmptyTag;
};

USTRUCT(DisplayName="Attribute Definition")
struct RECALLATTRIBUTEMODULE_API FRecallAttributeDefinition
{
	GENERATED_BODY()

	/**
	 * Default attribute value.
	 */
	UPROPERTY(EditAnywhere, meta=(ShowOnlyInnerProperties))
	FVariableCollectionFloat DefaultValue = 0.0f;

	/**
	 * Minimum attribute value;
	 */
	UPROPERTY(EditAnywhere)
	FRecallAttributeBoundDefinition MinValue = 0.0f;
	
	/**
	 * Maximum attribute value;
	 */
	UPROPERTY(EditAnywhere)
	FRecallAttributeBoundDefinition MaxValue = 100.0f;
};

UCLASS()
class RECALLATTRIBUTEMODULE_API URecallAttributeSetAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	TArray<FGameplayTag> GetAttributeTags() const;
	const FRecallAttributeDefinition& GetAttributeChecked(const FGameplayTag& Attribute) const;
	TArray<TSubclassOf<class URecallAttributeMod>> GetDefaultModClasses() const;

	//~ Begin UObject Interface
#if WITH_EDITOR
protected:
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR
	//~ End UObject Interface
	
protected:
	UPROPERTY(EditAnywhere, meta=(GameplayTagFilter="Attribute"))
	TObjectPtr<URecallAttributeSetAsset> Parent;

	/**
	 * Define our entity attributes.
	 */
	UPROPERTY(EditAnywhere, meta=(GameplayTagFilter="Attribute"))
	TMap<FGameplayTag, FRecallAttributeDefinition> Attributes;

	/**
	 * Default attribute modifiers.
	 */
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class URecallAttributeMod>> DefaultModClasses;
	
#if WITH_EDITOR
private:
	void ValidateParent_Internal();
#endif // WITH_EDITOR
};
