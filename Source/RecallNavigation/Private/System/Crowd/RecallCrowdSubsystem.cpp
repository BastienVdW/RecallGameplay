// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "System/Crowd/RecallCrowdSubsystem.h"

#include "Crowd/RecallCrowdManager.h"
#include "EngineUtils.h"
#include "MassEntityUtils.h"
#include "RecallCrowdSnapshotTypes.h"
#include "Crowd/RecallCrowdAgent.h"

struct FRecallCrowdAgentInternalData
{
	TSharedPtr<FRecallCrowdAgent> Agent;
	FRecallDetourCrowdAgent DetourCrowdAgent;
	FVector MoveDirection = FVector::ZeroVector;
};

void URecallCrowdSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void URecallCrowdSubsystem::Deinitialize()
{
	Super::Deinitialize();
	
	SetCrowdManager(nullptr);
}

void URecallCrowdSubsystem::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector)
{
	Super::AddReferencedObjects(InThis, Collector);

	URecallCrowdSubsystem* This = CastChecked<URecallCrowdSubsystem>(InThis);
	Collector.AddReferencedObject(This->CrowdManager, InThis);
}

void URecallCrowdSubsystem::Reset()
{
	CrowdAgentUniqueIdGenerator = 0;
	ReleaseAllCrowdAgents_Internal();
	
	SetCrowdManager(nullptr);
}

void URecallCrowdSubsystem::Start(const FRecallSimulationStartParams& Params)
{
	CreateCrowdManager();
}

void URecallCrowdSubsystem::Save(const FRecallSnapshotContext& Context, FInstancedStruct& OutSnapshot)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_Crowd_Save);

	OutSnapshot.InitializeAs<FRecallCrowdSnapshot>();

	FRecallCrowdSnapshot& Snapshot = OutSnapshot.GetMutable<FRecallCrowdSnapshot>();
	Snapshot.CrowdAgentUniqueIdGenerator = CrowdAgentUniqueIdGenerator;

	Snapshot.CrowdAgents.SetNum(CrowdAgents.Num());
	int32 AgentIndex = 0;

	for (const TPair<FRecallCrowdAgentHandle, TSharedPtr<FRecallCrowdAgentInternalData>>& AgentTuple : CrowdAgents)
	{
		FRecallCrowdAgentSnapshot& AgentSnapshot = Snapshot.CrowdAgents[AgentIndex++];
		AgentSnapshot.AgentHandle = AgentTuple.Key;
		AgentSnapshot.AgentData = CrowdManager->GetAgentDataChecked(AgentTuple.Value->Agent.Get());
		AgentSnapshot.AgentMutableData = AgentTuple.Value->Agent->GetMutableData();
		AgentSnapshot.MoveDirection = AgentTuple.Value->MoveDirection;
		AgentSnapshot.DetourCrowdAgent.Save(CrowdManager->GetDetourCrowdAgent(AgentTuple.Value->Agent.Get()));
	}
}

void URecallCrowdSubsystem::Restore(const FRecallSnapshotContext& Context, const FInstancedStruct& InSnapshot)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_Crowd_Restore);

	if (const FRecallCrowdSnapshot* DataPtr = InSnapshot.GetPtr<FRecallCrowdSnapshot>())
	{
		CrowdAgentUniqueIdGenerator = DataPtr->CrowdAgentUniqueIdGenerator;

		// TODO: Deserialize FRecallCrowdAgentData OR/AND dtCrowd* DetourCrowd;

		TSet<FRecallCrowdAgentHandle> OldAgentHandles;
		CrowdAgents.GetKeys(OldAgentHandles);

		for (const FRecallCrowdAgentSnapshot& AgentSnapshot : DataPtr->CrowdAgents)
		{
			TSharedPtr<FRecallCrowdAgentInternalData>* AgentInternalData = CrowdAgents.Find(AgentSnapshot.AgentHandle);
			if (AgentInternalData != nullptr)
			{
				check(AgentInternalData->IsValid());
				OldAgentHandles.Remove(AgentSnapshot.AgentHandle);
			}
			else
			{
				CreateCrowdAgent_Internal(AgentSnapshot.AgentHandle);
				AgentInternalData = &CrowdAgents[AgentSnapshot.AgentHandle];
			}
			
			CrowdManager->SetAgentData(AgentInternalData->Get()->Agent.Get(), AgentSnapshot.AgentData);
			AgentInternalData->Get()->Agent->SetMutableData(AgentSnapshot.AgentMutableData);
			AgentInternalData->Get()->MoveDirection = AgentSnapshot.MoveDirection;
			AgentInternalData->Get()->DetourCrowdAgent = AgentSnapshot.DetourCrowdAgent;
		}

		for (const FRecallCrowdAgentHandle& OldAgentHandle : OldAgentHandles)
		{
			ReleaseCrowdAgent_Internal(OldAgentHandle);
		}
	}
}

