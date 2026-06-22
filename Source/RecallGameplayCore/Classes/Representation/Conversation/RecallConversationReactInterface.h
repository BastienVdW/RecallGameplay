// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "UObject/Interface.h"
#include "ConversationTypes.h"

#include "RecallConversationReactInterface.generated.h"

/**
 * Interface for the representation of conversation.
 */
UINTERFACE()
class RECALLGAMEPLAYCORE_API URecallConversationReactInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class RECALLGAMEPLAYCORE_API IRecallConversationReactInterface
{
	GENERATED_IINTERFACE_BODY()

public:
	virtual void OnConversationUpdatedEvent(const FClientConversationMessagePayload& Message, int32 PlayerIndex = 0) {}
	virtual void OnConversationStatusChangedEvent(bool bStarted, int32 PlayerIndex = 0) {}
};
