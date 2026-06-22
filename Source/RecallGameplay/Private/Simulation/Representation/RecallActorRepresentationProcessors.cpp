// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallActorRepresentationProcessors.h"

#include "Actor/RecallRepresentationActor.h"
#include "Animation/RecallAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "MassExtendedExecutionContext.h"
#include "Physics/RecallPhysicsObjects.h"
#include "Physics/Character/RecallPhysicsCharacterObject.h"
#include "Simulation/Controller/RecallControllerFragments.h"
#include "Simulation/Movement/RecallMovementFragments.h"
#include "Simulation/Physics/RecallPhysicsBodyFragment.h"
#include "Simulation/Physics/RecallPhysicsCharacterFragments.h"
#include "Simulation/Representation/RecallActorRepresentationFragments.h"
#include "Simulation/Transform/RecallTransformFragments.h"
#include "System/Actor/RecallActorSubsystem.h"
#include "System/Physics/RecallPhysicsSubsystem.h"
#include "System/Representation/RecallActorInterpolationSubsystem.h"
#include "Utility/Math/RecallMathUtils.h"
#include "Utility/Simulation/RecallSimulationUtils.h"

//----------------------------------------------------------------------//
// URecallActorRepresentationInitializer
//----------------------------------------------------------------------//
URecallActorRepresentationInitializer::URecallActorRepresentationInitializer()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EExtendedProcessorExecutionFlags::All);
	ObservedType = FRecallActorRepresentationFragment::StaticStruct();
	Operation = EMassExtendedObservedOperation::Add;
}

void URecallActorRepresentationInitializer::InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallActorRepresentationInitializer::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallActorRepresentationFragment>(EMassExtendedFragmentAccess::ReadWrite);
	EntityQuery.AddConstSharedRequirement<FRecallSkeletalMeshActorRepresentationConstSharedFragment>(EMassExtendedFragmentPresence::Optional);
	EntityQuery.AddConstSharedRequirement<FRecallStaticMeshActorRepresentationConstSharedFragment>(EMassExtendedFragmentPresence::Optional);
	EntityQuery.AddConstSharedRequirement<FRecallActorRepresentationConstSharedFragment>(EMassExtendedFragmentPresence::Optional);
	EntityQuery.AddConstSharedRequirement<FRecallDecalActorRepresentationConstSharedFragment>(EMassExtendedFragmentPresence::Optional);
	EntityQuery.AddSubsystemRequirement<URecallActorSubsystem>(EMassExtendedFragmentAccess::ReadWrite);
}

void URecallActorRepresentationInitializer::Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(Context, [](FMassExtendedExecutionContext& Context)
	{
		URecallActorSubsystem& ActorSystem = Context.GetMutableSubsystemChecked<URecallActorSubsystem>();

		const FRecallSkeletalMeshActorRepresentationConstSharedFragment* SkeletalMeshSharedFragment = Context.GetConstSharedFragmentPtr<FRecallSkeletalMeshActorRepresentationConstSharedFragment>();
		const FRecallStaticMeshActorRepresentationConstSharedFragment* StaticMeshSharedFragment = Context.GetConstSharedFragmentPtr<FRecallStaticMeshActorRepresentationConstSharedFragment>();
		const FRecallActorRepresentationConstSharedFragment* ActorSharedFragment = Context.GetConstSharedFragmentPtr<FRecallActorRepresentationConstSharedFragment>();
		const auto* DecalActorConstSharedFragment = Context.GetConstSharedFragmentPtr<FRecallDecalActorRepresentationConstSharedFragment>();

		const TArrayView<FRecallActorRepresentationFragment> ActorList = Context.GetMutableFragmentView<FRecallActorRepresentationFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			FRecallActorRepresentationFragment& ActorFragment = ActorList[EntityIndex];
			if (ActorFragment.ActorHandle.IsSet())
			{
				continue;
			}

			if (SkeletalMeshSharedFragment != nullptr)
			{
				ActorFragment.ActorHandle = ActorSystem.CreateActor(SkeletalMeshSharedFragment->Definition);
			}
			else if (StaticMeshSharedFragment != nullptr)
			{
				ActorFragment.ActorHandle = ActorSystem.CreateActor(StaticMeshSharedFragment->Definition);
			}
			else if (ActorSharedFragment != nullptr)
			{
				ActorFragment.ActorHandle = ActorSystem.CreateActor(ActorSharedFragment->Definition);
			}
			else if (DecalActorConstSharedFragment != nullptr)
			{
				ActorFragment.ActorHandle = ActorSystem.CreateActor(DecalActorConstSharedFragment->Desc);
			}
		}
	});
}

