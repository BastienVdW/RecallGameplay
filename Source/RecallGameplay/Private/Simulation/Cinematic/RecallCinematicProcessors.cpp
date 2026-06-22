// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallCinematicProcessors.h"

#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "MassExecutionContext.h"
#include "RecallCinematicInternalFragments.h"
#include "RecallSignalSubsystem.h"
#include "MovieScene/RecallMovieSceneLabelSection.h"
#include "MovieScene/RecallMovieSceneLabelTrack.h"
#include "MovieScene.h"
#include "Representation/Actor/RecallActorMeshRepresentationTypes.h"
#include "Simulation/Cinematic/RecallCinematicFragments.h"
#include "Simulation/Cinematic/RecallCinematicSignalTypes.h"
#include "Simulation/Transform/RecallTransformFragments.h"
#include "System/Actor/RecallActorSubsystem.h"
#include "System/Asset/RecallAssetManagerSubsystem.h"
#include "Utility/Player/RecallPlayerUtils.h"

//----------------------------------------------------------------------//
// URecallCinematicEventProcessor
//----------------------------------------------------------------------//
URecallCinematicEventProcessor::URecallCinematicEventProcessor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void URecallCinematicEventProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);

	SubscribeToSignal(Recall::Cinematic::Signals::Player::Play);
	SubscribeToSignal(Recall::Cinematic::Signals::Player::PlayLoop);
	SubscribeToSignal(Recall::Cinematic::Signals::Player::Stop);
}

void URecallCinematicEventProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallCinematicFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FRecallCinematicInternalFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddSubsystemRequirement<URecallActorSubsystem>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddSubsystemRequirement<URecallAssetManagerSubsystem>(EMassFragmentAccess::ReadWrite);
}

void URecallCinematicEventProcessor::SignalEntities(FMassEntityManager& EntityManager, FMassExecutionContext& Context, FRecallSignalNameLookup& EntitySignals)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_Cinematic_Event);

	EntityQuery.ForEachEntityChunk(Context, [&EntitySignals](FMassExecutionContext& Context)
	{
		URecallActorSubsystem& ActorSystem = Context.GetMutableSubsystemChecked<URecallActorSubsystem>();
		URecallAssetManagerSubsystem& AssetManagerSystem = Context.GetMutableSubsystemChecked<URecallAssetManagerSubsystem>();

		const TArrayView<FRecallCinematicFragment> CinematicList = Context.GetMutableFragmentView<FRecallCinematicFragment>();
		const TArrayView<FRecallCinematicInternalFragment> CinematicInternalList = Context.GetMutableFragmentView<FRecallCinematicInternalFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			const FMassEntityHandle Entity = Context.GetEntity(EntityIndex);

			FRecallCinematicFragment& CinematicFragment = CinematicList[EntityIndex];
			FRecallCinematicInternalFragment& CinematicInternalFragment = CinematicInternalList[EntityIndex];

			TArray<FName> Signals;
			EntitySignals.GetSignalsForEntity(Entity, Signals);

			if (Signals.Contains(Recall::Cinematic::Signals::Player::Play) ||
				Signals.Contains(Recall::Cinematic::Signals::Player::PlayLoop))
			{
				AssetManagerSystem.ReleaseAsset(CinematicInternalFragment.CinematicAssetHandle);
				ActorSystem.ReleaseActor(CinematicInternalFragment.CinematicActorHandle);

				CinematicInternalFragment.CinematicAssetHandle = AssetManagerSystem.RequestAsset(CinematicFragment.LevelSequenceAsset, CinematicFragment.LoadDuration);

				if (Signals.Contains(Recall::Cinematic::Signals::Player::PlayLoop))
				{
					CinematicInternalFragment.NumLoopLeft = CinematicFragment.NumLoops;
				}
				else if (Signals.Contains(Recall::Cinematic::Signals::Player::Play))
				{
					CinematicInternalFragment.NumLoopLeft = 1;
				}

				Context.Defer().AddTag<FRecallCinematicPlayingTag>(Entity);
			}
			else if (Signals.Contains(Recall::Cinematic::Signals::Player::Stop))
			{
				AssetManagerSystem.ReleaseAsset(CinematicInternalFragment.CinematicAssetHandle);
				ActorSystem.ReleaseActor(CinematicInternalFragment.CinematicActorHandle);

				CinematicInternalFragment.LabelSignals.Reset();
				CinematicFragment.Reset();

				Context.Defer().RemoveTag<FRecallCinematicPlayingTag>(Entity);
			}
		}
	});
}

//----------------------------------------------------------------------//
// URecallCinematicPlayingProcessor
//----------------------------------------------------------------------//
URecallCinematicPlayingProcessor::URecallCinematicPlayingProcessor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::All);
	ProcessingPhase = EMassProcessingPhase::FrameEnd;
}

void URecallCinematicPlayingProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallCinematicPlayingProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	FMassTagBitSet RequiredTags;
	RequiredTags.Add(FRecallCinematicPlayingTag::StaticStruct());

	EntityQuery.AddRequirement<FRecallTransformFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FRecallCinematicFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FRecallCinematicInternalFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddConstSharedRequirement<FRecallCinematicInternalSharedFragment>();
	EntityQuery.AddTagRequirements<EMassFragmentPresence::All>(RequiredTags);
	EntityQuery.AddSubsystemRequirement<URecallActorSubsystem>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddSubsystemRequirement<URecallAssetManagerSubsystem>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddSubsystemRequirement<URecallSignalSubsystem>(EMassFragmentAccess::ReadWrite);
}

static float ExtractLevelSequenceDurationSeconds(const ULevelSequence* Sequence)
{
	const UMovieScene* MovieScene = IsValid(Sequence) ? Sequence->GetMovieScene() : nullptr;
	if (!IsValid(MovieScene))
	{
		return 0.0f;
	}

	const FFrameRate TickResolution = MovieScene->GetTickResolution();
	const FFrameRate DisplayRate = MovieScene->GetDisplayRate();
	
	const TRange<FFrameNumber> PlaybackRange   = MovieScene->GetPlaybackRange();

	const FFrameNumber SrcStartFrame = UE::MovieScene::DiscreteInclusiveLower(PlaybackRange);
	const FFrameNumber SrcEndFrame = UE::MovieScene::DiscreteExclusiveUpper(PlaybackRange);

	const FFrameTime EndingTime = ConvertFrameTime(SrcEndFrame, TickResolution, DisplayRate);

	const FFrameNumber StartingFrame = ConvertFrameTime(SrcStartFrame, TickResolution, DisplayRate).FloorToFrame();
	const FFrameNumber EndingFrame = EndingTime.FloorToFrame();

	const int32 DurationFrames = (EndingFrame - StartingFrame).Value;
	const float DurationSubFrames = EndingTime.GetSubFrame();
	
	return FQualifiedFrameTime(FFrameTime(DurationFrames, DurationSubFrames), DisplayRate).AsSeconds();
}

static void BindLabelSignals(
	const ULevelSequence* Sequence, 
	FRecallCinematicInternalFragment& CinematicInternalFragment)
{
	CinematicInternalFragment.LabelSignals.Reset();

	const UMovieScene* MovieScene = IsValid(Sequence) ? Sequence->GetMovieScene() : nullptr;
	if (!IsValid(MovieScene))
	{
		return;
	}

	for (const UMovieSceneTrack* Track : MovieScene->GetTracks())
	{
		if (!IsValid(Track) || !Track->IsA<URecallMovieSceneLabelTrack>())
		{
			continue;
		}

		const URecallMovieSceneLabelTrack* LabelTrack = CastChecked<URecallMovieSceneLabelTrack>(Track);

		for (const UMovieSceneSection* Section : LabelTrack->GetAllSections())
		{
			if (!IsValid(Section) || !Section->IsA<URecallMovieSceneLabelSection>())
			{
				continue;
			}

			const URecallMovieSceneLabelSection* LabelSection = CastChecked<URecallMovieSceneLabelSection>(Section);
			if (LabelSection->LabelTrackData.Label.IsNone())
			{
				continue;
			}

			const FFrameRate TickResolution = MovieScene->GetTickResolution();
			const FFrameNumber SectionOffset = Section->GetRange().HasLowerBound() ? Section->GetRange().GetLowerBoundValue() : 0;
			const double TimeSeconds = TickResolution.AsSeconds(SectionOffset);

			FRecallCinematicLabelSignal& LabelSignal = CinematicInternalFragment.LabelSignals.AddDefaulted_GetRef();
			LabelSignal.Label = LabelSection->LabelTrackData.Label;
			LabelSignal.TimeSeconds = TimeSeconds;
		}
	}

	CinematicInternalFragment.LabelSignals.StableSort([](const FRecallCinematicLabelSignal& lhs, const FRecallCinematicLabelSignal& rhs)
		{
			return lhs.TimeSeconds <= rhs.TimeSeconds;
		}
	);
}

void URecallCinematicPlayingProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_Cinematic_Playing);

	EntityQuery.ForEachEntityChunk(Context, [](FMassExecutionContext& Context)
	{
		const URecallAssetManagerSubsystem& AssetManagerSystem = Context.GetSubsystemChecked<URecallAssetManagerSubsystem>();

		URecallActorSubsystem& ActorSystem = Context.GetMutableSubsystemChecked<URecallActorSubsystem>();
		URecallSignalSubsystem& SignalSystem = Context.GetMutableSubsystemChecked<URecallSignalSubsystem>();

		const FRecallCinematicInternalSharedFragment& SharedFragment = Context.GetConstSharedFragment<FRecallCinematicInternalSharedFragment>();

		const TConstArrayView<FRecallTransformFragment> TransformList = Context.GetFragmentView<FRecallTransformFragment>();

		const TArrayView<FRecallCinematicFragment> CinematicList = Context.GetMutableFragmentView<FRecallCinematicFragment>();
		const TArrayView<FRecallCinematicInternalFragment> CinematicInternalList = Context.GetMutableFragmentView<FRecallCinematicInternalFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			const FMassEntityHandle Entity = Context.GetEntity(EntityIndex);
			const FRecallTransformFragment& TransformFragment = TransformList[EntityIndex];

			FRecallCinematicFragment& CinematicFragment = CinematicList[EntityIndex];
			FRecallCinematicInternalFragment& CinematicInternalFragment = CinematicInternalList[EntityIndex];

			// Wait load
			if (!AssetManagerSystem.IsAssetLoaded(CinematicInternalFragment.CinematicAssetHandle))
			{
				continue;
			}

			// Spawn actor
			if (!CinematicInternalFragment.CinematicActorHandle.IsValid())
			{
				ULevelSequence* LevelSequenceAsset = AssetManagerSystem.GetLoadedAsset<ULevelSequence>(CinematicInternalFragment.CinematicAssetHandle);
				if (!ensure(LevelSequenceAsset))
				{
					SignalSystem.SignalEntity(Recall::Cinematic::Signals::Player::Stop, Entity);
					continue;
				}

				FLevelSequenceActorRepresentationDesc Desc;
				Desc.Blueprint = SharedFragment.LevelSequenceActorClass;
				Desc.LevelSequenceAsset = LevelSequenceAsset;

				CinematicInternalFragment.CinematicActorHandle = ActorSystem.CreateActor(Desc);
				CinematicInternalFragment.TimeSeconds = 0.0;
				CinematicInternalFragment.DurationSeconds = ExtractLevelSequenceDurationSeconds(LevelSequenceAsset);

				BindLabelSignals(LevelSequenceAsset, CinematicInternalFragment);
				
				if (EnumHasAnyFlags(CinematicFragment.Callback, ERecallCinematicCallback::Start))
				{
					SignalSystem.SignalEntity(Recall::Cinematic::Signals::Callback::OnStart, Entity);
				}
			}
			// Tick
			else
			{
				CinematicInternalFragment.TimeSeconds += static_cast<double>(Context.GetDeltaTimeSeconds());

				while (CinematicInternalFragment.LabelSignals.Num() && CinematicInternalFragment.TimeSeconds >= CinematicInternalFragment.LabelSignals[0].TimeSeconds)
				{
					SignalSystem.SignalEntity(CinematicInternalFragment.LabelSignals[0].Label, Entity);
					CinematicInternalFragment.LabelSignals.RemoveAt(0, EAllowShrinking::No);
				}

				if (CinematicInternalFragment.TimeSeconds >= CinematicInternalFragment.DurationSeconds)
				{
					if (CinematicInternalFragment.NumLoopLeft > 0)
					{
						CinematicInternalFragment.NumLoopLeft--;
					}

					if (CinematicInternalFragment.NumLoopLeft == 0)
					{
						if (EnumHasAnyFlags(CinematicFragment.Callback, ERecallCinematicCallback::End))
						{
							SignalSystem.SignalEntity(Recall::Cinematic::Signals::Callback::OnEnd, Entity);
						}

						SignalSystem.SignalEntity(Recall::Cinematic::Signals::Player::Stop, Entity);
					}
					else
					{
						CinematicInternalFragment.TimeSeconds -= CinematicInternalFragment.DurationSeconds;
					}
				}
			}
		}
	});
}

//----------------------------------------------------------------------//
// URecallCinematicRepresentationProcessor
//----------------------------------------------------------------------//
URecallCinematicRepresentationProcessor::URecallCinematicRepresentationProcessor()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::All);
	ProcessingPhase = EMassProcessingPhase::Render;
	bRequiresGameThreadExecution = true;
}

void URecallCinematicRepresentationProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallCinematicRepresentationProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	FMassTagBitSet RequiredTags;
	RequiredTags.Add(FRecallCinematicPlayingTag::StaticStruct());

	EntityQuery.AddRequirement<FRecallTransformFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FRecallCinematicFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FRecallCinematicInternalFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddConstSharedRequirement<FRecallCinematicInternalSharedFragment>();
	EntityQuery.AddTagRequirements<EMassFragmentPresence::All>(RequiredTags);
	EntityQuery.AddSubsystemRequirement<URecallActorSubsystem>(EMassFragmentAccess::ReadOnly);
}

