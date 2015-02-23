// Ocean created by Handkor

#include "VolumetricBuoyancy.h"
#include "Ocean/OceanManager.h"

AOceanManager::AOceanManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Center = FVector(0, 0, 0);
	Size = 10000.0f;
}

void AOceanManager::Initialize()
{
	ColorBuffer.Reset();
}

FVector AOceanManager::CalculateGerstnerWave(float WaveLength, float Amplitude, FVector2D Position, FVector2D Direction, float Angle, float Steepness, float Time, float Phase)
{
	float Lambda = (2 * PI) / WaveLength;

	FVector Dir = FVector(Direction.X, Direction.Y, 0.0f);
	Dir = Dir.RotateAngleAxis(Angle * 360, FVector(0, 0, 1));

	FVector2D RotatedDirection = FVector2D(Dir.X, Dir.Y);

	float WavePhase = Lambda * FVector2D::DotProduct(RotatedDirection, Position) + (Time + Phase);

	float Cos = FMath::Cos(WavePhase);
	float Sin = FMath::Sin(WavePhase);

	float QA = Steepness * Amplitude;

	return FVector(QA * RotatedDirection.X * Cos, QA * RotatedDirection.Y * Cos, Amplitude * Sin);
}

FVector AOceanManager::CalculateGerstnerWaveCluser(float MedianaWaveLength, float MedianaAmplitude, FVector2D Position, FVector2D MedianaDirection, float Steepness, float Time)
{
	FVector Sum = FVector(0, 0, 0);

	Sum += CalculateGerstnerWave(MedianaWaveLength, MedianaAmplitude, Position, MedianaDirection, 0, Steepness, Time, 0);
	Sum += CalculateGerstnerWave(MedianaWaveLength * 0.5f, MedianaAmplitude * 0.5f, Position, MedianaDirection, -0.1f, Steepness, Time, 0);
	Sum += CalculateGerstnerWave(MedianaWaveLength * 2.0f, MedianaAmplitude * 2.0f, Position, MedianaDirection, 0.1f, Steepness, Time, 0);
	Sum += CalculateGerstnerWave(MedianaWaveLength * 1.25f, MedianaAmplitude * 1.25f, Position, MedianaDirection, 0.05f, Steepness, Time, 0);
	Sum += CalculateGerstnerWave(MedianaWaveLength * 0.75f, MedianaAmplitude * 0.75f, Position, MedianaDirection, 0.075f, Steepness, Time, 0);
	Sum += CalculateGerstnerWave(MedianaWaveLength * 1.5f, MedianaAmplitude * 1.5f, Position, MedianaDirection, -0.125f, Steepness, Time, 0);
	Sum += CalculateGerstnerWave(MedianaWaveLength * 0.825f, MedianaAmplitude * 0.825f, Position, MedianaDirection, 0.063f, Steepness, Time, 0);
	Sum += CalculateGerstnerWave(MedianaWaveLength * 0.65f, MedianaAmplitude * 0.65f, Position, MedianaDirection, -0.11f, Steepness, Time, 0);

	return Sum / 8;
}


FVector AOceanManager::GetWaveHeight(FVector Location, float Time)
{
	FVector Sum = FVector(0, 0, 0);
	// 50 / 20
	// @FIXME: First 2 values for waveLength and Amplitude should be Read from Material in BP
	Sum += CalculateGerstnerWaveCluser(2500, 200, FVector2D(Location.X, Location.Y), FVector2D(0, 1), 0.5f, Time);
	Sum += CalculateGerstnerWaveCluser(1000, 115, FVector2D(Location.X, Location.Y), FVector2D(0, 1), 0.5f, Time);

	return Sum / 2;
}

FColor AOceanManager::GetTextureColorAt(int32 x, int32 y)
{
	if (Texture == NULL)
	{
		return FColor();
	}

	float Width = Texture->GetSurfaceWidth();
	float Height = Texture->GetSurfaceHeight();

	uint8* MipMap = (uint8*)Texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_ONLY);

	FColor* data = (FColor*)&MipMap[(x + (int)Width * y) * sizeof(FColor)];

	Texture->PlatformData->Mips[0].BulkData.Unlock();

	
	return *data;
}