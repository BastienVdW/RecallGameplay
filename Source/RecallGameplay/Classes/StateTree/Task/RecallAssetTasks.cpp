// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallAssetTasks.h"

#include "MassEntityView.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "System/Asset/RecallAssetManagerSubsystem.h"

//----------------------------------------------------------------------//
// FRecallJRPGLoadAssetTask
//----------------------------------------------------------------------//
bool FRecallJRPGLoadAssetTask::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(AssetManagerSystemHandle);
	return true;
}

EStateTreeRunStatus FRecallJRPGLoadAssetTask::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.bLoaded = false;

	if (AssetPath.IsNull())
	{
		return EStateTreeRunStatus::Failed;
	}

	URecallAssetManagerSubsystem& AssetManagerSystem = Context.GetExternalData(AssetManagerSystemHandle);
	InstanceData.AssetHandle = AssetManagerSystem.RequestAsset(AssetPath);

	return Super::EnterState(Context, Transition);
}

void FRecallJRPGLoadAssetTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	URecallAssetManagerSubsystem& AssetManagerSystem = Context.GetExternalData(AssetManagerSystemHandle);
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	AssetManagerSystem.ReleaseAsset(InstanceData.AssetHandle);

	return Super::ExitState(Context, Transition);
}

EStateTreeRunStatus FRecallJRPGLoadAssetTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.bLoaded)
	{
		const URecallAssetManagerSubsystem& AssetManagerSystem = Context.GetExternalData(AssetManagerSystemHandle);
		if (AssetManagerSystem.IsAssetLoaded(InstanceData.AssetHandle))
		{
			InstanceData.bLoaded = true;
		}
	}

	return Super::Tick(Context, DeltaTime);
}
