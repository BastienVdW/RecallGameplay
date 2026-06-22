// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "StateTree/RecallGameRuleStateTreeSchema.h"

#include "MassEntityElementTypes.h"
#include "Simulation/StateTree/RecallStateTreeFragments.h"
#include "StateTree/RecallGameRuleStateTreeConditionBase.h"
#include "StateTree/RecallGameRuleStateTreeEvaluatorBase.h"
#include "StateTree/RecallGameRuleStateTreeTaskBase.h"
#include "StateTree/Task/RecallCommonTasks.h"
#include "Subsystems/WorldSubsystem.h"

URecallGameRuleStateTreeSchema::URecallGameRuleStateTreeSchema()
	: Super()
{
	LinkContextData(StateTreeInstanceFragment,		FName("Instance Data"),		FGuid(TEXT("7B193872-4106-207A-D92A-07B8AD5A3E8B")));
}

bool URecallGameRuleStateTreeSchema::IsStructAllowed(const UScriptStruct* InScriptStruct) const
{
	// Only allow Recall evals and tasks,and common conditions.
	return IsStructChildOf<FStateTreeConditionCommonBase>(InScriptStruct)
		|| IsStructChildOf<FRecallGameRuleStateTreeTaskBase>(InScriptStruct)
		|| IsStructChildOf<FRecallGameRuleStateTreeEvaluatorBase>(InScriptStruct)
		|| IsStructChildOf<FRecallGameRuleStateTreeConditionBase>(InScriptStruct)
		|| IsStructChildOf<FRecallDelayTask>(InScriptStruct, false);
}

bool URecallGameRuleStateTreeSchema::IsExternalItemAllowed(const UStruct& InStruct) const
{
	// Allow only WorldSubsystems and fragments as external data.
	return InStruct.IsChildOf(UWorldSubsystem::StaticClass())
		|| InStruct.IsChildOf(FMassFragment::StaticStruct())
		|| InStruct.IsChildOf(FMassSharedFragment::StaticStruct())
		|| InStruct.IsChildOf(FMassConstSharedFragment::StaticStruct());
}

TConstArrayView<FStateTreeExternalDataDesc> URecallGameRuleStateTreeSchema::GetContextDataDescs() const
{
	return ContextDataDescs;
}
