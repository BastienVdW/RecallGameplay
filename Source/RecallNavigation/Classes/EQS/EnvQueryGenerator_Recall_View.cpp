// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "EnvQueryGenerator_Recall_View.h"

#include "EnvironmentQuery/Contexts/EnvQueryContext_Querier.h"
#include "GameFramework/Actor.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EnvQueryGenerator_Recall_View)

#define LOCTEXT_NAMESPACE "EnvQueryGenerator"

UEnvQueryGenerator_Recall_View::UEnvQueryGenerator_Recall_View(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	CenterEntity = UEnvQueryContext_Querier::StaticClass();
	AlignedPointsDistance.DefaultValue = 100.f;
	ConeDegrees.DefaultValue = 90.f;
	AngleStep.DefaultValue = 10.f;
	Range.DefaultValue = 1000.f;
	Height.DefaultValue = 0.0f;
	MinHeight.DefaultValue = 0.0f;
	bIncludeContextLocation = false;
}

void UEnvQueryGenerator_Recall_View::BindDataToDataProviders(FEnvQueryInstance& QueryInstance) const
{
	//Bind the necessary data to data providers
	UObject* BindOwner = QueryInstance.Owner.Get();
	AlignedPointsDistance.BindData(BindOwner, QueryInstance.QueryID);
	ConeDegrees.BindData(BindOwner, QueryInstance.QueryID);
	AngleStep.BindData(BindOwner, QueryInstance.QueryID);
	Range.BindData(BindOwner, QueryInstance.QueryID);
}

void UEnvQueryGenerator_Recall_View::GenerateItems(FEnvQueryInstance& QueryInstance) const
{
	TArray<FEnvQuerySpatialData> CenterActors;
	QueryInstance.PrepareContext(CenterEntity, CenterActors);

	if (CenterActors.Num() <= 0)
	{
		return;
	}

	BindDataToDataProviders(QueryInstance);

	const float ConeDegreesValue = FMath::Clamp(FMath::Abs(ConeDegrees.GetValue()), 0.f, 359.f);
	if (ConeDegreesValue == 0)
	{
		return;
	}

	//Get the values from each data provider
	const float AlignedPointDistanceValue = AlignedPointsDistance.GetValue();
	const float AngleStepValue = FMath::Clamp(AngleStep.GetValue(), 1.f, 359.f);
	const float GenerationRange = FMath::Clamp(Range.GetValue(), 0.f, MAX_flt);
	const int32 MaxPointsPerAngleValue = FMath::FloorToInt(GenerationRange / AlignedPointDistanceValue);
	const float GenerationHeight = FMath::Clamp(Height.GetValue(), 0.f, MAX_flt);
	const int32 MaxPointsPerHeightValue = FMath::FloorToInt(GenerationHeight / AlignedPointDistanceValue) + 1;
	const float GenerationMinHeight = FMath::Clamp(MinHeight.GetValue(), 0.f, MAX_flt);

	TArray<FNavLocation> GeneratedItems;
	GeneratedItems.Reserve(CenterActors.Num() * FMath::CeilToInt(ConeDegreesValue / AngleStepValue) * MaxPointsPerAngleValue * MaxPointsPerHeightValue + 1);

	//Generate points for each actor
	for (int32 CenterIndex = 0; CenterIndex < CenterActors.Num(); CenterIndex++)
	{
		const FVector ForwardVector = CenterActors[CenterIndex].Rotation.Quaternion().GetForwardVector();
		const FVector ActorLocation = CenterActors[CenterIndex].Location;
		
		for (float Angle = -(ConeDegreesValue * 0.5f); Angle <= (ConeDegreesValue * 0.5f); Angle += AngleStepValue)
		{
			const FVector AxisDirection = ForwardVector.RotateAngleAxis(Angle, FVector::UpVector);

			// skipping PointIndex == 0 as that's the context's location
			for (int32 AnglePointIndex = 1; AnglePointIndex <= MaxPointsPerAngleValue; AnglePointIndex++)
			{
				for (int32 HeightPointIndex = 0; HeightPointIndex < MaxPointsPerHeightValue; HeightPointIndex++)
				{
					const float HeightOffset = HeightPointIndex * AlignedPointDistanceValue - GenerationHeight * 0.5f;
					const FVector GeneratedLocation = ActorLocation + (AxisDirection * AnglePointIndex * AlignedPointDistanceValue) + (FVector::UpVector * HeightOffset);
					if (GeneratedLocation.Z < GenerationMinHeight)
					{
						continue;
					}
					
					GeneratedItems.Add(FNavLocation(GeneratedLocation));
				}
			}
		}

		if (bIncludeContextLocation)
		{
			GeneratedItems.Add(FNavLocation(ActorLocation));
		}
	}	

	ProjectAndFilterNavPoints(GeneratedItems, QueryInstance);
	StoreNavPoints(GeneratedItems, QueryInstance);
}

FText UEnvQueryGenerator_Recall_View::GetDescriptionTitle() const
{
	return FText::Format(LOCTEXT("ConeDescriptionGenerateAroundContext", "{0}: generate in front of {1}"),
		Super::GetDescriptionTitle(), UEnvQueryTypes::DescribeContext(CenterEntity));
}

FText UEnvQueryGenerator_Recall_View::GetDescriptionDetails() const
{
	FText Desc = FText::Format(LOCTEXT("ConeDescription", "degrees: {0}, angle step: {1}"),
		FText::FromString(ConeDegrees.ToString()), FText::FromString(AngleStep.ToString()));

	FText ProjDesc = ProjectionData.ToText(FEnvTraceData::Brief);
	if (!ProjDesc.IsEmpty())
	{
		FFormatNamedArguments ProjArgs;
		ProjArgs.Add(TEXT("Description"), Desc);
		ProjArgs.Add(TEXT("ProjectionDescription"), ProjDesc);
		Desc = FText::Format(LOCTEXT("ConeDescriptionWithProjection", "{Description}, {ProjectionDescription}"), ProjArgs);
	}

	return Desc;
}

#undef LOCTEXT_NAMESPACE
