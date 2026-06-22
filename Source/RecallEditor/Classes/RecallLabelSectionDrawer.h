// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "ISequencerSection.h"
#include "MovieScene/RecallMovieSceneLabelSection.h"
#include "SequencerSectionPainter.h"

class RECALLEDITOR_API FRecallLabelDataSectionDrawer : public  ISequencerSection
{
public:
	FRecallLabelDataSectionDrawer(UMovieSceneSection& InSection, TWeakPtr<ISequencer> InSequencer)
	{
		LabelDataSection = Cast<URecallMovieSceneLabelSection>(&InSection);
	}

	virtual int32 OnPaintSection(FSequencerSectionPainter& InPainter) const override
	{
		return InPainter.PaintSectionBackground();
	}

	virtual UMovieSceneSection* GetSectionObject() override
	{
		return LabelDataSection;
	}

	virtual FText GetSectionTitle() const override
	{
		if (LabelDataSection->LabelTrackData.Label.IsNone())
		{
			return FText::FromString(FString(TEXT("Label Data Section")));
		}
		else
		{
			return FText::FromName(LabelDataSection->LabelTrackData.Label);
		}
	}

	virtual float GetSectionHeight() const override
	{
		return 40.0f;
	}

	class URecallMovieSceneLabelSection* LabelDataSection;
};
