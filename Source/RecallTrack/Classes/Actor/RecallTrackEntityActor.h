// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Actor/RecallEntityActor.h"
#include "Track/RecallTrackTypes.h"

#include "RecallTrackEntityActor.generated.h"

class USplineComponent;

UENUM()
enum class ERecallTrackColliderMeshMode : uint8
{
	/**
	 * Generate smooth box along the spline.
	 */
	Box,

	/**
	 * Deform the mesh along the spline.
	 */
	Mesh,
};

/*
* Actor to generate a tack entity.
* The spline mesh and the landscape are made using Unreal component system for the representation.
* They will then be converted into Entity for the ECS collision system.
*/
UCLASS(Abstract, Blueprintable, DisplayName="RE Track Entity Actor", ComponentWrapperClass)
class RECALLTRACK_API ARecallTrackEntityActor : public ARecallEntityActor
{
	GENERATED_UCLASS_BODY()

	//~ Begin AActor Interface
public:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PostRegisterAllComponents() override;
	//~ End AActor Interface
	
	//~ Begin UObject Interface
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR
	//~ Begin UObject Interface

public:
	const TArray<FRecallTrackSegment>& GetTrackSegments() const { return TrackSegments; }
	const USplineComponent* GetSplineComponent() const { return SplineComponent; }
	float GetTrackWidthAtPosition(const FVector& Position) const;
	float GetTrackOffsetAtPosition(const FVector& Position) const;
	float GetNormalizedTrackOffsetAtPosition(const FVector& Position) const;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TObjectPtr<USplineComponent> SplineComponent;

	/**
	 * Mesh to use for the spline mesh.
	 */
	UPROPERTY(EditAnywhere, Category="Track | Mesh")
	TObjectPtr<UStaticMesh> TrackStaticMesh;

	/**
	 * Automatically generate the spline mesh each time the spline is modified.
	 */
	UPROPERTY(EditAnywhere, Category="Track | Mesh")
	bool bAutoGenerateSplineMesh = true;

	/**
	 * Define the way the collider mesh will be generated along the spline.
	 */
	UPROPERTY(EditAnywhere, Category="Track|Collider")
	ERecallTrackColliderMeshMode TrackColliderMeshMode = ERecallTrackColliderMeshMode::Box;
	
	/**
	 * Distance between collider subdvision of the track.
	 */
	UPROPERTY(EditAnywhere, Category="Track|Collider", meta=(Units=Centimeters, EditCondition="TrackColliderMeshMode == ERecallTrackColliderMeshMode::Box", EditConditionHides))
	float TrackColliderSubdivisionDistance = 100.0f;

	/**
	 * How long each mesh of the track should be at most.
	 */
	UPROPERTY(EditAnywhere, Category="Track|Collider", meta=(Units=Centimeters, EditCondition="TrackColliderMeshMode == ERecallTrackColliderMeshMode::Box", EditConditionHides))
	float TrackColliderMeshLength = 10000.0f;
	
	/**
	 * Offset collider of the track to better match the mesh.
	 */
	UPROPERTY(EditAnywhere, Category="Track|Collider", meta=(Units=Centimeters, EditCondition="TrackColliderMeshMode == ERecallTrackColliderMeshMode::Box", EditConditionHides))
	FVector TrackColliderOffset = FVector::ZeroVector;
	
	/**
	 * The track collider will generate walls around the whole track.
	 * If false, then only the track itself will have a collider.
	 */
	UPROPERTY(EditAnywhere, Category="Track|Collider", meta=(EditCondition="TrackColliderMeshMode == ERecallTrackColliderMeshMode::Box", EditConditionHides))
	bool bWallAroundTrack = true;
	
	/**
	 * Automatically generate the landscape each time the spline is moved.
	 */
	UPROPERTY(EditAnywhere, Category="Track|Landscape")
	bool bAutoCarveLandscape = true;

	/**
	 * Height offset around our track when blending with the landscape.
	 */
	UPROPERTY(EditAnywhere, Category="Track|Landscape")
	float MountainLandscapeTrackOffset = 8.0f;

	/**
	 * How far it should start blending with the landscape on the edges of the track.
	 */
	UPROPERTY(EditAnywhere, Category="Track|Landscape")
	float MountainLandscapeEdgeBlendDistance = 300.0f;
	
	/**
	 * How far it should start blending with the landscape along the track.
	 */
	UPROPERTY(EditAnywhere, Category="Track|Landscape")
	float MountainLandscapeBlendDistance = 20.0f;
	
	/**
	 * Create a mountain around the track.
	*/
	UPROPERTY(EditAnywhere, Category="Track|Landscape")
	bool bGenerateMountainAroundTrack = true;

	/**
	 * How large should the mountain be around the track.
	 */
	UPROPERTY(EditAnywhere, Category="Track|Landscape", meta=(EditCondition="bGenerateMountainAroundTrack", Units=Centimeters, ClampMin="100.0"))
	float MountainRadius = 10000.0f;
	
	UPROPERTY(EditAnywhere, Category="Track|Landscape", meta=(EditCondition="bGenerateMountainAroundTrack"))
	TObjectPtr<UCurveFloat> MountainCurve;
	
protected:
	/**
	 * Generate the spline mesh along the spline.
	 */
	UFUNCTION(CallInEditor, Category="Track|Mesh")
	void GenerateSplineMesh();

	/**
	 * Generate the landscape around the track.
	 */
	UFUNCTION(CallInEditor, Category="Track|Landscape")
	void ApplyToLandscape() const;

	float EvaluateLandscapeHeight(const FVector& Position) const;
	
#if WITH_EDITORONLY_DATA
	FTimerHandle CarveLandscapeTimerHandle;
	bool bDirtySpline = false;
#endif // WITH_EDITORONLY_DATA
	
	UPROPERTY()
	TArray<TObjectPtr<class USplineMeshComponent>> SplineMeshComponents;
	UPROPERTY()
	TArray<FRecallTrackSegment> TrackSegments;
	UPROPERTY()
	FTransform OldTrackTransform = FTransform::Identity;
	
	/**
	 * Generate the collider mesh segments of the track.
	 */
	TArray<FRecallTrackSegment> GenerateTrackColliderMeshSegments() const;
};