void URecallCrowdSubsystem::CreateCrowdManager()
{
	UClass* CrowdManagerClassInstance = URecallCrowdManager::StaticClass();
	if (CrowdManagerClassInstance)
	{
		URecallCrowdManager* ManagerInstance = NewObject<URecallCrowdManager>(this, CrowdManagerClassInstance);
		// creating an instance when we have a valid class should never fail
		check(ManagerInstance);
		SetCrowdManager(ManagerInstance);
	}
}

void URecallCrowdSubsystem::SetCrowdManager(URecallCrowdManager* NewCrowdManager)
{
	if (NewCrowdManager == CrowdManager.Get())
	{
		return;
	}

	if (CrowdManager.IsValid())
	{
		CrowdManager->RemoveFromRoot();
	}
	CrowdManager = NewCrowdManager;
	if (NewCrowdManager != nullptr)
	{
		CrowdManager->AddToRoot();
		
		for (TActorIterator<ANavigationData> It(GetWorld()); It; ++It)
		{
			ANavigationData* NavData = (*It);
			if (NavData != nullptr)
			{
				CrowdManager->OnNavDataRegistered(*NavData);
			}
		}
	}
}

void URecallCrowdSubsystem::CreateCrowdAgent_Internal(const FRecallCrowdAgentHandle& Handle)
{
	TSharedPtr<FRecallCrowdAgentInternalData> AgentInternalData = MakeShared<FRecallCrowdAgentInternalData>();
	AgentInternalData->Agent = MakeShared<FRecallCrowdAgent>(*this);
	CrowdManager->RegisterAgent(AgentInternalData->Agent.Get());
	CrowdAgents.Add(Handle, AgentInternalData);
	AgentHandles.Add(Handle);
}

void URecallCrowdSubsystem::ReleaseCrowdAgent_Internal(const FRecallCrowdAgentHandle& Handle)
{
	TSharedPtr<FRecallCrowdAgentInternalData> AgentInternalData;
	if (CrowdAgents.RemoveAndCopyValue(Handle, AgentInternalData))
	{
		check(AgentInternalData.IsValid());
		CrowdManager->UnregisterAgent(AgentInternalData->Agent.Get());
		AgentHandles.Remove(Handle);
	}
}

void URecallCrowdSubsystem::ReleaseAllCrowdAgents_Internal()
{
	if (CrowdManager.IsValid())
	{
		for (const TTuple<FRecallCrowdAgentHandle, TSharedPtr<FRecallCrowdAgentInternalData>>& AgentTuple : CrowdAgents)
		{
			CrowdManager->UnregisterAgent(AgentTuple.Value->Agent.Get());
		}
	}
	CrowdAgents.Reset();
	AgentHandles.Reset();
}

