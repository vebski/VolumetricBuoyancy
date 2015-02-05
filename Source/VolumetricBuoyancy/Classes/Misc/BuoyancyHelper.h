// Implementation created by David 'vebski' Niemiec
#pragma once

#include "Object.h"
#include "Components/StaticMeshComponent.h"
#include "StaticMeshResources.h"
#include "Misc/BuoyancyTypes.h"
#include "Ocean/OceanManager.h"
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
	*	@param OceanManager				Current ocean manager on level
	*	@param BuoyantMesh				Mesh for calculation
	*	@param BuoyantData				Data about body
	*/
	static void ComputeBuoyancy(AOceanManager* OceanManager, UStaticMeshComponent* BuoyantMesh, FBuoyantBodyData& BuoyantData);

private:

	static float ComputeTetrahedronVolume(FVector& Center, FVector Point, FVector Vertex1, FVector Vertex2, FVector Vertex3);

	static float ClipTriangle(FVector& Center, FVector Point, FVector Vertex1, FVector Vertex2, FVector Vertex3, float Depth1, float Depth2, float Depth3);

	/* Calculate submerged volume of body
	*	@param OceanManager				Current ocean manager on level
	*	@param BuoyantMesh				Mesh for calculation
	*	@param Centroid		(out)		Center of calculated volume
	*/
	static float ComputeSubmergedVolume(AOceanManager* OceanManager, UStaticMeshComponent* BuoyantMesh, FVector& Centroid, FBuoyantBodyData& BuoyantData);

	static FClippingPlane ClaculateClippingPlane(AOceanManager* OceanManager, UStaticMeshComponent* BuoyantMesh, FBuoyantBodyData& BuoyantData);

	/* Calculate clipping points for 'Best fit plane' for extends of mesh
	*	@param OceanManager				Current ocean manager on level
	*	@param BuoyantMesh				Mesh for calculation
	*	@param ClippingPoints	(out)	Calculated clipping points
	*/
	static void GetTransformedTestPoints(AOceanManager* OceanManager, UStaticMeshComponent* BuoyantMesh, TArray<FVector>& ClippingPoints, FBuoyantBodyData& BuoyantData);
};
