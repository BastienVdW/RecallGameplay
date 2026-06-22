// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "System/Device/RecallDeviceSubsystem.h"

#include "Data/Device/RecallDeviceAsset.h"
#include "Engine/AssetManager.h"
#include "UObject/CoreRedirects.h"

void URecallDeviceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	if (UAssetManager* AssetManager = UAssetManager::GetIfInitialized())
	{
		TArray<FSoftObjectPath> AssetPaths;
		AssetManager->GetPrimaryAssetPathList(URecallDeviceAsset::AssetType, AssetPaths);
		
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
				DeviceAssetPtrs.Emplace(AssetPath);
			}
		}
		
		DeviceAssetMap.Empty(DeviceAssetPtrs.Num());
		DeviceAssetPtrMap.Empty(DeviceAssetPtrs.Num());
	
		for (const TSoftObjectPtr<URecallDeviceAsset>& DeviceAssetPtr : DeviceAssetPtrs)
		{
			URecallDeviceAsset* DeviceAsset = DeviceAssetPtr.LoadSynchronous();
			if (!ensureAlwaysMsgf(IsValid(DeviceAsset), TEXT("Invalid asset")))
			{
				continue;
			}

			DeviceAssetMap.Add(DeviceAsset->DeviceTag.GetTagName(), DeviceAsset);
			DeviceAssetPtrMap.Add(DeviceAsset->DeviceTag.GetTagName(), DeviceAssetPtr);
		}
	}
}

void URecallDeviceSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

TObjectPtr<URecallDeviceAsset> URecallDeviceSubsystem::GetDeviceAsset(const FGameplayTag& DeviceTag) const
{
	return GetDeviceAssetByTagName(DeviceTag.GetTagName());
}

TObjectPtr<URecallDeviceAsset> URecallDeviceSubsystem::GetDeviceAssetByTagName(
	const FName& DeviceTagName) const
{
	if (const TObjectPtr<URecallDeviceAsset>* DeviceAssetPtr = DeviceAssetMap.Find(DeviceTagName))
	{
		return *DeviceAssetPtr;
	}
	return nullptr;
}

TArray<TObjectPtr<URecallDeviceAsset>> URecallDeviceSubsystem::GetDeviceAssetsByParent(
	const FGameplayTag& DeviceParentTag) const
{
	const FGameplayTagContainer DeviceTagContainer = UGameplayTagsManager::Get().RequestGameplayTagChildren(DeviceParentTag);

	TArray<FGameplayTag> DeviceTags;
	DeviceTagContainer.GetGameplayTagArray(DeviceTags);

	TArray<TObjectPtr<URecallDeviceAsset>> Results;

	for (const FGameplayTag& DeviceTag : DeviceTags)
	{
		if (const TObjectPtr<URecallDeviceAsset> DeviceAsset = GetDeviceAsset(DeviceTag))
		{
			Results.Add(DeviceAsset);
		}
	}

	return Results;
}

TSoftObjectPtr<URecallDeviceAsset> URecallDeviceSubsystem::BP_GetDeviceAsset(FGameplayTag DeviceTag) const
{
	return DeviceAssetPtrMap.FindRef(DeviceTag.GetTagName());
}

const URecallDeviceSubsystem& URecallDeviceSubsystem::GetRef(const UWorld* World)
{
	check(World);
	const UGameInstance* GameInstance = World->GetGameInstance();
	check(GameInstance);
	const URecallDeviceSubsystem* DeviceSubsystem = GameInstance->GetSubsystem<URecallDeviceSubsystem>();
	check(DeviceSubsystem);
	return *DeviceSubsystem;
}
