// Implementation created by David 'vebski' Niemiec

#pragma once

#include "BuoyancyTypes.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct FBuoyantBodyData
{
	GENERATED_USTRUCT_BODY()

	/* Total volume of buoyant mesh */
	UPROPERTY()
	float BodyVolume;

	/* Center of mass for buoyant mesh */
	UPROPERTY()
	FVector LocalCentroidOfVolume;

	/* Density of body, it must be lower for body to float on surface.
	 *	1000.0f -> Density of water (1000kg/m^3)
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Buoyancy)
	float DensityOfBody;

	/* Length of body along X axis */
	UPROPERTY()
	float BodyLengthX;

	FBuoyantBodyData()
	{
		BodyVolume = 0.0f;
		LocalCentroidOfVolume = FVector::ZeroVector;
		DensityOfBody = 500.0f;
		BodyLengthX = 0.0f;
	}
};