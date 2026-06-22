// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "StructUtils/InstancedStruct.h"

#include "RecallInteractTypes.generated.h"

RECALLGAMEPLAYCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Interacting);
RECALLGAMEPLAYCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_BlockInteraction);

RECALLGAMEPLAYCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_InteractSpeedModifier);

RECALLGAMEPLAYCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(StateTreeEvent_Interact_Contextual_Primary);
RECALLGAMEPLAYCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(StateTreeEvent_Interact_Contextual_Secondary);
RECALLGAMEPLAYCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(StateTreeEvent_Interact_Contextual_Tertiary);
RECALLGAMEPLAYCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(StateTreeEvent_Interact_Contextual_Quaternary);

namespace Recall::Input::Option
{

	static const FString InteractContextualPrimary		= "InteractContextualPrimary";
	static const FString InteractContextualSecondary	= "InteractContextualSecondary";
	static const FString InteractContextualTertiary		= "InteractContextualTertiary";
	static const FString InteractContextualQuaternary	= "InteractContextualQuaternary";
	
} // namespace Recall::Input::Option

/**
 * Inputs that can be used to trigger an interaction.
 * This allows an entity to have multiple active interactions.
 */
UENUM(BlueprintType)
enum class ERecallInteractInput : uint8
{
	Primary,
	Secondary,
	Tertiary,
	Quaternary,

	MAX				UMETA(Hidden),

	Any				UMETA(Hidden),
};
#define RECALL_INTERACT_INPUT_COUNT static_cast<int32>(ERecallInteractInput::MAX)
	
USTRUCT()
struct RECALLGAMEPLAYCORE_API FRecallInteractionEvent
{
	GENERATED_BODY()

#if WITH_EDITORONLY_DATA
protected:
	/**
	 * A friendly name that is only used in the editor.
	 */
	UPROPERTY(EditAnywhere)
	FName FriendlyName = NAME_None;
	
	/**
	 * Comment for other developers.
	 * Won't be used in-game, and won't be cooked.
	 */
	UPROPERTY(EditAnywhere, meta=(MultiLine))
	FString Comment;
#endif // WITH_EDITORONLY_DATA

public:
	// Input & Display
	/**
	 * Input used to trigger this event.
	 */
	UPROPERTY(EditAnywhere, Category="Input")
	ERecallInteractInput Input = ERecallInteractInput::Primary;

	/**
	 * Handle this interaction as a contextual action.
	 * Contextual actions are displayed in the action bar directly.
	 */
	UPROPERTY(EditAnywhere, Category="Input")
	bool bContextualAction = false;

	/**
	 * Contextual UI action to display in the action bar.
	 */
	UPROPERTY(EditAnywhere, Category="Input", meta=(EditCondition="bContextualAction", EditConditionHides))
	TObjectPtr<class UInputAction> ContextualInputAction;
	
	/**
	 * Text to display for this interaction event.
	 */
	UPROPERTY(EditAnywhere, Category="Input", meta=(EditCondition="!bContextualAction", EditConditionHides))
	FText Text;
	
	/**
	 * Events with higher priority will be displayed first.
	 */
	UPROPERTY(EditAnywhere, Category="Input")
	int32 EventPriority = 0;

	// Timing & Progress
	/**
	 * If true the interaction will run forever until something stops it.
	 */
	UPROPERTY(EditAnywhere, Category="Timing")
	bool bRunForever = false;
	
	/**
	 * How long the interact button must be held to interact with.
	 */
	UPROPERTY(EditAnywhere, Category="Timing", meta=(Units="Seconds", ClampMin=0.0, EditCondition="!bRunForever"))
	float Duration = 0.0f;

	/**
	 * Interaction progress is shared and persistent.
	 */
	UPROPERTY(EditAnywhere, Category="Timing", meta=(EditCondition="!bRunForever"))
	bool bPersistentProgress = false;

	/**
	 * How frequently to update the interaction.
	 */
	UPROPERTY(EditAnywhere, Category="Timing", meta=(Units="Seconds", ClampMin=0.0))
	float ProgressTickRate = 0.1f;

	// Execution
	/**
	 * Only trigger interact commands one time.
	 */
	UPROPERTY(EditAnywhere, Category="Execution")
	bool bExecuteOnce = false;

	/**
	 * Cooldown time before this event can be triggered again.
	 * Useful for press-only actions to prevent spam while input is held.
	 */
	UPROPERTY(EditAnywhere, Category="Execution", meta=(Units="Seconds", ClampMin=0.0, ToolTip="Cooldown time before this event can be triggered again. Useful for press-only actions to prevent spam."))
	float CooldownSeconds = 0.0f;

	/**
	 * Conditions to trigger this event.
	 */
	UPROPERTY(EditAnywhere, Category="Execution", meta=(BaseStruct="/Script/RecallInteractModule.RecallInteractCondition", ExcludeBaseStruct))
	TArray<FInstancedStruct> Conditions;
	
	/**
	 * Commands to execute on interact.
	 */
	UPROPERTY(EditAnywhere, Category="Execution", meta=(BaseStruct="/Script/RecallInteractModule.RecallInteractCommand", ExcludeBaseStruct))
	TArray<FInstancedStruct> ExecuteCommands;

	// Distance & Positioning
	/**
	 * Maximum distance at which this interaction can be triggered.
	 * Set to 0.0 for unlimited range (default behavior).
	 */
	UPROPERTY(EditAnywhere, Category="Distance", meta=(Units="Centimeters", ClampMin=0.0))
	float MaxInteractionRange = 0.0f;
	
	/**
	 * Use projected location on the interactable's bounding box instead of center position for distance calculation and display.
	 * Useful for large interactables where interaction should appear near the closest surface rather than the center.
	 */
	UPROPERTY(EditAnywhere, Category="Distance")
	bool bUseBoundingBoxProjection = false;
};

USTRUCT()
struct RECALLGAMEPLAYCORE_API FRecallInteractionParameters
{
	GENERATED_BODY()

	FRecallInteractionParameters();

	UPROPERTY(EditAnywhere, meta=(TitleProperty="{FriendlyName}"))
	TArray<FRecallInteractionEvent> Events;
	
	UPROPERTY(EditAnywhere, Category=Icon, DisplayName="Socket Name")
	FName IconSocketName = NAME_None;

	bool HasEvent(int32 Index) const;
	int32 NumEvent() const { return Events.Num(); }
	FRecallInteractionEvent& GetMutableEventChecked(int32 Index);
	const FRecallInteractionEvent& GetEventChecked(int32 Index) const;
};

/**
 * Representation of an interaction's progress.
 */
USTRUCT(BlueprintType)
struct RECALLGAMEPLAYCORE_API FRecallInteractProgress
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ID = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector Location = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Progress = 0.0f;
};
