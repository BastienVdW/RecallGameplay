// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "RecallTrackEntityActor.h"

#include "Component/RecallEntityComponent.h"
#include "Components/SplineComponent.h"
#include "Landscape.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "System/Track/RecallTrackSubsystem.h"
#include "Utility/Physics/RecallPhysicsUtils.h"

#if WITH_EDITOR
#include "Async/ParallelFor.h"
#include "LandscapeEdit.h"
#include "LandscapeEditorUtils.h"
#endif // WITH_EDITOR

#define LOCTEXT_NAMESPACE "RecallTrack"

ARecallTrackEntityActor::ARecallTrackEntityActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bHideActorInGameOnBeginPlay = false;

	if (EntityComponent)
	{
		EntityComponent->SetMutableEntity(false);
	}
	
	SplineComponent = ObjectInitializer.CreateDefaultSubobject<USplineComponent>(this, TEXT("SplineComponent"));
	if (SplineComponent)
	{
		SplineComponent->SetupAttachment(RootComponent);
		SplineComponent->SetMobility(EComponentMobility::Static);
		SplineComponent->ReparamStepsPerSegment = 50;
	}
}

void ARecallTrackEntityActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

#if WITH_EDITOR
	const UWorld* World = GetWorld();
	if (World == nullptr ||
		(!World->IsPreviewWorld() &&
			(World->IsEditorWorld() == false || World->IsGameWorld())
			)
		)
	{
		return;
	}
	
	bDirtySpline = false;
	
	if (bAutoGenerateSplineMesh)
	{
		GenerateSplineMesh();

		if (bDirtySpline)
		{
			Modify();
		}
	}

	if (bAutoCarveLandscape && (bDirtySpline || !OldTrackTransform.Equals(Transform)))
	{
#if WITH_EDITORONLY_DATA
		World->GetTimerManager().SetTimer(CarveLandscapeTimerHandle, this, &ThisClass::ApplyToLandscape, 0.5f);
#endif // WITH_EDITORONLY_DATA
	}
	
	OldTrackTransform = Transform;
#endif // WITH_EDITOR
}

void ARecallTrackEntityActor::BeginPlay()
{
	Super::BeginPlay();

	if (URecallTrackSubsystem* TrackSubsystem = UWorld::GetSubsystem<URecallTrackSubsystem>(GetWorld()))
	{
		TrackSubsystem->RegisterTrackActor(this);
	}
}

void ARecallTrackEntityActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (URecallTrackSubsystem* TrackSubsystem = UWorld::GetSubsystem<URecallTrackSubsystem>(GetWorld()))
	{
		TrackSubsystem->UnregisterTrackActor(this);
	}
}

void ARecallTrackEntityActor::PostRegisterAllComponents()
{
	Super::PostRegisterAllComponents();
}

#if WITH_EDITOR
void ARecallTrackEntityActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif // WITH_EDITOR

float ARecallTrackEntityActor::GetTrackWidthAtPosition(const FVector& Position) const
{
	if (!SplineComponent || !TrackStaticMesh)
	{
		return 0.0f;
	}

	const FBox TrackMeshBoundingBox = TrackStaticMesh->GetBoundingBox();
	const FVector TrackMeshExtents = TrackMeshBoundingBox.GetExtent();

	const float Distance = SplineComponent->GetDistanceAlongSplineAtLocation(Position, ESplineCoordinateSpace::World);
	const FTransform SplineTransform = SplineComponent->GetTransformAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World, true);

	return TrackMeshExtents.Y * SplineTransform.GetScale3D().Y;
}

float ARecallTrackEntityActor::GetTrackOffsetAtPosition(const FVector& Position) const
{
	if (!SplineComponent)
	{
		return 0.5f;
	}

	const float Distance = SplineComponent->GetDistanceAlongSplineAtLocation(Position, ESplineCoordinateSpace::World);
	const FTransform SplineTransform = SplineComponent->GetTransformAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World, true);

	const float Offset = FVector::DotProduct(Position - SplineTransform.GetLocation(), SplineTransform.GetRotation().GetRightVector());
	return Offset;
}

float ARecallTrackEntityActor::GetNormalizedTrackOffsetAtPosition(const FVector& Position) const
{
	const float TrackWidth = GetTrackWidthAtPosition(Position);
	if (TrackWidth == 0.0f)
	{
		return 0.5f;
	}	

	const float Offset = GetTrackOffsetAtPosition(Position);
	return FMath::Clamp((Offset + TrackWidth * 0.5f) / TrackWidth, 0.0f, 1.0f);
}

