// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Subsystems/GameInstanceSubsystem.h"

#include "RecallItemSubsystem.generated.h"

class URecallInventoryItemAsset;

UCLASS()
class RECALLINVENTORYMODULE_API URecallItemSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
	static const URecallItemSubsystem& GetRef(const UWorld* World);

public:
	TObjectPtr<const URecallInventoryItemAsset> GetItemAsset(const FGameplayTag& ItemTag) const;

protected:
	UFUNCTION(BlueprintPure, Category="Inventory", meta=(BlueprintThreadSafe, DisplayName="Get Item Asset"))
	TSoftObjectPtr<const URecallInventoryItemAsset> BP_GetItemAsset(FGameplayTag ItemTag) const;
	
	// USubsystem implementation Begin
protected:
	void Initialize(FSubsystemCollectionBase& Collection) override final;
	void Deinitialize() override final;
	// USubsystem implementation End
    
private:
    UPROPERTY(Transient)
	TArray<TSoftObjectPtr<URecallInventoryItemAsset>> ItemAssetPtrs;
	UPROPERTY(Transient)
	TMap<FName, TSoftObjectPtr<const URecallInventoryItemAsset>> ItemAssetPtrMap;
    UPROPERTY(Transient)
    TMap<FName, TObjectPtr<const URecallInventoryItemAsset>> ItemAssetMap;
};