//----------------------------------------------------------------------//
// URecallActorRepresentationDeinitializer
//----------------------------------------------------------------------//
URecallActorRepresentationDeinitializer::URecallActorRepresentationDeinitializer()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EExtendedProcessorExecutionFlags::All);
	ObservedType = FRecallActorRepresentationFragment::StaticStruct();
	Operation = EMassExtendedObservedOperation::Remove;
}

void URecallActorRepresentationDeinitializer::InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallActorRepresentationDeinitializer::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallActorRepresentationFragment>(EMassExtendedFragmentAccess::ReadWrite);
	EntityQuery.AddSubsystemRequirement<URecallActorSubsystem>(EMassExtendedFragmentAccess::ReadWrite);	
	EntityQuery.AddSubsystemRequirement<URecallActorInterpolationSubsystem>(EMassExtendedFragmentAccess::ReadWrite);
}

void URecallActorRepresentationDeinitializer::Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(Context, [](FMassExtendedExecutionContext& Context)
	{
		URecallActorSubsystem& ActorSystem = Context.GetMutableSubsystemChecked<URecallActorSubsystem>();
		URecallActorInterpolationSubsystem& ActorInterpolationSystem = Context.GetMutableSubsystemChecked<URecallActorInterpolationSubsystem>();

		const TArrayView<FRecallActorRepresentationFragment> ActorList = Context.GetMutableFragmentView<FRecallActorRepresentationFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			FRecallActorRepresentationFragment& ActorFragment = ActorList[EntityIndex];

			ActorInterpolationSystem.ClearInterpolationCache(ActorFragment.ActorHandle);
			ActorSystem.ReleaseActor(ActorFragment.ActorHandle);
		}
	});
}

static const FName RecallActorRepresentation = TEXT("RecallActorRepresentation");

//----------------------------------------------------------------------//
// URecallActorRepresentationProcessor
//----------------------------------------------------------------------//
URecallActorRepresentationProcessor::URecallActorRepresentationProcessor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EExtendedProcessorExecutionFlags::All);
	ProcessingPhase = EMassExtendedProcessingPhase::Render;
	ExecutionOrder.ExecuteInGroup = RecallActorRepresentation;
	bRequiresGameThreadExecution = true;
}

void URecallActorRepresentationProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallActorRepresentationProcessor::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallTransformFragment>(EMassExtendedFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FRecallActorRepresentationFragment>(EMassExtendedFragmentAccess::ReadOnly);
	EntityQuery.AddSubsystemRequirement<URecallActorSubsystem>(EMassExtendedFragmentAccess::ReadOnly);

	// This system is used for representation only so it is safe to write.
	ProcessorRequirements.AddSubsystemRequirement<URecallActorInterpolationSubsystem>(EMassExtendedFragmentAccess::ReadWrite);
}

void URecallActorRepresentationProcessor::Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_Actor_Representation);

	URecallActorInterpolationSubsystem& ActorInterpolationSystem = Context.GetMutableSubsystemChecked<URecallActorInterpolationSubsystem>();

	ActorInterpolationSystem.UpdateDeltaFrame(Context.GetDeltaTimeSeconds());
	
	EntityQuery.ForEachEntityChunk(Context,
		[&ActorInterpolationSystem](FMassExtendedExecutionContext& Context)
	{
		const URecallActorSubsystem& ActorSystem = Context.GetSubsystemChecked<URecallActorSubsystem>();

		const TConstArrayView<FRecallActorRepresentationFragment> ActorList = Context.GetFragmentView<FRecallActorRepresentationFragment>();
		const TConstArrayView<FRecallTransformFragment> TransformList = Context.GetFragmentView<FRecallTransformFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			const FRecallActorRepresentationFragment& ActorFragment = ActorList[EntityIndex];
			const FRecallTransformFragment& TransformFragment = TransformList[EntityIndex];

			const TWeakObjectPtr<AActor> Actor = ActorSystem.GetActor(ActorFragment.ActorHandle);
			if (!Actor.IsValid())
			{
				continue;
			}
			
			Actor->SetActorHiddenInGame(ActorFragment.bHideActor);
			if (ActorFragment.bHideActor)
			{
				continue;
			}
			
			const FTransform RootTransform(
				TransformFragment.Rotation, TransformFragment.Position, ActorFragment.Scale);
			const FTransform Transform = ActorFragment.Offset * RootTransform;
			
			FVector NewPosition = Transform.GetLocation();
			FQuat NewRotation = Transform.GetRotation();
			
			ActorInterpolationSystem.InterpolateVector(ActorFragment.ActorHandle, TEXT("Position"), NewPosition);
			ActorInterpolationSystem.InterpolateQuat(ActorFragment.ActorHandle, TEXT("Rotation"), NewRotation);

			Actor->SetActorLocationAndRotation(NewPosition, NewRotation);

			if (Actor->GetActorScale3D() != Transform.GetScale3D())
			{
				Actor->SetActorScale3D(Transform.GetScale3D());
			}
		}
	});
}

