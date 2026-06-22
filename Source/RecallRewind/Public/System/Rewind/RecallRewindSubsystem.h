// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "System/Interface/RecallSimulationReactSystemInterface.h"
#include "MassExtendedExternalSubsystemTraits.h"
#include "System/Snapshot/RecallSnapshotTypes.h"
#include "Containers/RingBuffer.h"

#include "RecallRewindSubsystem.generated.h"

#define RECALL_REWIND_MAX 512

/*
* Rewind our simulation instead of stepping it
*/
UCLASS(config=RecallGameplay, defaultconfig)
class RECALLREWIND_API URecallRewindSubsystem : 
	public UWorldSubsystem,
	public IRecallSimulationReactSystemInterface
{
	GENERATED_BODY()

public:
	void Rewind();

protected:
	// UWorldSubsystem implementation Begin
	void Initialize(FSubsystemCollectionBase& Collection) override final;
	void Deinitialize() override final;
	// UWorldSubsystem implementation End

	// IRecallSimulationReactSystemInterface implementation Begin
	void Reset() override final;
	void Save(const FRecallSnapshotContext& Context, FInstancedStruct& OutSnapshot) override final;
	void Restore(const FRecallSnapshotContext& Context, const FInstancedStruct& InSnapshot) override final;
	// IRecallSimulationReactSystemInterface implementation End

protected:
	UPROPERTY(EditAnywhere, config)
	bool bEnableRewind = true;

	UPROPERTY(EditAnywhere, config, meta=(Units="Seconds"))
	float RewindMaxDuration = 10.0f;

	UPROPERTY(EditAnywhere, config)
	float RewindSpeed = 5.0f;

	UPROPERTY(EditAnywhere, config)
	bool bSaveSnapshot = false;

	UPROPERTY(EditAnywhere)
	FName RewindReason = TEXT("Rewind");

	UFUNCTION()
	void OnFrameEnd(uint32 Frame);

private:
	UPROPERTY(Transient)
	TWeakObjectPtr<class URecallSnapshotSubsystem> SnapshotSystem;

	TRingBuffer<TSharedPtr<FRecallSimulationSnapshot>, TFixedAllocator<RECALL_REWIND_MAX>> Snapshots;
	bool bIsRewinding = false;

	mutable FCriticalSection DataGuard;

	int32 GetRewindFrameRate() const;
	int32 GetRewindSnapshotFrequency() const;
	int32 GetRewindMaxFrameCount() const;
};

template<>
struct TMassExtendedExternalSubsystemTraits<URecallRewindSubsystem> final
{
	enum
	{
		GameThreadOnly = false
	};
};
