// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "RecallMovieSceneLabelSection.h"
#include "Evaluation/MovieSceneEvalTemplate.h"
#include "MovieSceneObjectBindingID.h"

#include "RecallMovieSceneLabelTemplate.generated.h"

class UMovieSceneEventTrack;
struct EventData;

USTRUCT()
struct FRecallMovieSceneLabelSectionTemplate : public FMovieSceneEvalTemplate
{
	GENERATED_BODY()
	
	FRecallMovieSceneLabelSectionTemplate() {}
	FRecallMovieSceneLabelSectionTemplate(const URecallMovieSceneLabelSection* InSection);

	UPROPERTY()
	const URecallMovieSceneLabelSection* Section = nullptr;

	virtual void Evaluate(const FMovieSceneEvaluationOperand& Operand, const FMovieSceneContext& Context, const FPersistentEvaluationData& PersistentData, FMovieSceneExecutionTokens& ExecutionTokens) const override;

private:

	virtual UScriptStruct& GetScriptStructImpl() const override { return *StaticStruct(); }
	virtual void EvaluateSwept(const FMovieSceneEvaluationOperand& Operand, const FMovieSceneContext& Context, const TRange<FFrameNumber>& SweptRange, const FPersistentEvaluationData& PersistentData, FMovieSceneExecutionTokens& ExecutionTokens) const override;
};
