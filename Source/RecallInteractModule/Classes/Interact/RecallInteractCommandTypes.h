// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "RecallInteractContextTypes.h"

#include "RecallInteractCommandTypes.generated.h"

RECALLINTERACTMODULE_API DECLARE_LOG_CATEGORY_EXTERN(LogRecallInteractCommand, Log, All);

UENUM(meta=(BitFlags, UseEnumValuesAsMaskValuesInEditor="true"))
enum class ERecallInteractCommandExecuteGroup : uint8
{
	None			= 0			UMETA(Hidden),
	
	OnBegin			= 1 << 0,
	OnTick			= 1 << 1,
	OnEnd			= 1 << 2,
	OnComplete		= 1 << 3,
};
ENUM_CLASS_FLAGS(ERecallInteractCommandExecuteGroup)

constexpr bool EnumHasAnyFlags(uint8 Flags, ERecallInteractCommandExecuteGroup Contains) { return (Flags & static_cast<uint8>(Contains)) != 0; }

/**
 * Command executed when interaction is performed
 */
USTRUCT()
struct RECALLINTERACTMODULE_API FRecallInteractCommand
{
	GENERATED_BODY()

public:
	virtual ~FRecallInteractCommand() = default;

public:
	void Execute(const FRecallInteractContext& Context, ERecallInteractCommandExecuteGroup Group) const;

protected:
#if WITH_EDITORONLY_DATA
	/**
	 * Comment for other developers.
	 * Won't be used in-game, and won't be cooked.
	 */
	UPROPERTY(EditAnywhere, meta=(MultiLine))
	FString Comment;
#endif // WITH_EDITORONLY_DATA
	
	UPROPERTY(EditAnywhere, meta=(Bitmask, BitmaskEnum="/Script/RecallInteractModule.ERecallInteractCommandExecuteGroup"))
	uint8 ExecuteGroup = static_cast<uint8>(ERecallInteractCommandExecuteGroup::OnComplete);

public:
	/**
	 * Verify that the command can be executed.
	 */
	virtual bool Validate(const FRecallInteractContext& Context) const { return true; }

	/**
	 * Verify that the command can be ended.
	 * The interaction will be blocked until this is true.
	 */
	virtual bool CanEndInteraction(const FRecallInteractContext& Context) const { return true; }
	
protected:
	/**
	 * Callback when interaction begin
	 */
	virtual void OnBegin(const FRecallInteractContext& Context) const {}
	
	/**
	 * Callback when interaction end
	 */
	virtual void OnEnd(const FRecallInteractContext& Context) const {}
	
	/**
	 * Callback when interaction is complete
	 */
	virtual void OnExecute(const FRecallInteractContext& Context) const {}
};