//----------------------------------------------------------------------//
// URecallControllerActorRepresentationProcessor
//----------------------------------------------------------------------//
URecallControllerActorRepresentationProcessor::URecallControllerActorRepresentationProcessor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EExtendedProcessorExecutionFlags::All);
	ProcessingPhase = EMassExtendedProcessingPhase::Render;
	ExecutionOrder.ExecuteAfter.Add(RecallActorRepresentation);
	bRequiresGameThreadExecution = true;
}

void URecallControllerActorRepresentationProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallControllerActorRepresentationProcessor::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager)
{
	// This system is used for representation only so it is safe to write.
	EntityQuery.AddSubsystemRequirement<URecallActorInterpolationSubsystem>(EMassExtendedFragmentAccess::ReadWrite);
	
	EntityQuery.AddRequirement<FRecallActorRepresentationFragment>(EMassExtendedFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FRecallControllerFragment>(EMassExtendedFragmentAccess::ReadOnly);
	EntityQuery.AddSubsystemRequirement<URecallActorSubsystem>(EMassExtendedFragmentAccess::ReadOnly);
}

void URecallControllerActorRepresentationProcessor::Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_ControllerActor_Representation);

	EntityQuery.ForEachEntityChunk(Context,
		[](FMassExtendedExecutionContext& Context)
	{
		URecallActorInterpolationSubsystem& ActorInterpolationSystem = Context.GetMutableSubsystemChecked<URecallActorInterpolationSubsystem>();
			
		const URecallActorSubsystem& ActorSystem = Context.GetSubsystemChecked<URecallActorSubsystem>();

		const TConstArrayView<FRecallActorRepresentationFragment> ActorList = Context.GetFragmentView<FRecallActorRepresentationFragment>();
		const TConstArrayView<FRecallControllerFragment> ControllerList = Context.GetFragmentView<FRecallControllerFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			const FRecallActorRepresentationFragment& ActorFragment = ActorList[EntityIndex];

			const TWeakObjectPtr<ARecallRepresentationActor> Actor = ActorSystem.GetActor<ARecallRepresentationActor>(ActorFragment.ActorHandle);
			if (!Actor.IsValid())
			{
				continue;
			}
			
			const FRecallControllerFragment& ControllerFragment = ControllerList[EntityIndex];

			FQuat ControlRotation = ControllerFragment.ControlRotation.Quaternion();

			ActorInterpolationSystem.InterpolateQuat(ActorFragment.ActorHandle, TEXT("ControlRotation"), ControlRotation);
			
			Actor->SetControlRotationRepresentation(ControlRotation);
		}
	});
}

//----------------------------------------------------------------------//
// URecallActorAnimationRepresentationProcessor
//----------------------------------------------------------------------//
URecallActorAnimationRepresentationProcessor::URecallActorAnimationRepresentationProcessor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EExtendedProcessorExecutionFlags::All);
	ProcessingPhase = EMassExtendedProcessingPhase::Render;
	bRequiresGameThreadExecution = true;
}

void URecallActorAnimationRepresentationProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassExtendedEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallActorAnimationRepresentationProcessor::ConfigureQueries(const TSharedRef<FMassExtendedEntityManager>& EntityManager)
{
	FMassExtendedTagBitSet RequiredTags;
	RequiredTags.Add(*FRecallSkeletalMeshActorRepresentationTag::StaticStruct());
	
	FMassExtendedTagBitSet InvalidTags;
	// InvalidTags.Add(*FRecallOverrideActorAnimationRepresentationTag::StaticStruct());
	
	EntityQuery.AddRequirement<FRecallActorRepresentationFragment>(EMassExtendedFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FRecallMovementFragment>(EMassExtendedFragmentAccess::ReadOnly, EMassExtendedFragmentPresence::Optional);
	EntityQuery.AddRequirement<FRecallPhysicsBodyFragment>(EMassExtendedFragmentAccess::ReadOnly, EMassExtendedFragmentPresence::Optional);
	EntityQuery.AddRequirement<FRecallControllerFragment>(EMassExtendedFragmentAccess::ReadOnly, EMassExtendedFragmentPresence::Optional);
	EntityQuery.AddRequirement<FRecallPhysicsCharacterFragment>(EMassExtendedFragmentAccess::ReadOnly, EMassExtendedFragmentPresence::Optional);
	EntityQuery.AddTagRequirements<EMassExtendedFragmentPresence::None>(InvalidTags);
	EntityQuery.AddTagRequirements<EMassExtendedFragmentPresence::All>(RequiredTags);
	EntityQuery.AddSubsystemRequirement<URecallActorSubsystem>(EMassExtendedFragmentAccess::ReadOnly);
	EntityQuery.AddSubsystemRequirement<URecallPhysicsSubsystem>(EMassExtendedFragmentAccess::ReadOnly);
}

void URecallActorAnimationRepresentationProcessor::Execute(FMassExtendedEntityManager& EntityManager, FMassExtendedExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_Actor_AnimationRepresentation);

	EntityQuery.ForEachEntityChunk(Context, [](FMassExtendedExecutionContext& Context)
	{
		const float DeltaTime = Recall::Simulation::Utils::GetDilatedFixedDeltaTime(Context.GetWorld());
		const float DeltaFrame = Recall::Simulation::Utils::GetRepresentationDeltaFrame(Context.GetWorld());

		const float AnimationDeltaTime = DeltaTime * DeltaFrame;

		const URecallActorSubsystem& ActorSystem = Context.GetSubsystemChecked<URecallActorSubsystem>();
		const URecallPhysicsSubsystem& PhysicsSystem = Context.GetSubsystemChecked<URecallPhysicsSubsystem>();

		const TConstArrayView<FRecallActorRepresentationFragment> ActorList = Context.GetFragmentView<FRecallActorRepresentationFragment>();
		const TConstArrayView<FRecallMovementFragment> MovementList = Context.GetFragmentView<FRecallMovementFragment>();
		const TConstArrayView<FRecallPhysicsBodyFragment> BodyList = Context.GetFragmentView<FRecallPhysicsBodyFragment>();
		const TConstArrayView<FRecallControllerFragment> ControllerList = Context.GetFragmentView<FRecallControllerFragment>();
		const TConstArrayView<FRecallPhysicsCharacterFragment> CharacterList = Context.GetFragmentView<FRecallPhysicsCharacterFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			const FRecallActorRepresentationFragment& ActorFragment = ActorList[EntityIndex];

			const TWeakObjectPtr<AActor> Actor = ActorSystem.GetActor(ActorFragment.ActorHandle);
			if (!Actor.IsValid())
			{
				continue;
			}

			Actor->ForEachComponent<USkeletalMeshComponent>(false, [&](USkeletalMeshComponent* Component)
			{
				URecallAnimInstance* AnimInstance = Cast<URecallAnimInstance>(Component->GetAnimInstance());
				if (IsValid(AnimInstance))
				{
					if (MovementList.IsValidIndex(EntityIndex))
					{
						const FRecallMovementFragment& MovementFragment = MovementList[EntityIndex];
						AnimInstance->MovementDirection = FVector2f(MovementFragment.MovementDirection.X, MovementFragment.MovementDirection.Y);
					}

					if (BodyList.IsValidIndex(EntityIndex))
					{
						const FRecallPhysicsBodyFragment& BodyFragment = BodyList[EntityIndex];
						const TWeakPtr<const FRecallPhysicsBody> Body = PhysicsSystem.GetBody(BodyFragment.BodyHandle);
						if (Body.IsValid())
						{							
							AnimInstance->PhysicsVelocity = Recall::Math::Utils::UnitsPerFrameToPerSecond(
								Body.Pin()->GetLinearVelocity());
						}
					}

					if (CharacterList.IsValidIndex(EntityIndex))
					{
						const FRecallPhysicsCharacterFragment& CharacterFragment = CharacterList[EntityIndex];
						AnimInstance->bGrounded = CharacterFragment.bIsSupported;
					}
					
					if (ControllerList.IsValidIndex(EntityIndex))
					{
						const FRecallControllerFragment& ControllerFragment = ControllerList[EntityIndex];
						AnimInstance->ControlRotation = ControllerFragment.ControlRotation;
					}
				}

				// We don't want to actually set the delta time for the animation tick to 0 since it will cause a bug with anim notify states
				// triggering their Begin event multiple times.
				Component->SetExternalDeltaTime(AnimationDeltaTime > 0 ? AnimationDeltaTime : DeltaTime);

				const bool StopAnimationTick = DeltaFrame <= 0; // || AnimationFragment.bPauseAnimation
				Component->EnableExternalUpdate(!StopAnimationTick);
			});
		}
	});
}
