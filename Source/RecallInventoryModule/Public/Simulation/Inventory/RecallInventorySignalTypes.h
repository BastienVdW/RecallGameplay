// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"

namespace Recall::Inventory::Signals
{

    namespace Callback
    {

    	/**
    	 * Callback when this entity loot an item.
    	 */
	    const FName OnLoot				= FName(TEXT("RecallInventoryOnLoot"));
    
    	/**
		 * Callback when an item is equipped.
		 */
    	const FName OnEquip				= FName(TEXT("RecallInventoryOnEquip"));
    	
    	/**
		 * Callback when an item is unequipped.
		 */
    	const FName OnUnequip				= FName(TEXT("RecallInventoryOnUnequip"));
    	
    } // namespace Callback

} // namespace Recall::Inventory::Signals
