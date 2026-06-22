// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "Utility/Interact/RecallInteractUtils.h"

#include "Interact/RecallInteractCommandTypes.h"
#include "Interact/RecallInteractConditionTypes.h"
#include "MassEntityView.h"
#include "MassExecutionContext.h"
#include "RecallSignalSubsystem.h"
#include "Simulation/Attribute/RecallAttributeFragments.h"
#include "Simulation/GameplayTag/RecallGameplayTagFragments.h"
#include "Simulation/Interact/RecallInteractFragments.h"
#include "Simulation/Interact/RecallInteractSignalTypes.h"
#include "Utility/Simulation/RecallSimulationUtils.h"

namespace Recall::Interact::Utils
{
	
static bool ValidateEventExecutableCommands(const FRecallInteractionEvent& Event,
	const FRecallInteractContext& InteractContext)
{
	int32 InvalidExecutableCommandCount = 0;
	for (const FInstancedStruct& ExecuteCommand : Event.ExecuteCommands)
	{
		const FRecallInteractCommand* InteractCommandPtr = ExecuteCommand.GetPtr<FRecallInteractCommand>();
		if (InteractCommandPtr != nullptr && !InteractCommandPtr->Validate(InteractContext))
		{
			InvalidExecutableCommandCount++;
		}
	}

	return InvalidExecutableCommandCount == 0;
}

static bool TestEventConditions(const FRecallInteractionEvent& Event,
	const FRecallInteractContext& InteractContext, FText* OutText = nullptr)
{
	for (const FInstancedStruct& Condition : Event.Conditions)
	{
		const FRecallInteractCondition* ConditionPtr = Condition.GetPtr<FRecallInteractCondition>();

		FText FailedText;
		if (ConditionPtr != nullptr && !ConditionPtr->EvaluateCondition(InteractContext, FailedText))
		{
			if (OutText != nullptr && !FailedText.IsEmpty())
			{
				(*OutText) = FailedText;
			}				
			return false;
		}
	}

	return true;
}

int32 FindInteractEventIndexByInput(const FRecallInteractExecuteContext& Context,
	const FMassEntityHandle& InteractionTarget, ERecallInteractInput Input,
	bool bIsContextual, FText* OutText, TObjectPtr<UInputAction>* OutUIAction)
{
	const FMassEntityManager& EntityManager = Context.ExecutionContext.GetEntityManagerChecked();
	if (!EntityManager.IsEntityValid(InteractionTarget))
	{
		return INDEX_NONE;
	}
	
	if (!EntityManager.IsEntityValid(Context.InteractorEntity))
	{
		return INDEX_NONE;
	}
	
	const FMassEntityView InteractorView(EntityManager, Context.InteractorEntity);
	const FRecallGameplayTagFragment* GameplayTagFragmentPtr = InteractorView.GetFragmentDataPtr<FRecallGameplayTagFragment>();
	if (GameplayTagFragmentPtr != nullptr && GameplayTagFragmentPtr->GameplayTagCountMap.HasTag(State_BlockInteraction))
	{
		return INDEX_NONE;
	}
	
	const FMassEntityView InteractionTargetView(EntityManager, InteractionTarget);	
	const FRecallInteractableFragment* InteractableFragmentPtr = InteractionTargetView.GetFragmentDataPtr<FRecallInteractableFragment>();
	if (InteractableFragmentPtr == nullptr)
	{
		return INDEX_NONE;
	}

	const FRecallInteractContext InteractContext{
		Context.ExecutionContext,
		Context.InteractorEntity,
		InteractionTarget
	};
	
	const auto* InteractableConstSharedFragmentPtr = InteractionTargetView.GetConstSharedFragmentDataPtr<FRecallInteractableConstSharedFragment>();

	// Look for an event that be executed using flattened hierarchy access
	const int32 TotalEventCount = InteractableFragmentPtr->GetInteractEventCount(InteractableConstSharedFragmentPtr);
	for (int32 EventIndex = 0; EventIndex < TotalEventCount; ++EventIndex)
	{
		const FRecallInteractionEvent& Event = InteractableFragmentPtr->GetInteractEventChecked(EventIndex, InteractableConstSharedFragmentPtr);
		if (bIsContextual != Event.bContextualAction)
		{
			continue;
		}
		
		if (Input != ERecallInteractInput::Any && Event.Input != Input)
		{
			continue;
		}

		// Validate executable commands, if one is invalid then the interaction fails right away
		if (!ValidateEventExecutableCommands(Event, InteractContext))
		{
			continue;
		}
		
		// Test event conditions to check if it can be executed
		if (TestEventConditions(Event, InteractContext, OutText))
		{
			if (OutText != nullptr)
			{
				(*OutText) = Event.Text;
			}
			if (OutUIAction != nullptr && Event.bContextualAction)
			{
				(*OutUIAction) = Event.ContextualInputAction;
			}
			return EventIndex;
		}
	}

	return INDEX_NONE;
}

/**
 * Execute interact command by execute group.
 */
static void ExecuteInteractionCommand(const FRecallInteractExecuteContext& Context,
	ERecallInteractCommandExecuteGroup ExecuteGroup, float DeltaTime = 1.0f)
{
	check(Context.InteractorFragmentPtr != nullptr);
	
	const FMassEntityManager& EntityManager = Context.ExecutionContext.GetEntityManagerChecked();
	const FMassEntityView CurrentInteractView(EntityManager, Context.InteractorFragmentPtr->CurrentInteractEntity);

	const auto* InteractableConstSharedFragmentPtr = CurrentInteractView.GetConstSharedFragmentDataPtr<FRecallInteractableConstSharedFragment>();
	const FRecallInteractableFragment& InteractableFragment = CurrentInteractView.GetFragmentData<FRecallInteractableFragment>();

	// Prevent multiple execution if bExecuteOnce is set
	const FRecallInteractContext InteractContext{
		Context.ExecutionContext,
		Context.InteractorEntity,
		Context.InteractorFragmentPtr->CurrentInteractEntity,
		Context.SignalSystemPtr,
		DeltaTime,
	};

	const FRecallInteractionEvent& InteractionEvent = InteractableFragment.GetInteractEventChecked(Context.InteractorFragmentPtr->InteractEventIndex, InteractableConstSharedFragmentPtr);
	const TArray<FInstancedStruct>& ExecuteCommands = InteractionEvent.ExecuteCommands;
	
	for (const FInstancedStruct& OnInteractCommand : ExecuteCommands)
	{
		if (const FRecallInteractCommand* InteractCommandPtr = OnInteractCommand.GetPtr<FRecallInteractCommand>())
		{
			InteractCommandPtr->Execute(InteractContext, ExecuteGroup);
		}
	}
}
	
/**
 * Return true if we can safely leave the interaction.
 */
static bool CanEndInteraction(const FRecallInteractExecuteContext& Context)
{
	check(Context.InteractorFragmentPtr != nullptr);
	
	const FMassEntityManager& EntityManager = Context.ExecutionContext.GetEntityManagerChecked();
	const FMassEntityView CurrentInteractView(EntityManager, Context.InteractorFragmentPtr->CurrentInteractEntity);

	const auto* InteractableConstSharedFragmentPtr = CurrentInteractView.GetConstSharedFragmentDataPtr<FRecallInteractableConstSharedFragment>();
	const FRecallInteractableFragment& InteractableFragment = CurrentInteractView.GetFragmentData<FRecallInteractableFragment>();

	// Prevent multiple execution if bExecuteOnce is set
	const FRecallInteractContext InteractContext{
		Context.ExecutionContext,
		Context.InteractorEntity,
		Context.InteractorFragmentPtr->CurrentInteractEntity
	};

	const FRecallInteractionEvent& InteractionEvent = InteractableFragment.GetInteractEventChecked(Context.InteractorFragmentPtr->InteractEventIndex, InteractableConstSharedFragmentPtr);
	const TArray<FInstancedStruct>& ExecuteCommands = InteractionEvent.ExecuteCommands;

	bool bCanEndInteraction = true;
	
	for (const FInstancedStruct& OnInteractCommand : ExecuteCommands)
	{
		if (const FRecallInteractCommand* InteractCommandPtr = OnInteractCommand.GetPtr<FRecallInteractCommand>())
		{
			bCanEndInteraction &= InteractCommandPtr->CanEndInteraction(InteractContext);
		}
	}

	return bCanEndInteraction;
}

/**
 * Check if an interaction event is on cooldown.
 * @param InteractableFragment The interactable fragment containing event data
 * @param InteractableConstSharedFragmentPtr The const shared fragment for interaction parameters
 * @param EventIndex The index of the interaction event to check
 * @param World The world context for getting current time
 * @return true if the interaction is on cooldown and should be blocked, false if it can proceed
 */
static bool IsInteractionOnCooldown(const FRecallInteractableFragment* InteractableFragment,
	const FRecallInteractableConstSharedFragment* InteractableConstSharedFragmentPtr,
	int32 EventIndex, const UWorld* World)
{
	const FRecallInteractionEvent& InteractionEvent = InteractableFragment->GetInteractEventChecked(EventIndex, InteractableConstSharedFragmentPtr);
	
	if (InteractionEvent.CooldownSeconds <= 0.0f)
	{
		return false; // No cooldown configured
	}
	
	const FRecallInteractionEventData& EventData = InteractableFragment->GetInteractionEventDataChecked(EventIndex, InteractableConstSharedFragmentPtr);
	const double CurrentTime = Recall::Simulation::Utils::GetTimeSeconds(World);
	const double TimeSinceLastExecution = CurrentTime - EventData.LastExecutionTimeSeconds;
	
	return TimeSinceLastExecution < InteractionEvent.CooldownSeconds;
}
	
bool BeginInteract(const FRecallInteractExecuteContext& Context, const FMassEntityHandle& InteractionTarget, int32 EventIndex)
{
	if (!ensureAlwaysMsgf(Context.InteractorFragmentPtr != nullptr
			&& !Context.InteractorFragmentPtr->CurrentInteractEntity.IsSet(),
				TEXT("%hs Is already interacting"), __FUNCTION__))
	{
		return false;
	}
	
	const FMassEntityManager& EntityManager = Context.ExecutionContext.GetEntityManagerChecked();
	if (!ensureAlwaysMsgf(EntityManager.IsEntityValid(InteractionTarget),
			TEXT("%hs Invalid interaction target"), __FUNCTION__))
	{
		return false;
	}
	
	const FMassEntityView CurrentInteractView(EntityManager, InteractionTarget);	
	FRecallInteractableFragment* InteractableFragmentPtr = CurrentInteractView.GetFragmentDataPtr<FRecallInteractableFragment>();
	if (!ensureAlwaysMsgf(InteractableFragmentPtr != nullptr,
			TEXT("%hs Can only interact with interactable"), __FUNCTION__))
	{
		return false;
	}
	
	// Prevent entering a new interaction the same frame that we left the previous one
	const uint32 Frame = Recall::Simulation::Utils::GetFrame(Context.ExecutionContext.GetWorld());
	if (Context.InteractorFragmentPtr->LastInteractionFrame == Frame)
	{
		return false;
	}
	
	// Check cooldown for this specific interaction event
	const auto* InteractableConstSharedFragmentPtr = CurrentInteractView.GetConstSharedFragmentDataPtr<FRecallInteractableConstSharedFragment>();
	if (IsInteractionOnCooldown(InteractableFragmentPtr, InteractableConstSharedFragmentPtr, EventIndex, Context.ExecutionContext.GetWorld()))
	{
		return false; // Still on cooldown
	}
	
	if (Context.InteractorGameplayTagFragmentPtr != nullptr &&
		!ensureAlwaysMsgf(!Context.InteractorGameplayTagFragmentPtr->GameplayTagCountMap.HasTag(State_Interacting),
			TEXT("%hs Already interacting"), __FUNCTION__))
	{
		return false;
	}
	
	Context.InteractorFragmentPtr->CurrentInteractEntity = InteractionTarget;
	Context.InteractorFragmentPtr->InteractionProgress = 0.0f;
	Context.InteractorFragmentPtr->InteractTimeSeconds = 0.0f;
	Context.InteractorFragmentPtr->InteractEventIndex = EventIndex;
	Context.InteractorFragmentPtr->bInteractionComplete = false;

	if (Context.InteractorGameplayTagFragmentPtr != nullptr)
	{
		Context.InteractorGameplayTagFragmentPtr->GameplayTagCountMap.AddTag(State_Interacting);
	}
	
	ExecuteInteractionCommand(Context, ERecallInteractCommandExecuteGroup::OnBegin);
		
	InteractableFragmentPtr->InProgressInstigators.Add(Context.InteractorEntity);

	if (Context.SignalSystemPtr != nullptr)
	{
		Context.SignalSystemPtr->SignalEntity(Signals::Begin, InteractionTarget);
	}

	return true;
}

void EndInteract(const FRecallInteractExecuteContext& Context)
{
	check(Context.InteractorFragmentPtr != nullptr);

	if (Context.InteractorGameplayTagFragmentPtr != nullptr &&
		!Context.InteractorGameplayTagFragmentPtr->GameplayTagCountMap.HasTag(State_Interacting))
	{
		return;
	}
	
	const uint32 Frame = Recall::Simulation::Utils::GetFrame(Context.ExecutionContext.GetWorld());
	
	const FMassEntityManager& EntityManager = Context.ExecutionContext.GetEntityManagerChecked();
	if (EntityManager.IsEntityValid(Context.InteractorFragmentPtr->CurrentInteractEntity))
	{
		const FMassEntityView CurrentInteractView(EntityManager, Context.InteractorFragmentPtr->CurrentInteractEntity);
		const auto* InteractableConstSharedFragmentPtr = CurrentInteractView.GetConstSharedFragmentDataPtr<FRecallInteractableConstSharedFragment>();
		FRecallInteractableFragment& InteractableFragment = CurrentInteractView.GetFragmentData<FRecallInteractableFragment>();

		ExecuteInteractionCommand(Context, ERecallInteractCommandExecuteGroup::OnEnd);

		// Unregister interactor
		InteractableFragment.InProgressInstigators.Remove(Context.InteractorEntity);

		if (Context.SignalSystemPtr != nullptr)
		{
			Context.SignalSystemPtr->SignalEntity(Signals::End, Context.InteractorFragmentPtr->CurrentInteractEntity);
		}
	}
	
	if (Context.InteractorGameplayTagFragmentPtr != nullptr)
	{
		Context.InteractorGameplayTagFragmentPtr->GameplayTagCountMap.RemoveTag(State_Interacting);
	}
	
	Context.InteractorFragmentPtr->CurrentInteractEntity.Reset();
	Context.InteractorFragmentPtr->InteractEventIndex = INDEX_NONE;
	Context.InteractorFragmentPtr->LastInteractionFrame = Frame;
}
	
static float GetInteractionProgress(
	const FRecallInteractorFragment& InteractorFragment,
	const FRecallInteractableFragment& InteractableFragment,
	const FRecallInteractableConstSharedFragment* InteractableConstSharedFragmentPtr)
{
	const FRecallInteractionEvent& Event = InteractableFragment.GetInteractEventChecked(InteractorFragment.InteractEventIndex, InteractableConstSharedFragmentPtr);
	
	if (Event.bRunForever)
	{
		return 0.0f;
	}
	
	auto GetInteractionTimeSeconds = [&]()
	{
		if (Event.bPersistentProgress)
		{
			return InteractableFragment.InteractionEventData.IsValidIndex(InteractorFragment.InteractEventIndex) ?
				InteractableFragment.InteractionEventData[InteractorFragment.InteractEventIndex].PersistentProgressTimeSeconds : 0.0f;
		}
		else
		{
			return InteractorFragment.InteractTimeSeconds;
		}
	};
	
	if (Event.Duration > 0.0f)
	{
		const double InteractionTimeSeconds = GetInteractionTimeSeconds();
		return FMath::Clamp(InteractionTimeSeconds / Event.Duration, 0.0f, 1.0f);
	}
	else
	{
		return 1.0f;
	}
}
	
static float GetInteractSpeedModifier(const FRecallInteractExecuteContext& Context)
{
	if (Context.InteractorAttributeFragmentPtr != nullptr)
	{
		return Context.InteractorAttributeFragmentPtr->AttributeSet.GetValue(Attribute_InteractSpeedModifier, 1.0f);
	}
	return 1.0f;
}
	
ERecallInteractResult UpdateInteract(const FRecallInteractExecuteContext& Context, ERecallInteractInput Input,
	float DeltaTime, bool& bOutCanEndInteraction, bool bIsContextual)
{
	const FMassEntityManager& EntityManager = Context.ExecutionContext.GetEntityManagerChecked();

	bOutCanEndInteraction = true;
	
	// Current interaction is stopped
	if (Context.InteractorFragmentPtr == nullptr ||
		!EntityManager.IsEntityValid(Context.InteractorFragmentPtr->CurrentInteractEntity))
	{
		return ERecallInteractResult::Failed;
	}

	// Fail the action if it can not be done anymore
	const int32 InteractEventIndex = FindInteractEventIndexByInput(Context,
		Context.InteractorFragmentPtr->CurrentInteractEntity, Input, bIsContextual);
	if (InteractEventIndex != Context.InteractorFragmentPtr->InteractEventIndex)
	{
		return ERecallInteractResult::Failed;
	}

	const FMassEntityView CurrentInteractView(EntityManager,Context.InteractorFragmentPtr->CurrentInteractEntity);
	const auto* InteractableConstSharedFragmentPtr = CurrentInteractView.GetConstSharedFragmentDataPtr<FRecallInteractableConstSharedFragment>();
	FRecallInteractableFragment& InteractableFragment = CurrentInteractView.GetFragmentData<FRecallInteractableFragment>();
	
	const FRecallInteractionEvent& Event = InteractableFragment.GetInteractEventChecked(InteractEventIndex, InteractableConstSharedFragmentPtr);
	FRecallInteractionEventData& EventData = InteractableFragment.GetMutableInteractionEventDataChecked(
		InteractEventIndex, InteractableConstSharedFragmentPtr);

	Context.InteractorFragmentPtr->InteractionProgress = GetInteractionProgress(*Context.InteractorFragmentPtr,
		InteractableFragment, InteractableConstSharedFragmentPtr);
	
	DeltaTime *= GetInteractSpeedModifier(Context);

	ExecuteInteractionCommand(Context, ERecallInteractCommandExecuteGroup::OnTick, DeltaTime);

	// Interaction is done
	if (Context.InteractorFragmentPtr->InteractionProgress == 1.0f)
	{		
		// Increment InteractionCount and signal interaction execution.
		if (!Context.InteractorFragmentPtr->bInteractionComplete && (!Event.bExecuteOnce || EventData.InteractionCount == 0))
		{			
			ExecuteInteractionCommand(Context, ERecallInteractCommandExecuteGroup::OnComplete);
			
			EventData.InteractionCount++;
			
			// Update timestamp for cooldown tracking
			EventData.LastExecutionTimeSeconds = Recall::Simulation::Utils::GetTimeSeconds(Context.ExecutionContext.GetWorld());
		
			InteractableFragment.ExecuteInstigators.Empty(1);
			InteractableFragment.ExecuteInstigators.Add(Context.InteractorEntity);
				
			if (Context.SignalSystemPtr != nullptr)
			{
				Context.SignalSystemPtr->SignalEntity(
					Signals::Execute, Context.InteractorFragmentPtr->CurrentInteractEntity);
			}
		}
		
		Context.InteractorFragmentPtr->bInteractionComplete = true;

		bOutCanEndInteraction = CanEndInteraction(Context);
		if (bOutCanEndInteraction)
		{
			return ERecallInteractResult::Succeeded;
		}
		else
		{
			return ERecallInteractResult::Running;
		}
	}
	else
	{
		EventData.PersistentProgressTimeSeconds += DeltaTime;
		Context.InteractorFragmentPtr->InteractTimeSeconds += DeltaTime;

		const float ProgressTickRate = Event.ProgressTickRate;
		if (ProgressTickRate > 0.0f)
		{
			const int32 NewTickCount = FMath::FloorToInt(EventData.PersistentProgressTimeSeconds / ProgressTickRate);
			if (NewTickCount > EventData.PersistentProgressTickCount)
			{
				if (Context.SignalSystemPtr != nullptr)
				{
					Context.SignalSystemPtr->DelaySignalEntity(
						Signals::InProgressTick, Context.InteractorFragmentPtr->CurrentInteractEntity, ProgressTickRate);
				}
				EventData.PersistentProgressTickCount++;
			}
		}	

		bOutCanEndInteraction = CanEndInteraction(Context);
		return ERecallInteractResult::Running;
	}
}
	
} // namespace Recall::Interact::Utils
