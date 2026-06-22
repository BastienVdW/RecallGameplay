// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallCarryProcessors.h"

#include "MassExecutionContext.h"
#include "MassEntityView.h"
#include "RecallSignalSubsystem.h"
#include "Simulation/Carry/RecallCarryFragments.h"
#include "Simulation/Carry/RecallCarrySignalTypes.h"
#include "Simulation/Interact/RecallInteractFragments.h"
#include "Simulation/Physics/RecallPhysicsBodyFragment.h"
#include "Simulation/StateTree/RecallStateTreeSignalTypes.h"
#include "Simulation/StateTree/RecallStateTreeProcessorGroupTypes.h"
#include "Simulation/Transform/RecallTransformFragments.h"
#include "System/Physics/RecallPhysicsSubsystem.h"

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
#include "DrawDebugHelpers.h"

static bool bDebugCarryPreview = false;
static FAutoConsoleVariableRef CVarRecallDebugCarryPreview(
	TEXT("recall.Gameplay.CarryPreview"),
	bDebugCarryPreview,
	TEXT("Preview for Carry interaction")
);
#endif // UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT

//----------------------------------------------------------------------//
// URecallCarryableDestructor
//----------------------------------------------------------------------//
URecallCarryableDestructor::URecallCarryableDestructor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::All);
	ObservedTypes.Add(FRecallCarryableFragment::StaticStruct());
	ObservedOperations = EMassObservedOperationFlags::RemoveElement;
}

void URecallCarryableDestructor::InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallCarryableDestructor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallCarryableFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FRecallPhysicsBodyFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddSubsystemRequirement<URecallPhysicsSubsystem>(EMassFragmentAccess::ReadWrite);
}

void URecallCarryableDestructor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_Carryable_Destructor);

	EntityQuery.ForEachEntityChunk(Context, [](FMassExecutionContext& Context)
	{
		const FMassEntityManager& EntityManager = Context.GetEntityManagerChecked();
		URecallPhysicsSubsystem& PhysicsSystem = Context.GetMutableSubsystemChecked<URecallPhysicsSubsystem>();
		
		const TConstArrayView<FRecallPhysicsBodyFragment> BodyList = Context.GetFragmentView<FRecallPhysicsBodyFragment>();
		const TArrayView<FRecallCarryableFragment> CarryableList = Context.GetMutableFragmentView<FRecallCarryableFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			const FRecallPhysicsBodyFragment& BodyFragment = BodyList[EntityIndex];
			FRecallCarryableFragment& CarryableFragment = CarryableList[EntityIndex];

			for (const FMassEntityHandle& CarrierEntity : CarryableFragment.CarrierEntities)
			{
				if (!EntityManager.IsEntityValid(CarrierEntity))
				{
					continue;
				}

				const FMassEntityView CarrierView(EntityManager, CarrierEntity);
				const FRecallPhysicsBodyFragment& CarrierBodyFragment = CarrierView.GetFragmentData<FRecallPhysicsBodyFragment>();
				PhysicsSystem.RemoveAllConstrains(BodyFragment.BodyHandle, CarrierBodyFragment.BodyHandle);
			}
			CarryableFragment.CarrierEntities.Empty();
		}
	});
}

//----------------------------------------------------------------------//
// URecallCarryableSignalProcessor
//----------------------------------------------------------------------//
URecallCarryableSignalProcessor::URecallCarryableSignalProcessor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ProcessingPhase = EMassProcessingPhase::PrePhysics;
	ExecutionOrder.ExecuteAfter.Add(Recall::StateTree::ProcessorGroupNames::StateTreeUpdate);
	bRequiresGameThreadExecution = true;
}

void URecallCarryableSignalProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);

	SubscribeToSignal(Recall::Carry::Signals::Start);
	SubscribeToSignal(Recall::Carry::Signals::Stop);
}

void URecallCarryableSignalProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallCarryableFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FRecallInteractableFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FRecallPhysicsBodyFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddConstSharedRequirement<FRecallCarryableConstSharedFragment>();
	EntityQuery.AddSubsystemRequirement<URecallPhysicsSubsystem>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddSubsystemRequirement<URecallSignalSubsystem>(EMassFragmentAccess::ReadWrite);
}

struct FRecallCarryableContext
{
	const FRecallPhysicsBodyFragment& BodyFragment;
	const FRecallInteractableFragment& InteractableFragment;
	FRecallCarryableFragment& CarryableFragment;
	const FRecallCarryableConstSharedFragment& CarryableConstSharedFragment;
};

/**
 * Update the state of the interactor carrying this entity.
 */
static void UpdateCarrierStates(
	FMassExecutionContext& Context,
	URecallPhysicsSubsystem& PhysicsSystem,
	const FRecallCarryableContext& CarryableContext)
{
	const FMassEntityManager& EntityManager = Context.GetEntityManagerChecked();

	auto UpdateCarrierState = [&](const FMassEntityHandle& CarrierEntity, bool bCarry)
	{
		// Skip physics constraint management if disabled
		if (!CarryableContext.CarryableConstSharedFragment.Settings.bUsePhysicsConstraints)
		{
			return;
		}

		const FMassEntityView CarrierView(EntityManager, CarrierEntity);
		const FRecallPhysicsBodyFragment& CarrierBodyFragment = CarrierView.GetFragmentData<FRecallPhysicsBodyFragment>();

		if (bCarry)
		{
			PhysicsSystem.CreateFixedConstrain(CarryableContext.BodyFragment.BodyHandle, CarrierBodyFragment.BodyHandle);
		}
		else
		{
			PhysicsSystem.RemoveAllConstrains(CarryableContext.BodyFragment.BodyHandle, CarrierBodyFragment.BodyHandle);
		}
	};

	TArray<FMassEntityHandle>& CarriersEntities = CarryableContext.CarryableFragment.CarrierEntities;

	for (int32 CarrierIndex = CarriersEntities.Num() - 1; CarrierIndex >= 0; CarrierIndex--)
	{
		const FMassEntityHandle& CarrierEntity = CarriersEntities[CarrierIndex];

		if (CarryableContext.InteractableFragment.InProgressInstigators.Contains(CarrierEntity))
		{
			continue;
		}
		
		// Stopped carrying
		UpdateCarrierState(CarrierEntity, false);
		CarriersEntities.RemoveAt(CarrierIndex, EAllowShrinking::No);
	}
	
	for (const FMassEntityHandle& InteractInstigator : CarryableContext.InteractableFragment.InProgressInstigators)
	{
		if (CarriersEntities.Contains(InteractInstigator))
		{
			continue;
		}

		// Start carrying
		UpdateCarrierState(InteractInstigator, true);
		CarriersEntities.Add(InteractInstigator);
	}
}

