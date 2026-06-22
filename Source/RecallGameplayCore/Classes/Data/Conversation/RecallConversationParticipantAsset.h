// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Engine/DataAsset.h"
#include "NativeGameplayTags.h"

#include "RecallConversationParticipantAsset.generated.h"

UCLASS()
class RECALLGAMEPLAYCORE_API URecallConversationParticipantAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category=Conversation, meta=(Categories="Conversation.Participant"))
	FGameplayTag ParticipantID;

	UPROPERTY(EditAnywhere, Category=Conversation)
	FText ParticipantName;
	
	UPROPERTY(EditAnywhere, Category=Conversation)
	TSoftObjectPtr<UTexture2D> ParticipantIcon;

public:
	static const FPrimaryAssetType AssetType;

protected:	
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
	
};