TArray<FRecallTrackSegment> ARecallTrackEntityActor::GenerateTrackColliderMeshSegments() const
{
	TArray<FRecallTrackSegment> NewTackSegments;

#if WITH_EDITOR
	if (!SplineComponent || !TrackStaticMesh)
	{
		return NewTackSegments;
	}

	const FTransform SplineTransform = SplineComponent->GetComponentTransform();
	const float SplineLength = SplineComponent->GetSplineLength();
	const FBox TrackMeshBoundingBox = TrackStaticMesh->GetBoundingBox();
	const FVector TrackMeshExtents = TrackMeshBoundingBox.GetExtent();
	
	auto PushTrackSegment = [this, &NewTackSegments, &SplineTransform](
		int32 SegmentIndex, const TArray<FVector>& Vertices, const TArray<int32>& Triangles)
	{
		FVector Pivot = FVector::ZeroVector;
		if (Vertices.Num() > 0)
		{
			for (int32 VertexIndex = 0; VertexIndex < Vertices.Num(); ++VertexIndex)
			{
				Pivot += Vertices[VertexIndex];
			}
			Pivot /= Vertices.Num();
		}
		
		FRecallTrackSegment& TrackSegment = NewTackSegments[SegmentIndex];		
		TrackSegment.Location = SplineTransform.TransformPosition(Pivot);
		TrackSegment.Rotation = FQuat::Identity;

		TrackSegment.Vertices.Reserve(Vertices.Num());
		Algo::Transform(Vertices, TrackSegment.Vertices, [&Pivot, &SplineTransform](const FVector& Vertex)
		{
			return static_cast<FVector3f>(SplineTransform.TransformVector(Vertex - Pivot));
		});
		TrackSegment.Triangles = Triangles;
	};
	
	if (TrackColliderMeshMode == ERecallTrackColliderMeshMode::Box)
	{
		constexpr float ColliderWallHalfHeight = 1000.0f;
		const FVector ColliderExtents = bWallAroundTrack ? FVector(TrackMeshExtents.X, TrackMeshExtents.Y, ColliderWallHalfHeight) : TrackMeshExtents;
		const FVector ColliderOffset = bWallAroundTrack ? TrackColliderOffset + FVector::UpVector * ColliderWallHalfHeight : TrackColliderOffset;
		const FBox ColliderBounds(ColliderOffset - ColliderExtents, ColliderOffset + ColliderExtents);
		
		auto GenerateBoxSegment = [&](int32 SegmentIndex, const FFloatInterval& AlphaInterval, 	ERecallSplineMeshFace BoxFaces)
		{			
			TArray<FVector> SegmentVertices;
			TArray<int32> SegmentTriangles;
			Recall::Physics::Utils::GenerateMeshAlongSpline(SplineComponent, ColliderBounds, SegmentVertices, SegmentTriangles,
				TrackColliderSubdivisionDistance, ESplineCoordinateSpace::Local, BoxFaces, AlphaInterval, bWallAroundTrack);

			PushTrackSegment(SegmentIndex, SegmentVertices, SegmentTriangles);
		};
		
		const int32 SegmentCount = FMath::FloorToInt(SplineLength / TrackColliderMeshLength);
		const float SegmentLength = SplineLength / SegmentCount;
				
		NewTackSegments.SetNum(SegmentCount + 1); // +1 for the floor

		// Walls
		ParallelFor(SegmentCount, [&](int32 SegmentIndex)
		{
			ERecallSplineMeshFace ColliderFaces = RECALL_SPLINE_MESH_ALL_FACE_EXCEPT_BOTTOM;
			
			if (bWallAroundTrack)
			{
				ColliderFaces = ERecallSplineMeshFace::Top | ERecallSplineMeshFace::Left | ERecallSplineMeshFace::Right;
				
				if (SegmentIndex == 0)
				{
					ColliderFaces |= ERecallSplineMeshFace::Front;
				}
				else if (SegmentIndex == SegmentCount - 1)
				{
					ColliderFaces |= ERecallSplineMeshFace::Back;
				}
			}
			
			const float StartAlpha = (SegmentIndex * SegmentLength) / SplineLength;
			const float EndAlpha = ((SegmentIndex + 1) * SegmentLength) / SplineLength;
			const FFloatInterval AlphaInterval = FFloatInterval(StartAlpha, EndAlpha);
			GenerateBoxSegment(SegmentIndex, AlphaInterval, ColliderFaces);
		});

		// Floor
		GenerateBoxSegment(SegmentCount, FFloatInterval(0.0f, 1.0f), ERecallSplineMeshFace::Bottom);
	}
	else if (TrackColliderMeshMode == ERecallTrackColliderMeshMode::Mesh)
	{
		const float TrackMeshLength = TrackMeshExtents.X * 2.0f;
		const int32 SegmentCount = FMath::FloorToInt(SplineLength / TrackMeshLength) + 1;
		const float SegmentLength = SplineLength / static_cast<float>(SegmentCount);
		const float MeshLengthScale = SegmentLength / TrackMeshLength;

		NewTackSegments.SetNum(SegmentCount);
		
		TArray<FVector> MeshVertices;
		TArray<int32> MeshTriangles;
		Recall::Physics::Utils::SaveMesh(TrackStaticMesh, MeshVertices, MeshTriangles);

		ParallelFor(SegmentCount, [&](int32 SegmentIndex)
		{
			const float SplineSegmentDistance = SegmentIndex * SegmentLength;

			TArray<FVector> SegmentVertices;
			SegmentVertices.SetNumUninitialized(MeshVertices.Num());

			for (int32 VertexIndex = 0; VertexIndex < MeshVertices.Num(); ++VertexIndex)
			{
				FVector MeshVertex = MeshVertices[VertexIndex] - (FVector::ForwardVector * TrackMeshBoundingBox.Min.X);
				MeshVertex.X *= MeshLengthScale;
				const float SplineVertexDistance = SplineSegmentDistance + MeshVertex.X;
				MeshVertex.X = 0.0; // Ignore the X axis since the vertex is already offset along the spline

				FTransform SplineVertexTransform =  SplineComponent->GetTransformAtDistanceAlongSpline(SplineVertexDistance, ESplineCoordinateSpace::Local, true);
				
				// Ignore up vector from transform
				const FVector ForwardVector = SplineVertexTransform.GetRotation().GetForwardVector();
				const FVector RightVector = FVector::CrossProduct(FVector::UpVector, ForwardVector);
				SplineVertexTransform.SetRotation(UKismetMathLibrary::MakeRotationFromAxes(ForwardVector, RightVector, FVector::UpVector).Quaternion());

				SegmentVertices[VertexIndex] = SplineVertexTransform.TransformPosition(MeshVertex);
			}

			PushTrackSegment(SegmentIndex, SegmentVertices, MeshTriangles);
		});
	}
	else
	{
		unimplemented();
	}
#endif // WITH_EDITOR

	return NewTackSegments;
}

