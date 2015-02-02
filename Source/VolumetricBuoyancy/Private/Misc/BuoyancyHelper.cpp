// Implementation created by David 'vebski' Niemiec

#include "VolumetricBuoyancy.h"
#include "Misc/BuoyancyHelper.h"

float UBuoyancyHelper::ComputeVolume(const UStaticMeshComponent* BuoyantMesh, FVector& VolumeCentroid)
{
	if (!BuoyantMesh || !BuoyantMesh->StaticMesh || !BuoyantMesh->StaticMesh->RenderData)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Buoyant mesh data is not correct!");

		return 0.0f;
	}

	if (BuoyantMesh->StaticMesh->RenderData->LODResources.Num() <= 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Buoyant mesh has no LOD Resources!");

		return 0.0f;
	}

	FStaticMeshLODResources* LODResource = &BuoyantMesh->StaticMesh->RenderData->LODResources[0];

	float Volume = 0.0f;
	FVector Center = FVector::ZeroVector;

	if (LODResource)
	{
		// Due to performance I calculate volume only for main mesh group inside model (Section[0]).
		// If you want to know how to calculate for more then one section see -> UStaticMesh::GetPhysicsTriMeshData
		FStaticMeshSection& Section = LODResource->Sections[0];
		FPositionVertexBuffer& PositionVertexBuffer = LODResource->PositionVertexBuffer;

		FIndexArrayView Indices = LODResource->IndexBuffer.GetArrayView();

		uint32 i = Section.FirstIndex;
		uint32 OnePastLastIndex = Section.FirstIndex + Section.NumTriangles * 3;
		for (i; i < OnePastLastIndex; i += 3)
		{
			FVector Vertex1 = PositionVertexBuffer.VertexPosition(Indices[i]);
			FVector Vertex2 = PositionVertexBuffer.VertexPosition(Indices[i + 1]);
			FVector Vertex3 = PositionVertexBuffer.VertexPosition(Indices[i + 2]);


			Volume += ComputeTetrahedronVolume(Center, FVector::ZeroVector, Vertex1, Vertex2, Vertex3);
		}
	}

	VolumeCentroid = Center;
	VolumeCentroid *= 1.0f / Volume;

	return Volume;
}

float UBuoyancyHelper::ComputeTetrahedronVolume(FVector& Center, FVector Point, FVector Vertex1, FVector Vertex2, FVector Vertex3)
{
	FVector A = Vertex2 - Vertex1;
	FVector B = Vertex3 - Vertex1;
	FVector R = Point - Vertex1;

	float Volume = (1.0f / 6.0f) * FVector::DotProduct(FVector::CrossProduct(B, A), R);

	Center += 0.25f * Volume * (Vertex1 + Vertex2 + Vertex3 + Point);

	return Volume;
}

float UBuoyancyHelper::ClipTriangle(FVector& Center, FVector Point, FVector Vertex1, FVector Vertex2, FVector Vertex3, float Depth1, float Depth2, float Depth3)
{
	float Volume = 0.0f;
	FVector Vc1 = Vertex1 + (Depth1 / (Depth1 - Depth2)) * (Vertex2 - Vertex1);

	if (Depth1 < 0.0f)
	{
		if (Depth3 < 0.0f)
		{
			FVector Vc2 = Vertex2 + (Depth2 / (Depth2 - Depth3)) * (Vertex3 - Vertex2);

			Volume += ComputeTetrahedronVolume(Center, Point, Vc1, Vc2, Vertex1);
			Volume += ComputeTetrahedronVolume(Center, Point, Vc2, Vertex3, Vertex1);
		}
		else
		{
			FVector Vc2 = Vertex1 + (Depth1 / (Depth1 - Depth3)) * (Vertex3 - Vertex1);

			Volume += ComputeTetrahedronVolume(Center, Point, Vc1, Vc2, Vertex1);
		}
	}
	else
	{
		if (Depth3 < 0.0f)
		{
			FVector Vc2 = Vertex1 + (Depth1 / (Depth1 - Depth3)) * (Vertex3 - Vertex1);

			Volume += ComputeTetrahedronVolume(Center, Point, Vc1, Vertex2, Vertex3);
			Volume += ComputeTetrahedronVolume(Center, Point, Vc1, Vertex3, Vc2);
		}
		else
		{
			FVector Vc2 = Vertex2 + (Depth2 / (Depth2 - Depth3)) * (Vertex3 - Vertex2);

			Volume += ComputeTetrahedronVolume(Center, Point, Vc1, Vertex2, Vc2);
		}
	}

	return Volume;
}

