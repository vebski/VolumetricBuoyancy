// Implementation created by David 'vebski' Niemiec

#include "VolumetricBuoyancy.h"
#include "ActorBuoyant.h"

AActorBuoyant::AActorBuoyant(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	bDrawBuoyancyDebug = false;

	/* Default setup for Buoyant Mesh */
	BuoyantMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("BuoyantMesh"));
	SetRootComponent(BuoyantMesh);
	BuoyantMesh->SetVisibility(false);
	BuoyantMesh->SetSimulatePhysics(true);
	BuoyantMesh->SetPhysicsMaxAngularVelocity(1500.0f);
}

void AActorBuoyant::BeginPlay()
{
	Super::BeginPlay();

	BuoyancyData.BodyVolume = UBuoyancyHelper::ComputeVolume(BuoyantMesh, BuoyancyData.LocalCentroidOfVolume);
}

void AActorBuoyant::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UBuoyancyHelper::ComputeBuoyancy(BuoyantMesh, BuoyancyData);

	DrawDebugHelpers();
}

void AActorBuoyant::DrawDebugHelpers()
{
#if !UE_BUILD_SHIPPING
	if (bDrawBuoyancyDebug)
	{
		FVector WorldVolumeCentoid = BuoyantMesh->GetCenterOfMass() + BuoyantMesh->GetComponentRotation().Quaternion().RotateVector(BuoyancyData.LocalCentroidOfVolume);

		/* Volume Centroid */
		DrawDebugSphere(BuoyantMesh->GetWorld(), WorldVolumeCentoid, 8.0f, 8, FColor::Cyan);
	}
#endif
}

UStaticMeshComponent* AActorBuoyant::GetBuoyantMesh() const
{
	return BuoyantMesh;
}



