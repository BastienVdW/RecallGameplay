// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "MovieSceneNameableTrack.h"
#include "Tracks/MovieSceneSpawnTrack.h"
#include "MovieSceneObjectBindingID.h"
#include "Compilation/IMovieSceneTrackTemplateProducer.h"
#include "Compilation/IMovieSceneDeterminismSource.h"
#include "RecallMovieSceneLabelTrack.generated.h"

UCLASS(MinimalAPI)
class URecallMovieSceneLabelTrack : public UMovieSceneTrack
{
	GENERATED_BODY()

public:
#if WITH_EDITORONLY_DATA
	virtual FText GetDisplayName() const override;
#endif // WITH_EDITORONLY_DATA
	
	virtual FName GetTrackName() const override;

	virtual bool IsEmpty() const override;


	//Basic section operations: 
	virtual class UMovieSceneSection* CreateNewSection() override;
	virtual void AddSection(class UMovieSceneSection& Section) override;
	virtual void RemoveSection(UMovieSceneSection& Section) override;


	virtual const TArray<UMovieSceneSection*>& GetAllSections() const override;


	static const FName GetDataTrackName();

	UPROPERTY()
	TArray<UMovieSceneSection*> Sections;
};
