// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallInteractCommandTypes.h"

DEFINE_LOG_CATEGORY(LogRecallInteractCommand);

void FRecallInteractCommand::Execute(const FRecallInteractContext& Context,
	ERecallInteractCommandExecuteGroup Group) const
{
	auto TryTriggerExecute = [this, Group](const FRecallInteractContext& Context)
	{
		if (EnumHasAnyFlags(ExecuteGroup, Group))
		{
			OnExecute(Context);
		}
	};
	
	switch (Group)
	{
	case ERecallInteractCommandExecuteGroup::OnTick:
	case ERecallInteractCommandExecuteGroup::OnComplete:
		{
			TryTriggerExecute(Context);
		}
		break;
		
	case ERecallInteractCommandExecuteGroup::OnBegin:
		{
			OnBegin(Context);
			TryTriggerExecute(Context);
		}
		break;
		
	case ERecallInteractCommandExecuteGroup::OnEnd:
		{
			TryTriggerExecute(Context);
			OnEnd(Context);
		}
		break;
		
	default:
		unimplemented();
		break;
	}
}
