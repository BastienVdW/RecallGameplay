// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "System/Conversation/RecallConversationParticipantSubsystem.h"

#include "Data/Conversation/RecallConversationParticipantAsset.h"
#include "Engine/AssetManager.h"
#include "UObject/CoreRedirects.h"

void URecallConversationParticipantSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	if (UAssetManager* AssetManager = UAssetManager::GetIfInitialized())
	{
		TArray<FSoftObjectPath> AssetPaths;
		AssetManager->GetPrimaryAssetPathList(URecallConversationParticipantAsset::AssetType, AssetPaths);
		
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
				ConversationParticipantAssetPaths.Emplace(AssetPath);
			}
		}

		ConversationParticipantStreamableHandle = AssetManager->LoadAssetList(ConversationParticipantAssetPaths,
			FStreamableDelegate::CreateUObject(this, &ThisClass::OnConversationParticipantAssetsLoaded));
	}
}

void URecallConversationParticipantSubsystem::Deinitialize()
{
	Super::Deinitialize();

	if (ConversationParticipantStreamableHandle.IsValid())
	{
		ConversationParticipantStreamableHandle->CancelHandle();
		ConversationParticipantStreamableHandle.Reset();
	}

	ConversationParticipantAssetPaths.Empty();
	ConversationParticipantAssetMap.Empty();
}

void URecallConversationParticipantSubsystem::OnConversationParticipantAssetsLoaded()
{
	ConversationParticipantAssetMap.Reserve(ConversationParticipantAssetPaths.Num());
	
	for (const FSoftObjectPath& ParticipantAssetPath : ConversationParticipantAssetPaths)
	{
		URecallConversationParticipantAsset* ParticipantAsset = Cast<URecallConversationParticipantAsset>(
			ParticipantAssetPath.ResolveObject());
		if (!ensureAlwaysMsgf(IsValid(ParticipantAsset),
			TEXT("%hs Failed to load RecallConversationParticipantAsset: %s"),
				__FUNCTION__, *ParticipantAssetPath.ToString()))
		{
			continue;
		}

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		if (!ensureAlwaysMsgf(ParticipantAsset->ParticipantID.IsValid(),
			TEXT("%hs ParticipantID is not set: %s"), __FUNCTION__, *ParticipantAssetPath.ToString()))
		{
			continue;
		}
#endif // UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		
		ConversationParticipantAssetMap.Add(ParticipantAsset->ParticipantID,
			TSoftObjectPtr<URecallConversationParticipantAsset>(ParticipantAssetPath));
	}

	ConversationParticipantAssetPaths.Empty();
	ConversationParticipantStreamableHandle.Reset();
}

const URecallConversationParticipantSubsystem& URecallConversationParticipantSubsystem::GetRef(const UWorld* World)
{
	check(World);
	const UGameInstance* GameInstance = World->GetGameInstance();
	check(GameInstance);
	URecallConversationParticipantSubsystem* MapSubsystem = GameInstance->GetSubsystem<URecallConversationParticipantSubsystem>();
	check(MapSubsystem);
	return *MapSubsystem;
}

TSoftObjectPtr<URecallConversationParticipantAsset> URecallConversationParticipantSubsystem::GetParticipantAssetPath(
	const FGameplayTag& ParticipantID) const
{
	return ConversationParticipantAssetMap.FindRef(ParticipantID);
}
