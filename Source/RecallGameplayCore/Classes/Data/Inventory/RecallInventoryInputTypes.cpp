// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallInventoryInputTypes.h"

#include "Utility/Input/RecallInputUtils.h"

#define RECALL_INVENTORY_INPUT_TYPE_OPTION			FString("InventoryInputType")
#define RECALL_INVENTORY_INPUT_ITEM_TAG_OPTION		FString("InventoryInputItemTag")

FRecallInventoryInputCommand::FRecallInventoryInputCommand(const FString& Options)
{
	Type = Recall::Input::Utils::GetEnumOption<ERecallInventoryInputType>(
		Options, RECALL_INVENTORY_INPUT_TYPE_OPTION, ERecallInventoryInputType::None);

	const FString GameplayTagStr = Recall::Input::Utils::GetOption(
		Options, RECALL_INVENTORY_INPUT_ITEM_TAG_OPTION);
	if (!GameplayTagStr.IsEmpty())
	{
		ItemTag = FGameplayTag::RequestGameplayTag(*GameplayTagStr);
	}
}

FString FRecallInventoryInputCommand::ToOptions() const
{
	FString Options;

	Recall::Input::Utils::AddEnumOption<ERecallInventoryInputType>(RECALL_INVENTORY_INPUT_TYPE_OPTION, Options, Type);
	Recall::Input::Utils::AddOption(RECALL_INVENTORY_INPUT_ITEM_TAG_OPTION, Options, ItemTag.ToString());

	return Options;
}
