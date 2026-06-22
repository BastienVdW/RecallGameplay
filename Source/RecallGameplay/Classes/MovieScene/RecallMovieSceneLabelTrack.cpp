// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#include "RecallMovieSceneLabelTrack.h"

#include "MovieSceneCommonHelpers.h"
#include "RecallMovieSceneLabelTemplate.h"
#include "RecallMovieSceneLabelSection.h"
#include "Evaluation/MovieSceneEvaluationTrack.h"
#include "Compilation/IMovieSceneTemplateGenerator.h"
#include "IMovieSceneTracksModule.h"
#include "Evaluation/MovieSceneEvaluationCustomVersion.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RecallMovieSceneLabelTrack)

#define LOCTEXT_NAMESPACE "RecallMovieSceneLabelTrack"

#if WITH_EDITORONLY_DATA
FText URecallMovieSceneLabelTrack::GetDisplayName() const
{
	static FText TrackName = FText::FromString(FString(TEXT("Label")));
	return TrackName;
}
#endif // WITH_EDITORONLY_DATA

FName URecallMovieSceneLabelTrack::GetTrackName() const
{
	return URecallMovieSceneLabelTrack::GetDataTrackName();
}


const FName URecallMovieSceneLabelTrack::GetDataTrackName()
{
	static FName TheName = FName(*FString(TEXT("LabelDataTrack")));
	return TheName;
}


bool URecallMovieSceneLabelTrack::IsEmpty() const
{
	return Sections.Num() == 0;
}


UMovieSceneSection* URecallMovieSceneLabelTrack::CreateNewSection()
{
	UMovieSceneSection* Section = NewObject<URecallMovieSceneLabelSection>(this, NAME_None, RF_Transactional);
	Sections.Add(Section);
	return Section;
}


void URecallMovieSceneLabelTrack::AddSection(UMovieSceneSection& Section)
{
	Sections.Add(&Section);
}


void URecallMovieSceneLabelTrack::RemoveSection(UMovieSceneSection& Section)
{
	Sections.Remove(&Section);
}


const TArray<UMovieSceneSection*>& URecallMovieSceneLabelTrack::GetAllSections() const
{
	return Sections;
}

#undef LOCTEXT_NAMESPACE

