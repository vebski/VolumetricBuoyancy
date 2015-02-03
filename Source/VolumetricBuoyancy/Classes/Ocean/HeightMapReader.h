// Ocean created by Handkor

#pragma once

#include "GameFramework/Actor.h"
#include "HeightMapReader.generated.h"

/**
 * 
 */
UCLASS()
class VOLUMETRICBUOYANCY_API AHeightMapReader : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = HeightMap)
	UTextureRenderTarget2D* RenderTarget;

	UFUNCTION(BlueprintCallable, Category = "HeightMap|Update")
	void UpdateBuffer();

	UFUNCTION(BlueprintCallable, Category = "HeightMap|Texture Helper")
	FColor GetRenderTargetValue(float x, float y);

private:

	TArray<FColor> ColorBuffer;
	
};
