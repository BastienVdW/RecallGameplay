// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "System/Inventory/RecallItemSubsystem.h"

#include "Data/Inventory/RecallInventoryItemAsset.h"
#include "Engine/AssetManager.h"
#include "UObject/CoreRedirects.h"

void URecallItemSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	if (UAssetManager* AssetManager = UAssetManager::GetIfInitialized())
	{
		TArray<FSoftObjectPath> AssetPaths;
		AssetManager->GetPrimaryAssetPathList(URecallInventoryItemAsset::AssetType, AssetPaths);
		
		for (const FSoftObjectPath& AssetPath : AssetPaths)
		{
			//.pak files containing the map may not be mounted. If so, ignore them.
			const FCoreRedirectObjectName RedirectedName =
				FCoreRedirects::GetRedirectedName(
					ECoreRedirectFlags::Type_Package,
					FCoreRedirectObjectName(AssetPath.GetLongPackageName()));
			
			FString LocalizedName;
			LocalizedName = FPackageName::GetDelegateResolvedPackagePath(RedirectedName.PackageName.ToString());
			LocalizedName = FPackageName::GetLocalizedPackagePath(LocalizedName);
			
			if (FPackageName::DoesPackageExist(LocalizedName))
			{
				ItemAssetPtrs.Emplace(AssetPath);
			}
		}
		
		ItemAssetMap.Empty(ItemAssetPtrs.Num());
		ItemAssetPtrMap.Empty(ItemAssetPtrs.Num());
	
		for (const TSoftObjectPtr<URecallInventoryItemAsset>& ItemAssetPtr : ItemAssetPtrs)
		{
			const URecallInventoryItemAsset* ItemAsset = ItemAssetPtr.LoadSynchronous();
			if (!ensureAlwaysMsgf(IsValid(ItemAsset), TEXT("Invalid asset")))
			{
				continue;
			}

			ItemAssetMap.Add(ItemAsset->ItemTag.GetTagName(), ItemAsset);
			ItemAssetPtrMap.Add(ItemAsset->ItemTag.GetTagName(), ItemAssetPtr);
		}
	}
}

void URecallItemSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

TObjectPtr<const URecallInventoryItemAsset> URecallItemSubsystem::GetItemAsset(const FGameplayTag& ItemTag) const
{
	if (const TObjectPtr<const URecallInventoryItemAsset>* ItemAssetPtr = ItemAssetMap.Find(ItemTag.GetTagName()))
	{
		return *ItemAssetPtr;
	}
	return nullptr;
}

TSoftObjectPtr<const URecallInventoryItemAsset> URecallItemSubsystem::BP_GetItemAsset(FGameplayTag ItemTag) const
{
	return ItemAssetPtrMap.FindRef(ItemTag.GetTagName());
}

const URecallItemSubsystem& URecallItemSubsystem::GetRef(const UWorld* World)
{
	check(World);
	const UGameInstance* GameInstance = World->GetGameInstance();
	check(GameInstance);
	const URecallItemSubsystem* ItemSubsystem = GameInstance->GetSubsystem<URecallItemSubsystem>();
	check(ItemSubsystem);
	return *ItemSubsystem;
}