void URecallCrowdSubsystem::TickCrowdManager(float DeltaSeconds)
{
	if (CrowdManager.IsValid() && AgentHandles.Num() > 0)
	{
		{
			TRACE_CPUPROFILER_EVENT_SCOPE_STR("URecallCrowdSubsystem::TickCrowdManager ResetCrowdManager");
			CrowdManager->DestroyCrowdManager();
			CrowdManager->CreateCrowdManager();
		}
		
		ParallelFor(AgentHandles.Num(), [&](int32 Index)
		{
			TRACE_CPUPROFILER_EVENT_SCOPE_STR("URecallCrowdSubsystem::TickCrowdManager RestoreAgent");
			const TSharedPtr<FRecallCrowdAgentInternalData>& AgentInternalData = CrowdAgents[AgentHandles[Index]];
			CrowdManager->SetAgentMoveDirection(AgentInternalData->Agent.Get(), AgentInternalData->MoveDirection);
			AgentInternalData->DetourCrowdAgent.Restore(CrowdManager->GetMutableDetourCrowdAgent(AgentInternalData->Agent.Get()));
		});
		
		QUICK_SCOPE_CYCLE_COUNTER(Recall_Crowd_TickCrowdManager);
		for (int32 IterationStep = 0; IterationStep < IterationStepCount; ++IterationStep)
		{
			TRACE_CPUPROFILER_EVENT_SCOPE_STR("URecallCrowdSubsystem::TickCrowdManager Tick");
			CrowdManager->Tick(DeltaSeconds / static_cast<float>(IterationStepCount));
		}
		
		ParallelFor(AgentHandles.Num(), [&](int32 Index)
		{
			TRACE_CPUPROFILER_EVENT_SCOPE_STR("URecallCrowdSubsystem::TickCrowdManager SaveAgent");
			const TSharedPtr<FRecallCrowdAgentInternalData>& AgentInternalData = CrowdAgents[AgentHandles[Index]];
			AgentInternalData->DetourCrowdAgent.Save(CrowdManager->GetMutableDetourCrowdAgent(AgentInternalData->Agent.Get()));
		});
	}
}

FRecallCrowdAgentHandle URecallCrowdSubsystem::RegisterCrowdAgent()
{
	const FRecallCrowdAgentHandle NewHandle(++CrowdAgentUniqueIdGenerator);
	CreateCrowdAgent_Internal(NewHandle);
	return NewHandle;
}

void URecallCrowdSubsystem::ReleaseCrowdAgent(FRecallCrowdAgentHandle& Handle)
{
	ReleaseCrowdAgent_Internal(Handle);
	Handle.Reset();
}

void URecallCrowdSubsystem::SetAgentMoveDirection(const FRecallCrowdAgentHandle& Handle,
	const FVector& MoveDirection) const
{
	const TSharedPtr<FRecallCrowdAgentInternalData>& CrowdAgent = CrowdAgents.FindChecked(Handle);
	if (CrowdAgent.IsValid())
	{
		CrowdAgent->MoveDirection = MoveDirection;
	}
}

void URecallCrowdSubsystem::SetSuspendCrowdAgentSimulation(const FRecallCrowdAgentHandle& Handle, bool bSuspend)
{
	const TSharedPtr<FRecallCrowdAgentInternalData>& CrowdAgent = CrowdAgents.FindChecked(Handle);
	if (CrowdAgent.IsValid() && ensure(CrowdAgent->Agent.IsValid()))
	{
		CrowdAgent->Agent->SetSuspendCrowdSimulation(bSuspend);
	}
}

void URecallCrowdSubsystem::SetCrowdAgentSimulationState(const FRecallCrowdAgentHandle& Handle,
	ERecallCrowdSimulationState State)
{
	const TSharedPtr<FRecallCrowdAgentInternalData>& CrowdAgent = CrowdAgents.FindChecked(Handle);
	if (CrowdAgent.IsValid() && ensure(CrowdAgent->Agent.IsValid()))
	{
		CrowdAgent->Agent->SetSimulationState(State);
	}
}

FRecallCrowdAgentBlackboard& URecallCrowdSubsystem::GetMutableAgentBlackboardChecked(
	const FRecallCrowdAgentHandle& Handle)
{
	const TSharedPtr<FRecallCrowdAgentInternalData>& CrowdAgent = CrowdAgents.FindChecked(Handle);
	check(CrowdAgent.IsValid() && CrowdAgent->Agent.IsValid());
	return CrowdAgent->Agent->GetMutableBlackboard();
}

const FRecallCrowdAgentBlackboard& URecallCrowdSubsystem::GetAgentBlackboardChecked(
	const FRecallCrowdAgentHandle& Handle) const
{
	const TSharedPtr<FRecallCrowdAgentInternalData>& CrowdAgent = CrowdAgents.FindChecked(Handle);
	check(CrowdAgent.IsValid() && CrowdAgent->Agent.IsValid());
	return CrowdAgent->Agent->GetBlackboard();
}