void ARecallTrackEntityActor::GenerateSplineMesh()
{	
#if WITH_EDITOR
	const TArray<FRecallTrackSegment> NewTackSegments = GenerateTrackColliderMeshSegments();

	if (NewTackSegments == TrackSegments)
	{
		return;
	}
	
	TrackSegments = NewTackSegments;
	
	Recall::Physics::Utils::GenerateSplineMeshAlongSpline(this, TrackStaticMesh, SplineComponent, SplineMeshComponents);

	bDirtySpline = true;
#endif // WITH_EDITOR
}

float ARecallTrackEntityActor::EvaluateLandscapeHeight(const FVector& Position) const
{	
	const float DistanceAlongSpline = SplineComponent->GetDistanceAlongSplineAtLocation(Position, ESplineCoordinateSpace::World);
	const FTransform SplinePointTransform = SplineComponent->GetTransformAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World, true);
	const float PositionOffset = FVector::DotProduct(SplinePointTransform.GetRotation().GetRightVector(), Position - SplinePointTransform.GetTranslation());
	
	const FVector TrackMeshExtents = TrackStaticMesh->GetBoundingBox().GetExtent() * SplinePointTransform.GetScale3D();
	const float TrackMeshHalfHeight = TrackMeshExtents.Z;
	const float TrackMeshHalfWidth = TrackMeshExtents.Y - MountainLandscapeBlendDistance;
	
	const float SplineLength = SplineComponent->GetSplineLength();
	const float WorldTrackHeight = SplinePointTransform.TransformPosition(FVector(0.0f, PositionOffset, 0.0f)).Z;
	float SplineDistance = FVector::Dist2D(SplinePointTransform.GetLocation(), Position);

	auto IsOutsideSpline = [this, &Position, SplineLength](bool bSplineStart)
	{
		const float EdgeDistance = bSplineStart ? 0.0f : SplineLength;
		const float ForwardMul = bSplineStart ? 1.0f : -1.0f;
		const FVector SplineEdgePos = SplineComponent->GetLocationAtDistanceAlongSpline(EdgeDistance, ESplineCoordinateSpace::World);
		const FVector SplineEdgeRightVec = SplineComponent->GetRightVectorAtSplinePoint(EdgeDistance, ESplineCoordinateSpace::World);
		const FVector SplineEdgeForwardVec = FVector::CrossProduct(SplineEdgeRightVec, FVector::UpVector) * ForwardMul;
		if (FVector::DotProduct(SplineEdgeForwardVec, Position - SplineEdgePos) < MountainLandscapeEdgeBlendDistance)
		{
			return true;				
		}
		return false;
	};
	
	bool bIsUnderTrack = SplineDistance < TrackMeshHalfWidth;
	if (bIsUnderTrack)
	{
		if (DistanceAlongSpline < MountainLandscapeEdgeBlendDistance)
		{
			if (IsOutsideSpline(true))
			{
				bIsUnderTrack = false;
				SplineDistance = 0.0f;
			}
		}
		else if (DistanceAlongSpline > SplineLength - MountainLandscapeEdgeBlendDistance)
		{
			if (IsOutsideSpline(false))
			{
				bIsUnderTrack = false;
				SplineDistance = 0.0f;
			}
		}
	}
	else
	{			
		SplineDistance -= TrackMeshHalfWidth;
	}
	
	float NewHeight = bIsUnderTrack ? WorldTrackHeight - TrackMeshHalfHeight - 50.0f : 0.0f;
	if (!bIsUnderTrack && bGenerateMountainAroundTrack && SplineDistance < MountainRadius)
	{
		float MountainAlpha = 1.0f - (SplineDistance / MountainRadius);
		if (MountainCurve)
		{
			MountainAlpha = MountainCurve->GetFloatValue(MountainAlpha);
		}
		NewHeight = MountainAlpha * (WorldTrackHeight + MountainLandscapeTrackOffset);
	}

	return NewHeight;
}

