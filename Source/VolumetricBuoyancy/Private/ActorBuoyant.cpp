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

	CurrentOceanManager = FindOceanManager();

	BuoyancyData.BodyVolume = UBuoyancyHelper::ComputeVolume(BuoyantMesh, BuoyancyData.LocalCentroidOfVolume);
	SetClippingTestPoints(BuoyancyData.ClippingPointsOffsets);

	// Save old rotation and zero it so we can get actual extent
	// @FIXME: Move this to some function, best place would be inside ComouteVolume
	FRotator OldRot = BuoyantMesh->GetComponentRotation();
	BuoyantMesh->SetWorldRotation(FRotator(0.0f, 0.0f, 0.0f));

	FVector TrueExtent = BuoyantMesh->Bounds.GetBox().GetExtent();
	BuoyancyData.BodyLengthX = TrueExtent.X + TrueExtent.X;

	// Set back old rotation
	BuoyantMesh->SetWorldRotation(OldRot);
}

void AActorBuoyant::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (CurrentOceanManager->IsValidLowLevel())
	{
		UBuoyancyHelper::ComputeBuoyancy(CurrentOceanManager, BuoyantMesh, BuoyancyData);

		DrawDebugHelpers();
	}
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

AOceanManager* AActorBuoyant::FindOceanManager()
{
	TActorIterator<AOceanManager> ActorItr(GetWorld());

	for (ActorItr; ActorItr; ++ActorItr)
	{
		AOceanManager* TempOceanManager = Cast<AOceanManager>(*ActorItr);

		if (TempOceanManager->IsValidLowLevel())
		{
			return TempOceanManager;
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Can't find ocean manager on level!");

	return NULL;
}

void AActorBuoyant::SetClippingTestPoints(TArray<FVector>& ClippingPoints)
{
	// Save old rotation and zero it so we can get actual extent
	FRotator OldRot = BuoyantMesh->GetComponentRotation();
	BuoyantMesh->SetWorldRotation(FRotator(0.0f, 0.0f, 0.0f));

	FVector TrueExtent = BuoyantMesh->Bounds.GetBox().GetExtent();

	// Set back old rotation
	BuoyantMesh->SetWorldRotation(OldRot);

	//Select 8 points from extent + Center
	ClippingPoints.Add(FVector(TrueExtent.X, -TrueExtent.Y, 0.0f));	// Front Left
	ClippingPoints.Add(FVector(TrueExtent.X, 0.0f, 0.0f));			// Front Middle
	ClippingPoints.Add(FVector(TrueExtent.X, TrueExtent.Y, 0.0f));	// Front Right
	ClippingPoints.Add(FVector(0.0f, -TrueExtent.Y, 0.0f));			// Center Left
	ClippingPoints.Add(FVector(0.0f, 0.0f, 0.0f));					// Center Middle
	ClippingPoints.Add(FVector(0.0f, TrueExtent.Y, 0.0f));			// Center Right
	ClippingPoints.Add(FVector(-TrueExtent.X, -TrueExtent.Y, 0.0f));// Back Left
	ClippingPoints.Add(FVector(-TrueExtent.X, 0.0f, 0.0f));			// Back Middle
	ClippingPoints.Add(FVector(-TrueExtent.X, TrueExtent.Y, 0.0f)); // Back Right
}



