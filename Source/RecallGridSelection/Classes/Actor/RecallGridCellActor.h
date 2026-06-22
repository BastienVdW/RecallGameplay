// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "GameFramework/Actor.h"

#include "RecallGridCellActor.generated.h"

/**
 * Selectable cell on a grid.
 */
UCLASS(Abstract, Blueprintable, DisplayName="RE Grid Cell Actor")
class RECALLGRIDSELECTION_API ARecallGridCellActor : public AActor
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category=Cell)
	void SetCellCoordinates(int32 PosX, int32 PosY);
	
	UFUNCTION(BlueprintNativeEvent, Category=Cell)
	void SetCellSize(float SizeX, float SizeY);

	//~ Begin AActor Interface
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End AActor Interface

protected:
	UPROPERTY(EditDefaultsOnly, Category=Cell)
	float CellHalfHeight = 50.0f;
	
	UPROPERTY(VisibleAnywhere, Category=Cell, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TObjectPtr<class UBoxComponent> BoxComponent;

	UPROPERTY(Transient, BlueprintReadOnly, Category=Cell)
	FIntVector CellCoordinates = FIntVector::ZeroValue;

	UFUNCTION()
	void OnBeginCursorOverEvent(AActor* TouchedActor);

	UFUNCTION()
	void OnEndCursorOverEvent(AActor* TouchedActor);
};