void ARecallTrackEntityActor::ApplyToLandscape() const
{
#if WITH_EDITOR
	ALandscape* Landscape = Cast<ALandscape>(UGameplayStatics::GetActorOfClass(this, ALandscape::StaticClass()));
	if (!IsValid(Landscape) || !SplineComponent || !TrackStaticMesh)
	{
		return;
	}
	
	const FTransform LandscapeTransform = Landscape->GetTransform();

	int32 SizeX, SizeY;
	TArray<float> Heights;
	Landscape->GetHeightValues(SizeX, SizeY, Heights);
	
	TArray<uint16> Data;
	Data.SetNum(SizeX * SizeY);

	ParallelFor(Data.Num(), [this, SizeX, &LandscapeTransform, &Data](int32 Index)
	{		
		const int32 Y = Index / SizeX;
		const int32 X = Index % SizeX;
		
		const FVector LocalHeightPos(X, Y, 0.0f);
		const FVector WorldHeightPosition = LandscapeTransform.TransformPosition(LocalHeightPos);
			
		const float NewWorldHeight = EvaluateLandscapeHeight(WorldHeightPosition);
		const FVector NewLocalHeightPos = LandscapeTransform.InverseTransformPosition(FVector(WorldHeightPosition.X, WorldHeightPosition.Y, NewWorldHeight));

		Data[Index] = LandscapeDataAccess::GetTexHeight(NewLocalHeightPos.Z);
	});
		
	FScopedSetLandscapeEditingLayer Scope(Landscape, Landscape->ReimportDestinationLayerGuid, [&] { check(Landscape); Landscape->RequestLayersContentUpdate(ELandscapeLayerUpdateMode::Update_Heightmap_All); });

	FScopedTransaction Transaction(LOCTEXT("Track_ApplyToLandscape", "Apply Track Height To Landscape"));

	LandscapeEditorUtils::SetHeightmapData(Landscape, Data);
	
	Landscape->ForceLayersFullUpdate();
#endif  // WITH_EDITOR
}

#undef LOCTEXT_NAMESPACE