static void BindLevelSequence(
	const TWeakObjectPtr<ALevelSequenceActor>& LevelSequenceActor,
	const TArray<FRecallCinematicBinding>& Bindings,
	const URecallActorSubsystem& ActorSystem)
{
	if (!LevelSequenceActor.IsValid())
	{
		return;
	}

	UMovieScene* MovieScene = LevelSequenceActor->LevelSequenceAsset ? LevelSequenceActor->LevelSequenceAsset->GetMovieScene() : NULL;
	if (!MovieScene)
	{
		return;
	}

	for (const FRecallCinematicBinding& BindingOverride : Bindings)
	{
		const TWeakObjectPtr<AActor> BindingActor = ActorSystem.GetActor(BindingOverride.ActorHandle);
		if (!BindingActor.IsValid())
		{
			continue;
		}

		const TArray<FMovieSceneBinding>& MovieSceneBindings = const_cast<const UMovieScene*>(MovieScene)->GetBindings();
		for (const FMovieSceneBinding& MovieSceneBinding : MovieSceneBindings)
		{
			const FGuid& BindingGuid = MovieSceneBinding.GetObjectGuid();
			FString BindingName;
			if (const FMovieScenePossessable* Possessable = MovieScene->FindPossessable(BindingGuid))
			{
				BindingName = Possessable->GetName();
			}
			else if (const FMovieSceneSpawnable* Spawnable = MovieScene->FindSpawnable(BindingGuid))
			{
				BindingName = Spawnable->GetName();
			}
			if (BindingName != BindingOverride.Name)
			{
				continue;
			}

			const FMovieSceneObjectBindingID TargetObjectBindingID = UE::MovieScene::FRelativeObjectBindingID(MovieSceneBinding.GetObjectGuid());
			LevelSequenceActor->SetBinding(TargetObjectBindingID, { BindingActor.Get() });
		}
	}
}

static bool ShouldPlayCinematicRepresentation(const UObject* WorldContextObject, const TArray<FString>& FilteredPlayers)
{
	if (FilteredPlayers.Num() == 0) // Empty filter
	{
		return true;
	}

	for (const FString& PlayerID : FilteredPlayers)
	{
		if (Recall::Player::Utils::IsLocalPlayer(WorldContextObject, PlayerID))
		{
			return true;
		}
	}

	return false;
}

void URecallCinematicRepresentationProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_Cinematic_Representation);

	EntityQuery.ForEachEntityChunk(Context, [](FMassExecutionContext& Context)
	{
		const URecallActorSubsystem& ActorSystem = Context.GetSubsystemChecked<URecallActorSubsystem>();

		const FRecallCinematicInternalSharedFragment& SharedFragment = Context.GetConstSharedFragment<FRecallCinematicInternalSharedFragment>();

		const TConstArrayView<FRecallCinematicFragment> CinematicList = Context.GetFragmentView<FRecallCinematicFragment>();
		const TConstArrayView<FRecallCinematicInternalFragment> CinematicInternalList = Context.GetFragmentView<FRecallCinematicInternalFragment>();
		const TConstArrayView<FRecallTransformFragment> TransformList = Context.GetFragmentView<FRecallTransformFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			const FRecallCinematicInternalFragment& CinematicInternalFragment = CinematicInternalList[EntityIndex];

			const TWeakObjectPtr<ALevelSequenceActor> LevelSequenceActor = ActorSystem.GetLevelSequenceActor(CinematicInternalFragment.CinematicActorHandle);
			if (!LevelSequenceActor.IsValid())
			{
				continue;
			}

			ULevelSequencePlayer* LevelSequencePlayer = LevelSequenceActor->GetSequencePlayer();
			if (ensure(LevelSequencePlayer))
			{
				const FRecallCinematicFragment& CinematicFragment = CinematicList[EntityIndex];

				if (!ShouldPlayCinematicRepresentation(LevelSequenceActor.Get(), CinematicFragment.FilteredPlayers))
				{
					if (LevelSequencePlayer->IsPlaying())
					{
						LevelSequencePlayer->Stop();
					}
					continue;
				}

				if (!LevelSequencePlayer->IsPlaying() && CinematicInternalFragment.NumLoopLeft != 0)
				{
					BindLevelSequence(LevelSequenceActor, CinematicFragment.Bindings, ActorSystem);

					if (CinematicInternalFragment.NumLoopLeft == 1)
					{
						LevelSequencePlayer->Play();
					}
					else
					{
						LevelSequencePlayer->PlayLooping(CinematicInternalFragment.NumLoopLeft);
					}
				}
			}

			// TODO: Restore time

			const FRecallTransformFragment& TransformFragment = TransformList[EntityIndex];
			LevelSequenceActor->SetActorLocationAndRotation(TransformFragment.Position, TransformFragment.Rotation);
		}
	});
}
