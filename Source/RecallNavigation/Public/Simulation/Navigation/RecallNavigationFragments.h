// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Mass/EntityElementTypes.h"
#include "System/Navigation/RecallNavigationTypes.h"

#include "RecallNavigationFragments.generated.h"

UENUM(meta=(BitFlags, UseEnumValuesAsMaskValuesInEditor="true"))
enum class ERecallNavMeshPathRequestFlag : uint16
{
	None					= 0,

	DirtyPath				= 1 << 0,
	PathFailed				= 1 << 1,
};
ENUM_CLASS_FLAGS(ERecallNavMeshPathRequestFlag)

inline uint16& operator|=(uint16& Lhs, ERecallNavMeshPathRequestFlag Rhs) { return Lhs |= static_cast<uint16>(Rhs); }
constexpr void EnumAddFlags(uint16& Flags, ERecallNavMeshPathRequestFlag FlagsToAdd) { Flags |= static_cast<uint16>(FlagsToAdd); }
constexpr void EnumRemoveFlags(uint16& Flags, ERecallNavMeshPathRequestFlag FlagsToRemove) { Flags = Flags & ~static_cast<uint16>(FlagsToRemove); }
constexpr bool EnumHasAnyFlags(uint16 Flags, ERecallNavMeshPathRequestFlag Contains) { return (Flags & static_cast<uint16>(Contains)) != 0; }

USTRUCT()
struct RECALLNAVIGATION_API FRecallNavMeshPathRequestFragment : public FMassFragment
{
	GENERATED_BODY()

public:
	FORCEINLINE void RequestPath(const FVector& Location)
	{
		TargetLocation = Location;
		EnumAddFlags(RequestFlags, ERecallNavMeshPathRequestFlag::DirtyPath);
		EnumRemoveFlags(RequestFlags, ERecallNavMeshPathRequestFlag::PathFailed);
	}

	FORCEINLINE bool IsPathDirty() const
	{
		return EnumHasAnyFlags(RequestFlags, ERecallNavMeshPathRequestFlag::DirtyPath);
	}

	FORCEINLINE void ClearDirtyPathFlag()
	{
		EnumRemoveFlags(RequestFlags, ERecallNavMeshPathRequestFlag::DirtyPath);
	}

	FORCEINLINE bool IsWaitingPath() const
	{
		return PathHandle.IsValid();
	}

	FORCEINLINE bool IsFailedPath() const
	{
		return EnumHasAnyFlags(RequestFlags, ERecallNavMeshPathRequestFlag::PathFailed);
	}

public:
	UPROPERTY(VisibleAnywhere)
	FVector TargetLocation = FVector::ZeroVector;
	UPROPERTY(VisibleAnywhere, meta=(Bitmask, BitmaskEnum="/Script/RecallNavigation.ERecallNavMeshPathRequestFlag"))
	uint16 RequestFlags = 0;

	UPROPERTY(VisibleAnywhere)
	FRecallNavigationHandle PathHandle;

private:
};

// For backward compatibility with existing code
using FRecallNavigationAgentFragment = FRecallNavMeshPathRequestFragment;

USTRUCT()
struct RECALLNAVIGATION_API FRecallNavigationAgentConstSharedFragment : public FMassConstSharedFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	FRecallNavigationAgentSettings Agent;
};
