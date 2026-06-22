// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallInteractAttributeConditionTypes.h"

#include "MassExtendedEntityView.h"
#include "Simulation/Attribute/RecallAttributeFragments.h"
#include "Utility/Attribute/RecallAttributeUtils.h"

bool FRecallInteractAttributeCondition::EvaluateCondition(const FRecallInteractContext& Context, FText& OutFailedText) const
{
	const FMassExtendedEntityHandle TargetEntity = Context.GetTargetEntity(Target);
	const FMassExtendedEntityView TargetView(Context.GetEntityManagerChecked(), TargetEntity);
	const FRecallAttributeFragment* TargetAttributeFragmentPtr = TargetView.GetFragmentDataPtr<FRecallAttributeFragment>();
	if (TargetAttributeFragmentPtr == nullptr)
	{
		return false;
	}
	
	return Recall::Attribute::Utils::EvaluateCondition(Condition, TargetAttributeFragmentPtr->AttributeSet);
}
