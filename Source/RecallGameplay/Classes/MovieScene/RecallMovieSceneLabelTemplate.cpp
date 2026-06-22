// Copyright Epic Games, Inc. All Rights Reserved.

#include "RecallMovieSceneLabelTemplate.h"

#include "RecallMovieSceneLabelTrack.h"
#include "MovieSceneSequence.h"
#include "Evaluation/MovieSceneEvaluationTemplateInstance.h"
#include "EngineGlobals.h"
#include "MovieScene.h"
#include "Evaluation/MovieSceneEvaluation.h"
#include "IMovieScenePlayer.h"
#include "Algo/Accumulate.h"
#include "Engine/LevelScriptActor.h"
#include "Logging/TokenizedMessage.h"
#include "Logging/MessageLog.h"
#include "Misc/UObjectToken.h"

#define LOCTEXT_NAMESPACE "RecallMovieSceneLabelTemplate"

FRecallMovieSceneLabelSectionTemplate::FRecallMovieSceneLabelSectionTemplate(const URecallMovieSceneLabelSection* InSection)
{
	Section = InSection;
}

void FRecallMovieSceneLabelSectionTemplate::Evaluate(const FMovieSceneEvaluationOperand& Operand, const FMovieSceneContext& Context, const FPersistentEvaluationData& PersistentData, FMovieSceneExecutionTokens& ExecutionTokens) const
{
}

void FRecallMovieSceneLabelSectionTemplate::EvaluateSwept(const FMovieSceneEvaluationOperand& Operand, const FMovieSceneContext& Context, const TRange<FFrameNumber>& SweptRange, const FPersistentEvaluationData& PersistentData, FMovieSceneExecutionTokens& ExecutionTokens) const
{
}

#undef LOCTEXT_NAMESPACE
