// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "ConversationSideEffectNode.h"

#include "RecallConversationSideEffect.generated.h"

/**
 * Base class for simulation friendly conversation side effect nodes.
 */
UCLASS(Abstract)
class RECALLCONVERSATIONMODULE_API URecallConversationSideEffectNode : public UConversationSideEffectNode
{
	GENERATED_BODY()

protected:
	virtual void CauseSideEffect_Implementation(const FConversationContext& Context) const;

private:
	void ServerCauseSideEffect_Implementation(const FConversationContext& Context) const override final;
	void ClientCauseSideEffect_Implementation(const FConversationContext& Context) const override final;

};
