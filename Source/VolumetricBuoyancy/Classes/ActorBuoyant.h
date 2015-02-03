// Implementation created by David 'vebski' Niemiec
#pragma once

#include "GameFramework/Actor.h"
#include "Misc/BuoyancyTypes.h"
#include "Misc/BuoyancyHelper.h"
#include "Ocean/OceanManager.h"
#include "ActorBuoyant.generated.h"

/**
 * 
 */
UCLASS()
class VOLUMETRICBUOYANCY_API AActorBuoyant : public AActor
{
	GENERATED_BODY()

protected:

	/* Should draw debug helper for buoyancy? */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Buoyancy)
	bool bDrawBuoyancyDebug;

	/* Current ocean manager on the level */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Buoyancy)
	AOceanManager* CurrentOceanManager;

	AActorBuoyant(const FObjectInitializer& ObjectInitializer);

	/* Buoyant mesh we use to calculate buoyancy for actor. It should have low amount of vertices.
	 * WARNING! Use only single mesh models! 
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Buoyancy)
	UStaticMeshComponent* BuoyantMesh;

	/* Informations about buoyancy for BuoyantMesh */
	UPROPERTY()
	FBuoyantBodyData BuoyancyData;

	virtual void DrawDebugHelpers();

	virtual AOceanManager* FindOceanManager();

	virtual void SetClippingTestPoints(TArray<FVector>& ClippingPoints);

public:
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	UStaticMeshComponent* GetBuoyantMesh() const;
};
