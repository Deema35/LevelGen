// Copyright 2018 Pavlov Dmitriy
#pragma once

#include "CoreMinimal.h"
#include "LevelGenCore.h"
#include <vector>
#include <memory>
#include <utility>
#include <set> 



struct FLevelCellBase;
struct FLevelGeneratorSettings;
class ALevelGenerator;
struct FLevelGraphNodeRoom;
struct FLevelGraphLinkRoom;
class FPlacedLevelRoomLinkedToLevel;
struct FJointPart;
struct FLvevelLayer;
struct FLevelGraphLink;


struct FLevelGraphNodeBase
{
	FLevelGraphNodeBase(bool _InUpperLayer, FDataStorage& _DataStorage, const FLevelGeneratorSettings& _LevelSettings, const ALevelGenerator& _LevelGenerator) : InUpperLayer(_InUpperLayer), FromLastGraph(false),
		DataStorage(_DataStorage), LevelSettings(_LevelSettings), LevelGenerator(_LevelGenerator)
	{}
	
	virtual ~FLevelGraphNodeBase() = 0;

	bool InUpperLayer;

	bool FromLastGraph;

	std::map<FLevelGraphNodeBase*, std::shared_ptr<FLevelGraphLink>> Links;

	std::shared_ptr<FPlacedLevelRoomLinkedToLevel> NodeRoom;

	void AddLink(FLevelGraphNodeBase* LinkedNode, std::shared_ptr<FLevelGraphLink> Link) { Links.insert(std::make_pair(LinkedNode, Link)); }

protected:

	bool CreateLink(int LinkX, int LinkY, FLvevelLayer& LvevelLayer);

	FDataStorage& DataStorage;

	const FLevelGeneratorSettings& LevelSettings;

	const ALevelGenerator& LevelGenerator;
};

struct FLevelGraphNode : public FLevelGraphNodeBase
{
	FLevelGraphNode(bool InUpperLayer, std::pair<FVector2D, FVector2D>& _GeneratedArea, FDataStorage& DataStorage,
		const FLevelGeneratorSettings& LevelSettings, const ALevelGenerator& LevelGenerator)
		: GeneratedArea(_GeneratedArea), FLevelGraphNodeBase(InUpperLayer, DataStorage, LevelSettings, LevelGenerator) {}

	bool SetRoom(FVector2D SearchStart, std::pair<int, int> FloorsInterval);

	void CreateLinks(FVector2D NodeCoordinate, FLvevelLayer& LvevelLayer);

private:

	bool SetRoomForCurrentFloor(int FlooreNum, FVector2D SearchStart, const UObjectsDisposer& NodeDisposer, const std::vector<FLevelRoomNode*>& LevelRooms);

	std::pair<FVector2D, FVector2D> GeneratedArea;

};




