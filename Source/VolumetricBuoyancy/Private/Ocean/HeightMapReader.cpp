// Ocean created by Handkor

#include "VolumetricBuoyancy.h"
#include "Ocean/HeightMapReader.h"


void AHeightMapReader::UpdateBuffer()
{
	ColorBuffer.Reset();

	if (RenderTarget != NULL)
	{
		FTextureRenderTarget2DResource* textureResource = (FTextureRenderTarget2DResource*)RenderTarget->Resource;
	}
}

FColor AHeightMapReader::GetRenderTargetValue(float x, float y)
{
	float Size = 10000.0f;

	if (RenderTarget == NULL || ColorBuffer.Num() == 0)
	{
		return FColor(0);
	}

	float Width = RenderTarget->GetSurfaceWidth();
	float Height = RenderTarget->GetSurfaceHeight();

	float NormalizedX = (x / Size) + 0.5f;
	float NormalizedY = (y / Size) + 0.5f;

	int i = (int)(NormalizedX * Width);
	int j = (int)(NormalizedY * Height);

	if (i < 0)
	{
		i = 0;
	}

	if (i >= Width)
	{
		i = Width - 1;
	}

	if (j < 0)
	{
		j = 0;
	}

	if (j >= Height)
	{
		j = Height - 1;
	}

	int Index = i + j * Width;
	if (Index < 0)
	{
		Index = 0;
	}

	if (Index >= ColorBuffer.Num())
	{
		Index = ColorBuffer.Num();
	}


	return ColorBuffer[Index];
}


