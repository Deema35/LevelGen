// Copyright 2018 Pavlov Dmitriy
#pragma once
#include "EngineMinimal.h"
#include <vector>
#include "LevelGeneratorSettings.generated.h"

class FTexture2D;
class UStaticMesh;

UENUM()
enum class EHoverCarTrackDirection : uint8
{
	Pozitive,
	Negotive,
};



USTRUCT()
struct FLevelGeneratorMaterialSettings
{
	GENERATED_BODY()
public:	

	FLevelGeneratorMaterialSettings() = default;

	FLevelGeneratorMaterialSettings(UMaterialInterface* _Material) : Material(_Material) {}

	bool operator== (const FLevelGeneratorMaterialSettings& right) const;

public:

	
	UPROPERTY(EditAnywhere)
		UMaterialInterface* Material = nullptr;

	UPROPERTY(EditAnywhere)
		float MaterialCoefficient = 0.01;

};

USTRUCT()
struct FHoverCarTrackLayer
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
		int32 TrackHeight = 0;

	UPROPERTY(EditAnywhere)
		EHoverCarTrackDirection TrackDirection = EHoverCarTrackDirection::Pozitive;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 1))
		int OneHowerCarsOnRoadUnits = 4000;
};


USTRUCT(BlueprintType)
struct FLevelGeneratorSettings
{
	GENERATED_BODY()

		FLevelGeneratorSettings();

	UPROPERTY(EditAnywhere)
		FLevelGeneratorMaterialSettings WallsMaterial;

	UPROPERTY(EditAnywhere)
		FLevelGeneratorMaterialSettings BildingWallMaterial;

	UPROPERTY(EditAnywhere)
		FLevelGeneratorMaterialSettings FloorMaterial;

	UPROPERTY(EditAnywhere)
		FLevelGeneratorMaterialSettings RoofMaterial;

	UPROPERTY(EditAnywhere)
		FLevelGeneratorMaterialSettings RoadGroundMaterial;

	UPROPERTY(EditAnywhere)
		FLevelGeneratorMaterialSettings TestMaterial;

	UPROPERTY(EditAnywhere)
		UStaticMesh* MainRoadMesh = nullptr;

	UPROPERTY(EditAnywhere)
		UStaticMesh* MainRoadCrossingMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = "Roads", meta = (ClampMin = 3))
		int32 RoadFrequency = 7;

	UPROPERTY(EditAnywhere, Category = "Roads",  meta = (ClampMin = 2))
		int32 MainRoadFrequency = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Roads", meta = (ClampMin = 1))
		int32 RoadSize = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Roads", meta = (ClampMin = 1))
		int32 MainRoadSize = 2;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 3))
		int32 TowerFrequency = 7;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 2))

		int32 FullBildingTowersFrequency = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 1))
		int32 FloorNum = 8;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0))
		int32 CellSize = 2000;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 100))
		int32 CellHeight = 400;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 500))
		int32 FirstCityFloor = 4000;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 1000))
		int32 SecondCityFloor = 14000;


	UPROPERTY(EditAnywhere)
		FName ActorTag = "LevelGen";

	UPROPERTY(EditAnywhere, Category = "Graph", meta = (ClampMin = 3))
		int32 GraphNodsFrequency = 5;

	UPROPERTY(EditAnywhere, Category = "Graph", meta = (ClampMin = 1))
		int32 GraphLayerNum = 2;

	UPROPERTY(EditAnywhere)
		TArray<FHoverCarTrackLayer> HoverCarTrackLayers;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 1))
		int32 WallThickness = 60;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 100))
		int32 ComplicitySpawnForTick = 100;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 1))

		int32 DeltaCellForGeneration = 1;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 1))

		int32 MinBildingZoneSize = 2;

	
	

	bool operator== (const FLevelGeneratorSettings& right) const;
};