// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallNavLinkProxy.h"

#include "Traversal/RecallNavLinkJumpTraversal.h"

URecallNavLinkProxy::URecallNavLinkProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NavLinkTraversal = FInstancedStruct::Make<FRecallNavLinkJumpTraversal>();
}

bool URecallNavLinkProxy::OnLinkMoveStarted(class UObject* PathComp, const FVector& DestPoint)
{
	return true;
}
