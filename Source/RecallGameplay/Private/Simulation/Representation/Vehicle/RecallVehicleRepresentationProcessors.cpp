// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallVehicleRepresentationProcessors.h"

#include "Actor/Vehicle/Interface/RecallPhysicsVehicleActorInterface.h"
#include "MassExtendedExecutionContext.h"
#include "Physics/Vehicle/RecallPhysicsVehicleObject.h"
#include "Simulation/Physics/RecallPhysicsBodyFragment.h"
#include "Simulation/Physics/RecallPhysicsVehicleFragments.h"
#include "Simulation/Representation/RecallActorRepresentationFragments.h"
#include "System/Actor/RecallActorSubsystem.h"
#include "System/Physics/RecallPhysicsSubsystem.h"

//----------------------------------------------------------------------//
// URecallVehicleRepresentationProcessor
//----------------------------------------------------------------------//
URecallVehicleRepresentationProcessor::URecallVehicleRepresentationProcessor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EExtendedProcessorExecutionFlags::All);
	ProcessingPhase = EMassExtendedProcessingPhase::Render;
	bRequiresGameThreadExecution = true;
}

void URecallVehicleRepresentationProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallVehicleRepresentationProcessor::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallActorRepresentationFragment>(EMassExtendedFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FRecallPhysicsBodyFragment>(EMassExtendedFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FRecallPhysicsVehicleFragment>(EMassExtendedFragmentAccess::ReadOnly);
	EntityQuery.AddSubsystemRequirement<URecallActorSubsystem>(EMassExtendedFragmentAccess::ReadOnly);
	EntityQuery.AddSubsystemRequirement<URecallPhysicsSubsystem>(EMassExtendedFragmentAccess::ReadOnly);
}

void URecallVehicleRepresentationProcessor::Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_VehicleRepresentation_Execute);

	EntityQuery.ForEachEntityChunk(Context, [](FMassExtendedExecutionContext& Context)
	{
		const URecallPhysicsSubsystem& PhysicsSystem = Context.GetSubsystemChecked<URecallPhysicsSubsystem>();
		const URecallActorSubsystem& ActorSystem = Context.GetSubsystemChecked<URecallActorSubsystem>();
		
		const TConstArrayView<FRecallPhysicsVehicleFragment> VehicleList = Context.GetFragmentView<FRecallPhysicsVehicleFragment>();
		const TConstArrayView<FRecallActorRepresentationFragment> ActorList = Context.GetFragmentView<FRecallActorRepresentationFragment>();
		const TConstArrayView<FRecallPhysicsBodyFragment> BodyList = Context.GetFragmentView<FRecallPhysicsBodyFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			const FRecallActorRepresentationFragment& ActorFragment = ActorList[EntityIndex];			
			const TWeakObjectPtr<AActor> Actor = ActorSystem.GetActor(ActorFragment.ActorHandle);
			if (!Actor.IsValid() || !Actor->GetClass()->ImplementsInterface(URecallPhysicsVehicleActorInterface::StaticClass()))
			{
				continue;
			}

			const FRecallPhysicsBodyFragment& BodyFragment = BodyList[EntityIndex];
			const TWeakPtr<const FRecallPhysicsBody> Body = PhysicsSystem.GetBody(BodyFragment.BodyHandle);
			const TWeakPtr<const FRecallPhysicsVehicleBody> Vehicle = StaticCastWeakPtr<const FRecallPhysicsVehicleBody>(Body);
			if (!Vehicle.IsValid())
			{
				continue;
			}
			
			const FRecallPhysicsVehicleFragment& VehicleFragment = VehicleList[EntityIndex];			
			IRecallPhysicsVehicleActorInterface::Execute_SetDriverInput(Actor.Get(),
				VehicleFragment.Forward, VehicleFragment.Right, VehicleFragment.Brake, VehicleFragment.HandBrake);

			FRecallVehicleRepresentationInfo RepresentationInfo;
			RepresentationInfo.SpeedKilometersPerHour = Vehicle.Pin()->GetSpeedKilometersPerHour(Context.GetWorld());
			RepresentationInfo.bHasRearContact = Vehicle.Pin()->HasRearWheelsContact();
			
			IRecallPhysicsVehicleActorInterface::Execute_SetVehicleRepresentationInfo(Actor.Get(), RepresentationInfo);
		}
	});
}
