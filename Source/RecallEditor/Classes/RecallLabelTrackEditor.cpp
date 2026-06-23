// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallLabelTrackEditor.h"

#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "GameFramework/Actor.h"
#include "Styling/AppStyle.h"
#include "UObject/Package.h"
#include "ISequencerSection.h"
#include "IDetailCustomization.h"
#include "PropertyEditorModule.h"
#include "Modules/ModuleManager.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "IDetailPropertyRow.h"
#include "Widgets/Layout/SBox.h"
#include "RecallLabelSectionDrawer.h"
#include "MovieScene/RecallMovieSceneLabelSection.h"
#include "MovieScene/RecallMovieSceneLabelTrack.h"
#include "MVVM/Views/ViewUtilities.h"
#include "MovieSceneSequenceEditor.h"

#define LOCTEXT_NAMESPACE "RecallLabelTrackEditor"

FRecallLabelTrackEditor::FRecallLabelTrackEditor(TSharedRef<ISequencer>InSequencer)
	: FMovieSceneTrackEditor(InSequencer)
{
}

TSharedRef<ISequencerTrackEditor> FRecallLabelTrackEditor::CreateTrackEditor(TSharedRef<ISequencer> InSequencer)
{
	return MakeShareable(new FRecallLabelTrackEditor(InSequencer));
}

//Please take a note that on Button click we are creating our Track and its section.
void FRecallLabelTrackEditor::BuildAddTrackMenu(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(
		LOCTEXT("AddLabelTrack", "Label Track"), //Label
		LOCTEXT("AddLabelTrack", "Label Track"), //Tooltip
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "ClassIcon.LevelInstance"),
		FUIAction(FExecuteAction::CreateLambda([FocusedMovieScene = GetFocusedMovieScene(), Sequencer = GetSequencer()]
			{
				if (IsValid(FocusedMovieScene))
				{
					//Create scoped transaction:
					const FScopedTransaction Transaction(LOCTEXT("FRecallLabelTrackEditor_Transaction", "Add Label Data Track"));
					FocusedMovieScene->Modify();

					URecallMovieSceneLabelTrack* NewTrack = FocusedMovieScene->AddTrack<URecallMovieSceneLabelTrack>();
					FRecallLabelTrackEditor::AddNewTrack(NewTrack, FocusedMovieScene);

					//Notify SequencerAboutTransactions:
					Sequencer->NotifyMovieSceneDataChanged(EMovieSceneDataChangeType::MovieSceneStructureItemAdded);
				}
			})
		)
	);
}

bool FRecallLabelTrackEditor::SupportsType(TSubclassOf<UMovieSceneTrack> Type) const
{
	return URecallMovieSceneLabelTrack::StaticClass() == Type;
}


bool FRecallLabelTrackEditor::SupportsSequence(UMovieSceneSequence* InSequence) const
{
	if (!IsValid(InSequence))
	{
		return false;
	}


	static UClass* LevelSequencerClass = FindObject<UClass>(nullptr, TEXT("/Script/LevelSequence.LevelSequence"), EFindObjectFlags::ExactClass);
	bool bValidClasses = true;


	bValidClasses &= (LevelSequencerClass != nullptr);
	bValidClasses &= InSequence->GetClass()->IsChildOf(LevelSequencerClass);


	return bValidClasses;
}


FReply FRecallLabelTrackEditor::AddNewTrack(UMovieSceneTrack* Track, UMovieScene* FocusedMovieScene)
{
	if (IsValid(FocusedMovieScene))
	{
		URecallMovieSceneLabelTrack* CustomTrack = Cast<URecallMovieSceneLabelTrack>(Track);
		if (IsValid(CustomTrack))
		{
			// Transactions mark on track
			Track->Modify();

			TRange<FFrameNumber> SectionRange = FocusedMovieScene->GetPlaybackRange();
			UMovieSceneSection* NewSection = Track->CreateNewSection();
			URecallMovieSceneLabelSection* DataSection = Cast<URecallMovieSceneLabelSection>(NewSection);

			if (IsValid(DataSection))
			{
				NewSection->SetRange(SectionRange);
			}
		}
	}


	return FReply::Handled();
}


TSharedRef<ISequencerSection> FRecallLabelTrackEditor::MakeSectionInterface(UMovieSceneSection& SectionObject, UMovieSceneTrack& Track, FGuid ObjectBinding)
{
	return MakeShareable(new FRecallLabelDataSectionDrawer(SectionObject, GetSequencer()));
}

TSharedPtr<SWidget> FRecallLabelTrackEditor::BuildOutlinerEditWidget(const FGuid& ObjectBinding, UMovieSceneTrack* Track, const FBuildEditWidgetParams& Params)
{
	return nullptr;
}

#undef LOCTEXT_NAMESPACE
