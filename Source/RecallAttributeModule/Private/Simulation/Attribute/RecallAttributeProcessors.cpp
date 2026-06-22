// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallAttributeProcessors.h"

#include "MassExecutionContext.h"
#include "Attribute/RecallAttributeDefinitionTypes.h"
#include "Simulation/Attribute/RecallAttributeFragments.h"

//----------------------------------------------------------------------//
// URecallAttributeInitializer
//----------------------------------------------------------------------//
URecallAttributeInitializer::URecallAttributeInitializer()
	: EntityQuery(*this)
{
	ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::All);
	ObservedType = FRecallAttributeFragment::StaticStruct();
	Operation = EMassObservedOperation::Add;
}

void URecallAttributeInitializer::InitializeInternal(UObject& Owner, const TSharedRef<FMassEntityManager>& InEntityManager)
{
	Super::InitializeInternal(Owner, InEntityManager);
}

void URecallAttributeInitializer::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FRecallAttributeFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddConstSharedRequirement<FRecallAttributeConstSharedFragment>();
}

void URecallAttributeInitializer::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(Context, [](FMassExecutionContext& Context)
	{
		const auto& AttributeConstSharedFragment = Context.GetConstSharedFragment<FRecallAttributeConstSharedFragment>();
		if (!AttributeConstSharedFragment.AttributeSet)
		{
			return;
		}
		
		const TArrayView<FRecallAttributeFragment> AttributeList = Context.GetMutableFragmentView<FRecallAttributeFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); EntityIndex++)
		{
			const FMassEntityHandle Entity = Context.GetEntity(EntityIndex);
			
			FRecallAttributeFragment& AttributeFragment = AttributeList[EntityIndex];
			AttributeFragment.AttributeSet = FRecallAttributeSet(AttributeConstSharedFragment.AttributeSet);
		}
	});
}
