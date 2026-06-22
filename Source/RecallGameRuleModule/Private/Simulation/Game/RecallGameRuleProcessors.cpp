// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallGameRuleProcessors.h"

#include "Data/Game/RecallGameRuleAsset.h"
#include "Desync/RecallDesyncLog.h"
#include "Entity/RecallGameRuleSpawnCommand.h"
#include "Game/RecallGameRuleReactInterface.h"
#include "MassExecutionContext.h"
#include "System/Entity/RecallEntityAsyncSpawnSubsystem.h"
#include "System/Game/RecallGameRuleSubsystem.h"
#include "System/Representation/RecallRepresentationEventSubsystem.h"
#include "Utility/Game/RecallGameUtils.h"
#include "Utility/Simulation/RecallSimulationUtils.h"

//----------------------------------------------------------------------//
// URecallGameRuleProcessor
//----------------------------------------------------------------------//
URecallGameRuleProcessor::URecallGameRuleProcessor()
{
	ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::All);
	ProcessingPhase = EMassProcessingPhase::PrePhysics;
}

void URecallGameRuleProcessor::InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

bool URecallGameRuleProcessor::ShouldAllowQueryBasedPruning(const bool bRuntimeMode) const
{
	return false;
}

void URecallGameRuleProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	ProcessorRequirements.AddSubsystemRequirement<URecallGameRuleSubsystem>(EMassFragmentAccess::ReadWrite);
	ProcessorRequirements.AddSubsystemRequirement<URecallEntityAsyncSpawnSubsystem>(EMassFragmentAccess::ReadWrite);
	ProcessorRequirements.AddSubsystemRequirement<URecallRepresentationEventSubsystem>(EMassFragmentAccess::ReadWrite);
}

void URecallGameRuleProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	QUICK_SCOPE_CYCLE_COUNTER(Recall_GameRule_Execute);
	
	URecallGameRuleSubsystem& GameRuleSystem = Context.GetMutableSubsystemChecked<URecallGameRuleSubsystem>();

	// No game rule asset could be found, skip
	const URecallGameRuleAsset* GameRuleAsset = GameRuleSystem.GetGameRuleAsset<URecallGameRuleAsset>();
	if (!IsValid(GameRuleAsset))
	{
		return;
	}

	const double TimeSeconds = Recall::Simulation::Utils::GetTimeSeconds(Context.GetWorld());
	URecallRepresentationEventSubsystem& RepresentationEventSystem = Context.GetMutableSubsystemChecked<URecallRepresentationEventSubsystem>();
	
	// Game timer
	if (GameRuleSystem.IsInProgress() && GameRuleAsset->MatchDuration > 0.0f)
	{
		const double MatchRemainingTimeSeconds = (GameRuleAsset->PreMatchDuration + GameRuleAsset->MatchDuration) - TimeSeconds;
		if (MatchRemainingTimeSeconds > 0.0f)
		{
			RepresentationEventSystem.PushObserverEvent<IRecallGameRuleReactInterface>([MatchRemainingTimeSeconds](auto& Observer)
			{
				IRecallGameRuleReactInterface::Execute_SetMatchTimerRemainingTime(Observer.Object.Get(), MatchRemainingTimeSeconds);
			});
		}
		else
		{
			GameRuleSystem.SetMatchState(Recall::Game::State::WaitingPostMatch);
				
			RepresentationEventSystem.PushObserverEvent<IRecallGameRuleReactInterface>([](auto& Observer)
			{
				IRecallGameRuleReactInterface::Execute_OnMatchTimerEnd(Observer.Object.Get());
			});

			Recall::Game::Utils::EndGame(RepresentationEventSystem, GameRuleAsset->MatchTimerRunOutReason);
		}
	}
	
	// Already started the match
	if (!GameRuleSystem.IsWaitingToStart())
	{
		return;
	}
	
	const uint32 Frame = Recall::Simulation::Utils::GetFrame(Context.GetWorld());

	// Pre-match time is done, start the match
	if (TimeSeconds >= GameRuleAsset->PreMatchDuration)
	{
		GameRuleSystem.SetMatchState(Recall::Game::State::InProgress);

		URecallEntityAsyncSpawnSubsystem& EntityAsyncSpawnSystem = Context.GetMutableSubsystemChecked<URecallEntityAsyncSpawnSubsystem>();
		
		// Create spawn parameters with game rule spawn command
		FRecallEntityAsyncSpawnParameters SpawnParams;
		SpawnParams.EntityCount = 1;
		
		// Create and configure the spawn command
		SpawnParams.SpawnCommand.InitializeAs<FRecallGameRuleSpawnCommand>();
		
		// Spawn entity with parameters
		EntityAsyncSpawnSystem.SpawnEntityAsync(GameRuleAsset->GameRuleEntityConfig,
			FVector::ZeroVector, FQuat::Identity, SpawnParams);

#if RECALL_DESYNC_LOG
		RECALL_DESYNC_LOG_INT(Context.GetWorld(), "GameRule_EntitySpawn", SpawnParams.EntityCount);
#endif

		// Timer end event
		if (Frame > 0)
		{
			RepresentationEventSystem.PushObserverEvent<IRecallGameRuleReactInterface>([](auto& Observer)
			{
				IRecallGameRuleReactInterface::Execute_OnPreMatchTimerEnd(Observer.Object.Get());
			});

			if (GameRuleAsset->MatchDuration > 0.0f)
			{
				RepresentationEventSystem.PushObserverEvent<IRecallGameRuleReactInterface>([](auto& Observer)
				{
					IRecallGameRuleReactInterface::Execute_OnMatchTimerStart(Observer.Object.Get());
				});
			}
		}
	}
	else
	{
		if (Frame == 0)
		{
			// Timer start event
			RepresentationEventSystem.PushObserverEvent<IRecallGameRuleReactInterface>([](auto& Observer)
			{
				IRecallGameRuleReactInterface::Execute_OnPreMatchTimerStart(Observer.Object.Get());
			});
		}
		
		// Update timer
		const double PreMatchRemainingTimeSeconds = GameRuleAsset->PreMatchDuration - TimeSeconds;
		RepresentationEventSystem.PushObserverEvent<IRecallGameRuleReactInterface>([PreMatchRemainingTimeSeconds](auto& Observer)
		{
			IRecallGameRuleReactInterface::Execute_SetPreMatchTimerRemainingTime(Observer.Object.Get(), PreMatchRemainingTimeSeconds);
		});
	}
}
