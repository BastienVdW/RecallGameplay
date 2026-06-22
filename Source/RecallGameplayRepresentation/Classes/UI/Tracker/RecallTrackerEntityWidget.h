// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "ExtendedCommonUserWidget.h"
#include "Representation/Tracker/RecallTrackerReactInterface.h"
#include "Mass/EntityElementTypes.h"

#include "RecallTrackerEntityWidget.generated.h"

USTRUCT()
struct FRecallTrackerEntityWidgetPool
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	TArray<TObjectPtr<UUserWidget>> Widgets;
};

/**
 * Widget to track an entity and display info about it.
 */
UCLASS(Abstract)
class RECALLGAMEPLAYREPRESENTATION_API URecallTrackerEntityWidget :
	public UExtendedCommonUserWidget,
	public IRecallTrackerReactInterface
{
	GENERATED_BODY()

	// UUserWidget implementation Begin
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	// UUserWidget implementation End
	
	// IRecallTrackerReactInterface implementation Begin
public:
	virtual void SetTrackedEntities(
		const TArray<FRecallTrackedEntityRepresentation>& TrackedEntityRepresentations) override;
	// IRecallTrackerReactInterface implementation End

protected:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<class UCanvasPanel> CanvasPanel_Track;

protected:
	UPROPERTY(Transient)
	TMap<FMassEntityHandle, TObjectPtr<UUserWidget>> ActiveTrackerWidgets;
	UPROPERTY(Transient)
	TMap<TSubclassOf<UUserWidget>, FRecallTrackerEntityWidgetPool> PoolTrackerWidgets;

	TArray<FRecallTrackedEntityRepresentation> CacheNewTrackedEntityRepresentations;
	TSet<FMassEntityHandle> CacheNewTrackedEntities;
	TArray<FMassEntityHandle> CacheActiveTrackedEntities;
	TArray<FMassEntityHandle> CacheReleaseEntities;
	
	void CreateTrackers(const TArray<FRecallTrackedEntityRepresentation>& NewTrackedEntities);
	void ReleaseTrackers(const TArray<FMassEntityHandle>& TrackedEntities);
	void UpdateTrackersPosition(const TArray<FRecallTrackedEntityRepresentation>& TrackedEntities);
};
