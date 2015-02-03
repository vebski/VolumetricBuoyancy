// Ocean created by Handkor
#pragma once

#include "GameFramework/Actor.h"
#include "OceanManager.generated.h"

/**
 * 
 */
UCLASS()
class VOLUMETRICBUOYANCY_API AOceanManager : public AActor
{
	GENERATED_BODY()

protected:

	AOceanManager(const FObjectInitializer& ObjectInitializer);

	TArray<FColor> ColorBuffer;

	FVector Center;

	float Size;

public:

	UPROPERTY(EditAnywhere, Category = HeightMap)
	UTexture2D* Texture;

	void Initialize();

	UFUNCTION(BlueprintCallable, Category = "GerstnerWave")
	FVector CalculateGerstnerWave(float WaveLength, float Amplitude, FVector2D Position, FVector2D Direction, float Angle, float Steepness, float Time, float Phase);

	UFUNCTION(BlueprintCallable, Category = "GerstnerWave")
	FVector CalculateGerstnerWaveCluser(float MedianaWaveLength, float MedianaAmplitude, FVector2D Position, FVector2D MedianaDirection, float Steepness, float Time);

	UFUNCTION(BlueprintCallable, Category = "GerstnerWave")
	FVector GetWaveHeight(FVector Location, float Time);

	UFUNCTION(BlueprintCallable, Category = "GerstnerWave")
	FColor GetTextureColorAt(int32 x, int32 y);
};