void UBuoyancyHelper::ComputeBuoyancy(UStaticMeshComponent* BuoyantMesh, FBuoyantBodyData& BuoyantData)
{
	if (!BuoyantMesh || !BuoyantMesh->StaticMesh || !BuoyantMesh->StaticMesh->RenderData)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Buoyant mesh data is not correct!");

		return;
	}

	if (BuoyantMesh->StaticMesh->RenderData->LODResources.Num() <= 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Buoyant mesh has no LOD Resources!");

		return;
	}

	FVector SubmergedCentroid = FVector::ZeroVector;
	float SubmergedVolume = ComputeSubmergedVolume(BuoyantMesh, SubmergedCentroid);

	// @TODO: Move to actor tick and add local center offset to BuoyantData
	DrawDebugSphere(BuoyantMesh->GetWorld(), SubmergedCentroid, 8.0f, 8, FColor::Blue);

	if (SubmergedVolume < 0)
	{
		const float WaterDensity = 0.0001f; // 1g/cm^3 -> 1000kg/m^3
		const float WaterLinearDrag = 50000.0f;
		const float WaterAngularDrag = 5000.0f;
		const FVector WaterVelocity = FVector::ZeroVector;
		const FVector PlaneNormal = FVector::UpVector;
		const FVector PlaneLocation = FVector::ZeroVector;

		float VolumeMass = (BuoyantData.DensityOfBody * 0.0000001f) * BuoyantMesh->GetMass();

		/* You can use this crazy version, but I prefer one above since it gives us 2 variables we can control (and it dosen't have 10 freaking zeros ;) ) */
		//float VolumeMass = FMath::Abs((BuoyantData.DensityOfBody * 0.00000000001f * BuoyantData.BodyVolume) * 0.5f);

		FVector BuoyantForce = (WaterDensity * SubmergedVolume * BuoyantMesh->GetWorld()->GetGravityZ()) * PlaneNormal;
		float PartialMass = VolumeMass * SubmergedVolume / BuoyantData.BodyVolume;
		FVector Rc = SubmergedCentroid - BuoyantMesh->GetCenterOfMass();
		FVector Vc = BuoyantMesh->GetPhysicsLinearVelocity() + FVector::CrossProduct(BuoyantMesh->GetPhysicsAngularVelocity(), Rc);
		FVector DragForce = (PartialMass * WaterLinearDrag) * (WaterVelocity - Vc);

		FVector TotalForce = BuoyantForce + DragForce;
		BuoyantMesh->AddForceAtLocation(TotalForce, SubmergedCentroid);

		FVector TotalDrag = FVector::CrossProduct(Rc, TotalForce);
		//BuoyantMesh->AddTorque(TotalDrag);

		// Save old rotation and zero it so we can get actual extent
		// @FIXME: Move this to BeginPlay and save in BuoyantData!
		FRotator OldRot = BuoyantMesh->GetComponentRotation();
		BuoyantMesh->SetWorldRotation(FRotator(0.0f, 0.0f, 0.0f));

		FVector TrueExtent = BuoyantMesh->Bounds.GetBox().GetExtent();
		float Length = TrueExtent.X + TrueExtent.X;

		// Set back old rotation
		BuoyantMesh->SetWorldRotation(OldRot);

		float Length2 = Length * Length;
		FVector DragTorque = (-PartialMass * WaterAngularDrag * Length2) * BuoyantMesh->GetPhysicsAngularVelocity();
		//BuoyantMesh->AddTorque(DragTorque);
	}
}

