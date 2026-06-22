// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/UObjectGlobals.h"
#include "Templates/SubclassOf.h"
#include "DataProviders/AIDataProvider.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EnvironmentQuery/Generators/EnvQueryGenerator_ProjectedPoints.h"
#include "EnvQueryGenerator_Recall_View.generated.h"

UCLASS(meta = (DisplayName = "Points: View"), MinimalAPI)
class UEnvQueryGenerator_Recall_View : public UEnvQueryGenerator_ProjectedPoints
{
	GENERATED_BODY()
public:
	UEnvQueryGenerator_Recall_View(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Binds data to data providers 
	 *  @param QueryInstance - the instance of the query
	 */
	void BindDataToDataProviders(FEnvQueryInstance& QueryInstance) const;
	
	virtual void GenerateItems(FEnvQueryInstance& QueryInstance) const override;

	/** Returns the title of the generator on the corresponding node in the EQS Editor window */
	virtual FText GetDescriptionTitle() const override;

	/** Returns the details of the generator on the corresponding node in the EQS Editor window */
	virtual FText GetDescriptionDetails() const override;

protected:
	/** Distance between each point of the same angle */
	UPROPERTY(EditDefaultsOnly, Category = Generator)
	FAIDataProviderFloatValue AlignedPointsDistance;

	/** Maximum degrees of the generated cone */
	UPROPERTY(EditDefaultsOnly, Category = Generator)
	FAIDataProviderFloatValue ConeDegrees;

	/** The step of the angle increase. Angle step must be >=1
	 *  Smaller values generate less items
	 */
	UPROPERTY(EditDefaultsOnly, Category = Generator)
	FAIDataProviderFloatValue AngleStep;

	/** Generation distance */
	UPROPERTY(EditDefaultsOnly, Category = Generator)
	FAIDataProviderFloatValue Range;

	/** Generation height */
	UPROPERTY(EditDefaultsOnly, Category = Generator)
	FAIDataProviderFloatValue Height;
	
	/** Minimum generated height */
	UPROPERTY(EditDefaultsOnly, Category = Generator)
	FAIDataProviderFloatValue MinHeight;
	
	/** The actor (or actors) that will generate a cone in their facing direction */
	UPROPERTY(EditAnywhere, Category = Generator)
	TSubclassOf<UEnvQueryContext> CenterEntity;

	/** Whether to include CenterActors' locations when generating items. 
	 *	Note that this option skips the MinAngledPointsDistance parameter. */
	UPROPERTY(EditAnywhere, Category = Generator)
	uint8 bIncludeContextLocation : 1;
};