void URecallCarryableSignalProcessor::SignalEntities(FMassEntityManager& EntityManager, FMassExecutionContext& Context, FRecallSignalNameLookup& EntitySignals)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_Carryable_Signal);

	EntityQuery.ForEachEntityChunk(Context, [&EntitySignals](FMassExecutionContext& Context)
	{
		URecallPhysicsSubsystem& PhysicsSystem = Context.GetMutableSubsystemChecked<URecallPhysicsSubsystem>();
		URecallSignalSubsystem& SignalSystem = Context.GetMutableSubsystemChecked<URecallSignalSubsystem>();

		const FRecallCarryableConstSharedFragment& CarryableConstSharedFragment = Context.GetConstSharedFragment<FRecallCarryableConstSharedFragment>();

		const TConstArrayView<FRecallInteractableFragment> InteractableList = Context.GetFragmentView<FRecallInteractableFragment>();
		const TConstArrayView<FRecallPhysicsBodyFragment> BodyList = Context.GetFragmentView<FRecallPhysicsBodyFragment>();

		const TArrayView<FRecallCarryableFragment> CarryableList = Context.GetMutableFragmentView<FRecallCarryableFragment>();

		const bool bHasCarriedTag = Context.DoesArchetypeHaveTag<FRecallCarriedTag>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			const FMassEntityHandle Entity = Context.GetEntity(EntityIndex);

			const FRecallInteractableFragment& InteractableFragment = InteractableList[EntityIndex];
			const FRecallPhysicsBodyFragment& BodyFragment = BodyList[EntityIndex];

			FRecallCarryableFragment& CarryableFragment = CarryableList[EntityIndex];

			const bool bIsCarried = CarryableConstSharedFragment.IsCarried(InteractableFragment.InProgressInstigators.Num());

			TArray<FName> Signals;
			EntitySignals.GetSignalsForEntity(Entity, Signals);

			if (Signals.Contains(Recall::Carry::Signals::Start))
			{
				if (bIsCarried && CarryableConstSharedFragment.Settings.bRunStateTreeWhileCarried)
				{
					SignalSystem.SignalEntity(Recall::StateTree::Signals::StateTreeStart, Entity);
				}
			}
			else if (Signals.Contains(Recall::Carry::Signals::Stop))
			{
				if (!bIsCarried && CarryableConstSharedFragment.Settings.bRunStateTreeWhileCarried)
				{
					SignalSystem.SignalEntity(Recall::StateTree::Signals::StateTreeStop, Entity);
				}
			}

			const FRecallCarryableContext CarryableContext
			{
				BodyFragment, InteractableFragment, CarryableFragment, CarryableConstSharedFragment
			};

			UpdateCarrierStates(Context, PhysicsSystem, CarryableContext);

			// Update Carried tag based on state
			if (bIsCarried && !bHasCarriedTag)
			{
				Context.Defer().AddTag<FRecallCarriedTag>(Entity);
			}
			else if (!bIsCarried && bHasCarriedTag)
			{
				Context.Defer().RemoveTag<FRecallCarriedTag>(Entity);
			}
		}
	});
}

//----------------------------------------------------------------------//
// URecallCarryableSignalProcessor
//----------------------------------------------------------------------//
URecallCarryableRepresentationProcessor::URecallCarryableRepresentationProcessor()
	: EntityQuery(*this)
{
	ExecutionFlags = (int32)EProcessorExecutionFlags::All;
	ProcessingPhase = EMassProcessingPhase::Render;
	bRequiresGameThreadExecution = true;
}

void URecallCarryableRepresentationProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallCarryableRepresentationProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	EntityQuery.AddRequirement<FRecallTransformFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FRecallCarryableFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddConstSharedRequirement<FRecallCarryableConstSharedFragment>();
#endif // UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
}

void URecallCarryableRepresentationProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	QUICK_SCOPE_CYCLE_COUNTER(Recall_Carryable_Representation);

	if (!bDebugCarryPreview)
	{
		return;
	}

	EntityQuery.ForEachEntityChunk(Context, [](FMassExecutionContext& Context)
	{
		const FRecallCarryableConstSharedFragment& CarryableConstSharedFragment = Context.GetConstSharedFragment<FRecallCarryableConstSharedFragment>();

		const TConstArrayView<FRecallTransformFragment> TransformList = Context.GetFragmentView<FRecallTransformFragment>();
		const TConstArrayView<FRecallCarryableFragment> CarryableList = Context.GetFragmentView<FRecallCarryableFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			const FRecallTransformFragment& TransformFragment = TransformList[EntityIndex];
			const FRecallCarryableFragment& CarryableFragment = CarryableList[EntityIndex];

			const FString DebugString = FString::Printf(TEXT("%d / %d-%d"),
				CarryableFragment.GetCarrierCount(),
				CarryableConstSharedFragment.Settings.CarrierRange.GetLowerBound().GetValue(),
				CarryableConstSharedFragment.Settings.CarrierRange.GetUpperBound().GetValue());

			DrawDebugString(Context.GetWorld(), TransformFragment.Position, DebugString);
		}
	});
#endif // UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
}
