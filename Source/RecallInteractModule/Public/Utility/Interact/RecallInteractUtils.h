// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "Data/Interact/RecallInteractTypes.h"

class URecallSignalSubsystem;
struct FMassEntityHandle;
struct FMassExecutionContext;
struct FRecallInteractorFragment;
class UInputAction;

struct FRecallInteractExecuteContext
{
	FMassExecutionContext& ExecutionContext;
	const FMassEntityHandle& InteractorEntity;
	FRecallInteractorFragment* const InteractorFragmentPtr = nullptr;
	struct FRecallGameplayTagFragment* const InteractorGameplayTagFragmentPtr = nullptr;
	struct FRecallAttributeFragment* const InteractorAttributeFragmentPtr = nullptr;
	URecallSignalSubsystem* const SignalSystemPtr = nullptr;
};

enum ERecallInteractResult
{
	Running,
	Failed,
	Succeeded,
};

namespace Recall::Interact::Utils
{
	
/// Find first interaction event that pass all the conditions.
/// @param Context Interaction context
/// @param InteractionTarget The target entity, return INDEX_NONE if not interactable
/// @param Input Interact input to evaluate
/// @param OutText Text to display for the interaction, failed text if return value is INDEX_NONE
/// @return The index of the interaction event that passed all the conditions, INDEX_NONE if none or any condition failed
RECALLINTERACTMODULE_API extern int32 FindInteractEventIndexByInput(const FRecallInteractExecuteContext& Context,
	const FMassEntityHandle& InteractionTarget, ERecallInteractInput Input = ERecallInteractInput::Any,
	bool bIsContextual = false, FText* OutText = nullptr, TObjectPtr<UInputAction>* OutUIAction = nullptr);
	
RECALLINTERACTMODULE_API extern bool BeginInteract(const FRecallInteractExecuteContext& Context,
	const FMassEntityHandle& InteractionTarget, int32 EventIndex = 0);
RECALLINTERACTMODULE_API extern void EndInteract(const FRecallInteractExecuteContext& Context);
RECALLINTERACTMODULE_API extern ERecallInteractResult UpdateInteract(const FRecallInteractExecuteContext& Context,
	ERecallInteractInput Input, float DeltaTime, bool& bOutCanEndInteraction, bool bIsContextual = false);
	
} // namespace Recall::Interact::Utils
