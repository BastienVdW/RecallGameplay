// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallGameRuleTimerWidget.h"

#include "Components/ProgressBar.h"
#include "RecallFrontendUtils.h"
#include "Utility/Game/RecallGameUtils.h"
#include "Utility/GameRule/RecallGameRuleUtils.h"

void URecallGameRuleTimerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Recall::Frontend::Utils::RegisterGlobalObserver<IRecallRepresentationReactInterface>(this);
}

void URecallGameRuleTimerWidget::NativeDestruct()
{
	Super::NativeDestruct();

	Recall::Frontend::Utils::UnregisterAllGlobalObservers(this);
}

void URecallGameRuleTimerWidget::OnRender()
{
	UpdateMatchTimer();
}

void URecallGameRuleTimerWidget::UpdateMatchTimer()
{
	const float CurrentMatchTime = Recall::Game::Utils::GetMatchTimeSeconds(this);
	const float MatchDuration = Recall::GameRule::Utils::GetMatchDuration(this);
	
	if (CachedMatchTimeSeconds != CurrentMatchTime || CachedMatchDuration != MatchDuration)
	{
		CachedMatchTimeSeconds = CurrentMatchTime;
		CachedMatchDuration = MatchDuration;
		
		if (ProgressBar_MatchTimer && MatchDuration > 0.0f)
		{
			float Progress = FMath::Clamp(CachedMatchTimeSeconds / CachedMatchDuration, 0.0f, 1.0f);
			
			// Invert progress if counting down (default behavior)
			if (bProgressBarCountsDown)
			{
				Progress = 1.0f - Progress;
			}
			
			ProgressBar_MatchTimer->SetPercent(Progress);
		}
	}
}
