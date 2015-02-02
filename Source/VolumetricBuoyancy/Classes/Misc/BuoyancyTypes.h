// Implementation created by David 'vebski' Niemiec

#pragma once

#include "BuoyancyTypes.generated.h"

USTRUCT()
struct FBuoyantBodyData
{
	GENERATED_USTRUCT_BODY()

	/* Total volume of buoyant mesh */
	UPROPERTY()
	float BodyVolume;

	/* Center of mass for buoyant mesh */
	UPROPERTY()
	FVector LocalCentroidOfVolume;

	FBuoyantBodyData()
	{
		BodyVolume = 0.0f;
		LocalCentroidOfVolume = FVector::ZeroVector;
	}
};