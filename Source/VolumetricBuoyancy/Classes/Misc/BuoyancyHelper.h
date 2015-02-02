// Implementation created by David 'vebski' Niemiec
#pragma once

#include "Object.h"
#include "Components/StaticMeshComponent.h"
#include "StaticMeshResources.h"
#include "Misc/BuoyancyTypes.h"
#include "BuoyancyHelper.generated.h"

/**
 * 
 */
UCLASS()
class VOLUMETRICBUOYANCY_API UBuoyancyHelper : public UObject
{
	GENERATED_BODY()

public:

	/* Calculate total volume of body
	 *	@param BuoyantMesh				Mesh for calculation
	 *	@param VolumeCentroid (out)		Center of calculated volume
	 */
	static float ComputeVolume(const UStaticMeshComponent* BuoyantMesh, FVector& VolumeCentroid);

	/* Calculate and apply buoyancy
	*	@param BuoyantMesh				Mesh for calculation
	*	@param BuoyantData				Data about body
	*/
	static void ComputeBuoyancy(UStaticMeshComponent* BuoyantMesh, FBuoyantBodyData& BuoyantData);

private:

	static float ComputeTetrahedronVolume(FVector& Center, FVector Point, FVector Vertex1, FVector Vertex2, FVector Vertex3);

	static float ClipTriangle(FVector& Center, FVector Point, FVector Vertex1, FVector Vertex2, FVector Vertex3, float Depth1, float Depth2, float Depth3);

	/* Calculate submerged volume of body
	*	@param BuoyantMesh				Mesh for calculation
	*	@param Centroid		(out)		Center of calculated volume
	*/
	static float ComputeSubmergedVolume(UStaticMeshComponent* BuoyantMesh, FVector& Centroid);
};
