// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "System/Interface/RecallSimulationReactSystemInterface.h"
#include "Mass/ExternalSubsystemTraits.h"
#include "RecallCrowdAgentHandle.h"

#include "RecallCrowdSubsystem.generated.h"

class URecallCrowdManager;
struct FRecallCrowdAgentBlackboard;

UCLASS()
class RECALLNAVIGATION_API URecallCrowdSubsystem :
	public UWorldSubsystem,
	public IRecallSimulationReactSystemInterface
{
	GENERATED_BODY()

public:
	void TickCrowdManager(float DeltaSeconds);

	FRecallCrowdAgentHandle RegisterCrowdAgent();
	void ReleaseCrowdAgent(FRecallCrowdAgentHandle& Handle);

	/**
	 * set the movement direction of the crowd agent, used for navigation.
	 */
	void SetAgentMoveDirection(const FRecallCrowdAgentHandle& Handle, const FVector& MoveDirection) const;
	
	/**
	 * if set, avoidance and steering will be suspended (used for direct move requests)
	 */
	void SetSuspendCrowdAgentSimulation(const FRecallCrowdAgentHandle& Handle, bool bSuspend);

	void SetCrowdAgentSimulationState(const FRecallCrowdAgentHandle& Handle, ERecallCrowdSimulationState State);

	FRecallCrowdAgentBlackboard& GetMutableAgentBlackboardChecked(const FRecallCrowdAgentHandle& Handle);
	const FRecallCrowdAgentBlackboard& GetAgentBlackboardChecked(const FRecallCrowdAgentHandle& Handle) const;
	
protected:
	//~ Begin UObject Interface
	static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);
	//~ End UObject Interface
	
	// UWorldSubsystem implementation Begin
	virtual void Initialize(FSubsystemCollectionBase& Collection) override final;
	virtual void Deinitialize() override final;
	// UWorldSubsystem implementation End

	// IRecallSimulationReactSystemInterface implementation Begin
	virtual void Reset() override final;
	virtual void Start(const FRecallSimulationStartParams& Params) override final;
	virtual int32 GetStartOrderPriority() const override final { return Recall::SimReactSystem::StartOrder::HighPriority; }
	virtual void Save(const FRecallSnapshotContext& Context, FInstancedStruct& OutSnapshot) override final;
	virtual void Restore(const FRecallSnapshotContext& Context, const FInstancedStruct& InSnapshot) override final;
	// IRecallSimulationReactSystemInterface implementation End

protected:
	UPROPERTY(EditAnywhere, meta=(ClampMin=1))
	int32 IterationStepCount = 1;

private:
	void CreateCrowdManager();
	void SetCrowdManager(URecallCrowdManager* NewCrowdManager);

	void CreateCrowdAgent_Internal(const FRecallCrowdAgentHandle& Handle);
	void ReleaseCrowdAgent_Internal(const FRecallCrowdAgentHandle& Handle);
	void ReleaseAllCrowdAgents_Internal();
	
	uint32 CrowdAgentUniqueIdGenerator = 0;

	TArray<FRecallCrowdAgentHandle> AgentHandles;
	TMap<FRecallCrowdAgentHandle, TSharedPtr<struct FRecallCrowdAgentInternalData>> CrowdAgents;
	
	TWeakObjectPtr<URecallCrowdManager> CrowdManager;	
};

template<>
struct TMassExternalSubsystemTraits<URecallCrowdSubsystem> final
{
	enum
	{
		GameThreadOnly = false
	};
};
