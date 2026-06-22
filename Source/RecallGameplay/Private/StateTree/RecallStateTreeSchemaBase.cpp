// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "StateTree/RecallStateTreeSchemaBase.h"

#include "MassExtendedEntityTypes.h"
#include "Simulation/StateTree/RecallStateTreeFragments.h"
#include "Simulation/Transform/RecallTransformFragments.h"
#include "StateTree/RecallStateTreeConditionBase.h"
#include "StateTree/RecallStateTreeEvaluatorBase.h"
#include "StateTree/RecallStateTreeTaskBase.h"
#include "Subsystems/WorldSubsystem.h"

URecallStateTreeSchemaBase::URecallStateTreeSchemaBase()
	: Super()
{
	LinkContextData(StateTreeInstanceFragment,		FName("Instance Data"),		FGuid(TEXT("7B193872-4106-207A-D92A-07B8AD5A3E8B")));
	LinkContextData(TransformFragment,				FName("Transform Data"),	FGuid(TEXT("F3F9B330-4779-30BD-5D6E-7496EE1FB3EB")));
}

bool URecallStateTreeSchemaBase::IsStructAllowed(const UScriptStruct* InScriptStruct) const
{
	// Only allow Recall evals and tasks,and common conditions.
	return InScriptStruct->IsChildOf(FStateTreeConditionCommonBase::StaticStruct())
		|| InScriptStruct->IsChildOf(FRecallStateTreeConditionBase::StaticStruct())
		|| InScriptStruct->IsChildOf(FRecallStateTreeEvaluatorBase::StaticStruct())
		|| InScriptStruct->IsChildOf(FRecallStateTreeTaskBase::StaticStruct());
}

bool URecallStateTreeSchemaBase::IsExternalItemAllowed(const UStruct& InStruct) const
{
	// Allow only WorldSubsystems and fragments as external data.
	return InStruct.IsChildOf(UWorldSubsystem::StaticClass())
		|| InStruct.IsChildOf(FMassExtendedFragment::StaticStruct())
		|| InStruct.IsChildOf(FMassExtendedSharedFragment::StaticStruct())
		|| InStruct.IsChildOf(FMassExtendedConstSharedFragment::StaticStruct());
}

TConstArrayView<FStateTreeExternalDataDesc> URecallStateTreeSchemaBase::GetContextDataDescs() const
{
	return ContextDataDescs;
}
