// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Subsystems/GameInstanceSubsystem.h"

#include "RecallConversationParticipantSubsystem.generated.h"

class URecallConversationParticipantAsset;
struct FGameplayTag;

UCLASS()
class RECALLGAMEPLAYREPRESENTATION_API URecallConversationParticipantSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static const URecallConversationParticipantSubsystem& GetRef(const UWorld* World);

public:
	UFUNCTION(BlueprintCallable, Category="Conversation")
	TSoftObjectPtr<URecallConversationParticipantAsset> GetParticipantAssetPath(
		const FGameplayTag& ParticipantID) const;
	
	// USubsystem implementation Begin
protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override final;
	virtual void Deinitialize() override final;
	// USubsystem implementation End

private:
	UPROPERTY(Transient)
	TArray<FSoftObjectPath> ConversationParticipantAssetPaths;
	UPROPERTY(Transient)
	TMap<FGameplayTag, TSoftObjectPtr<URecallConversationParticipantAsset>> ConversationParticipantAssetMap;

	TSharedPtr<FStreamableHandle> ConversationParticipantStreamableHandle;
	
	UFUNCTION()
	void OnConversationParticipantAssetsLoaded();
};
