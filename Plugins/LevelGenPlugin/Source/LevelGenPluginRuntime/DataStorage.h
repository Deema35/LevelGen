// Copyright 2018 Pavlov Dmitriy
#pragma once
#include "CoreMinimal.h"
#include <vector>
#include <mutex>
#include "LevelGenCore.h" 

struct FLevelGeneratorSettings;
class FLevelGenRoadBase;
class FLevelGenBildingZoneBase;
struct FLevelCellData;
struct FLvevelLayer;
class FCarTrackNode;
struct FCartTracsContainer;

enum class ELevelMapType : uint8
{
	BildingMap,
	LevelMap,
	RoadsMap,
	CarTrackMap
};


struct FDataStorage
{
	FDataStorage(const FLevelGeneratorSettings& _LevelSettings);

	~FDataStorage();

	TLevelMap2D<FLevelGenBildingZoneBase> BildingMap;

	TLevelMap2D<FLevelCellData> LevelMap;
	
	TLevelMap2D<FLevelGenRoadBase> RoadsMap;

	TLevelMap2D<std::vector<FCartTracsContainer>> CarTrackMap;

	FVector2D ActiveHoverCarTrackCenterCoordinate;

	int ActiveHoverCarTrackRadius;

	std::vector<std::vector<FCarTrackNode*>> SpawnCarTrack;

	std::mutex DataLock;

private:

	const FLevelGeneratorSettings& LevelSettings;
};