float UBuoyancyHelper::ComputeSubmergedVolume(UStaticMeshComponent* BuoyantMesh, FVector& Centroid)
{
	 //Temp units simulating Plane, exchange with DynamicWater data when 'Best fit plane' ready
	FVector PlaneNormal = FVector::UpVector;
	FVector PlaneLocation = FVector::ZeroVector;

	FQuat Qt = BuoyantMesh->GetComponentRotation().Quaternion().Inverse();
	FVector Normal = Qt.RotateVector(PlaneNormal);
	float Offset = PlaneLocation.Z - FVector::DotProduct(PlaneNormal, BuoyantMesh->GetCenterOfMass());

	float TINY_DEPTH = -1e-6f;
	
	FStaticMeshLODResources* LODResource = &BuoyantMesh->StaticMesh->RenderData->LODResources[0];
	FStaticMeshSection& Section = LODResource->Sections[0];
	FPositionVertexBuffer& PositionVertexBuffer = LODResource->PositionVertexBuffer;

	TArray<float> Ds;
	Ds.AddUninitialized(Section.MaxVertexIndex + 1);

	uint32 NumSubmerged = 0;
	uint32 SampleVertex = 0;

	int32 i = 0;
	for (i; i < Ds.Num(); ++i)
	{
		Ds[i] = FVector::DotProduct(Normal, PositionVertexBuffer.VertexPosition(i)) - Offset;
		
		if (Ds[i] < TINY_DEPTH)
		{
			++NumSubmerged;
			SampleVertex = i;
		}
	}


	/* Return if no vertices are submerged */
	if (NumSubmerged <= 0)
	{
		Centroid = FVector::ZeroVector;
		return 0.0f;
	}

	/* Find a point on the water surface. */
	FVector Point = PositionVertexBuffer.VertexPosition(SampleVertex) - Ds[SampleVertex] * Normal;

	float Volume = 0.0f;
	Centroid = FVector::ZeroVector;

	FIndexArrayView Indices = LODResource->IndexBuffer.GetArrayView();

	uint32 j = Section.FirstIndex;
	uint32 OnePastLastIndex = Section.FirstIndex + Section.NumTriangles * 3;
	for (j; j < OnePastLastIndex; j += 3)
	{
		FVector Vertex1 = PositionVertexBuffer.VertexPosition(Indices[j]);
		float Depth1 = Ds[Indices[j]];

		FVector Vertex2 = PositionVertexBuffer.VertexPosition(Indices[j + 1]);
		float Depth2 = Ds[Indices[j + 1]];

		FVector Vertex3 = PositionVertexBuffer.VertexPosition(Indices[j + 2]);
		float Depth3 = Ds[Indices[j + 2]];

		if (Depth1 * Depth2 < 0.0f)
		{
			Volume += ClipTriangle(Centroid, Point, Vertex1, Vertex2, Vertex3, Depth1, Depth2, Depth3);
		}
		else if (Depth1 * Depth3 < 0.0f)
		{
			Volume += ClipTriangle(Centroid, Point, Vertex3, Vertex1, Vertex2, Depth3, Depth1, Depth2);
		}
		else if (Depth2 * Depth3 < 0.0f)
		{
			Volume += ClipTriangle(Centroid, Point, Vertex2, Vertex3, Vertex1, Depth2, Depth3, Depth1);
		}
		else if (Depth1 < 0.0f || Depth2 < 0.0f || Depth3 < 0.0f)
		{
			Volume += ComputeTetrahedronVolume(Centroid, Point, Vertex1, Vertex2, Vertex3);
		}
	}


	float TINY_VOLUME = -1e-6f;
	if (Volume >= TINY_VOLUME)
	{
		Centroid = FVector::ZeroVector;
		return 0.0f;
	}

	Centroid *= 1.0f / Volume;
	Centroid = Centroid + BuoyantMesh->GetCenterOfMass() + BuoyantMesh->GetComponentRotation().Quaternion().RotateVector(Centroid);

	return Volume;
}
