// Copyright 2018 Pavlov Dmitriy
#include "LevelGraphNode.h"
#include "LevelCell.h"
#include "LevelGeneratorSettings.h"
#include "LevelGenPluginRuntime_LogCategory.h"
#include "LevelGenerator.h"
#include "Rooms/LevelRooms.h"
#include "Rooms/PlacedLevelRoom.h"
#include <list>
#include <random>   
#include <ctime>
#include <algorithm>
#include "SearchGraph.h"


//.........................................
//FLevelGraphLink
//.............................................

FLevelGraphLink::FLevelGraphLink(FJointPart& _In, FJointPart& _Out, FDataStorage& _DataStorage, const FLevelGeneratorSettings& _LevelSettings, const ALevelGenerator& _LevelGenerator)
	: In(_In), Out(_Out), DataStorage(_DataStorage), LevelSettings(_LevelSettings), LevelGenerator(_LevelGenerator)
{
	std::shared_ptr<USearchGraph> SearchGraph = std::shared_ptr<USearchGraph>(new USearchGraph(In, Out, DataStorage, LevelGenerator));

	std::vector<std::shared_ptr<FSearchGraphNode>>  RoomChain;

	SearchGraph->GetRoomChain(RoomChain);
	for (int i = 0; i < RoomChain.size(); i++)
	{
		Rooms.push_back(std::shared_ptr<FPlacedLevelRoomLinkedToLevel>(new FPlacedLevelRoomLinkedToLevel(*RoomChain[i], RoomChain[i]->GetStartCoordinate(), DataStorage)));
	}
	
}

//.........................................
//FLevelGraphNodeBase
//.............................................

FLevelGraphNodeBase::~FLevelGraphNodeBase() {}

bool FLevelGraphNodeBase::CreateLink(int LinkX, int LinkY, FLvevelLayer& LvevelLayer)
{
	if (LinkX >= 0, LinkY >= 0, LvevelLayer.Graph.size() > LinkX &&  LvevelLayer.Graph[LinkX].size() > LinkY)
	{
		if (LvevelLayer.Graph[LinkX][LinkY] && Links.find(LvevelLayer.Graph[LinkX][LinkY].get()) == Links.end())
		{
			Links.insert(std::make_pair(LvevelLayer.Graph[LinkX][LinkY].get(), std::shared_ptr<FLevelGraphLink>()));
			LvevelLayer.Graph[LinkX][LinkY]->Links.insert(std::make_pair(this, std::shared_ptr<FLevelGraphLink>()));
			return true;
		}
	}

	return false;
}


//.........................................
//FLevelGraphNode
//.............................................

 bool FLevelGraphNode::SetRoom(FVector2D SearchStart, std::pair<int, int> FloorsInterval)
 {
	 if (!NodeRoom)
	 {


		 std::vector<FLevelRoomNode*> LevelRooms;

		 LevelGenerator.GeRoomStorage()->GetLevelRooms(LevelRooms, Links.size(), LevelSettings.GraphNodsFrequency);

		 std::shuffle(LevelRooms.begin(), LevelRooms.end(), LevelGenerator.GetRandomGenerator().GetGenerator());


		 UObjectsDisposer NodeDisposer(GeneratedArea, LevelGenerator.GetRandomGenerator().GetGenerator(), LevelSettings);

		 int NodeFloor = LevelGenerator.GetRandomGenerator().GetRandomNumber(FloorsInterval.first, FloorsInterval.second);

		 if (SetRoomForCurrentFloor(NodeFloor, SearchStart, NodeDisposer, LevelRooms)) return true;

		 else
		 {
			 for (int j = 0; j < 3; j++)
			 {
				 if (NodeFloor - j >= FloorsInterval.first && SetRoomForCurrentFloor(NodeFloor - j, SearchStart, NodeDisposer, LevelRooms)) return true;
				 if (NodeFloor + j <= FloorsInterval.second && SetRoomForCurrentFloor(NodeFloor + j, SearchStart, NodeDisposer, LevelRooms)) return true;
			 }
		 }

		 return false;
	 }

	 return true;
	
 }

 bool FLevelGraphNode::SetRoomForCurrentFloor(int FlooreNum, FVector2D SearchStart, const UObjectsDisposer& NodeDisposer, const std::vector<FLevelRoomNode*>& LevelRooms)
 {
	 for (int i = 0; i < LevelRooms.size(); i++)
	 {
		 FVector2D NodeCoordinate;

		 auto Predicate = [&](FVector Coordinate)
		 {
			 return LevelRooms[i]->IsRoomFit(Coordinate, DataStorage);
		 };

		 int CellOffsetX = LevelGenerator.GetRandomGenerator().GetRandomNumber(-LevelSettings.GraphNodsFrequency / 2, LevelSettings.GraphNodsFrequency / 2);
		 int CellOffsetY = LevelGenerator.GetRandomGenerator().GetRandomNumber(-LevelSettings.GraphNodsFrequency / 2, LevelSettings.GraphNodsFrequency / 2);


		 FVector2D SearchStartCoordinate = FVector2D(SearchStart.X + CellOffsetX, SearchStart.Y + CellOffsetY);

		 if (NodeDisposer.GetLocation(NodeCoordinate, SearchStartCoordinate, FlooreNum, FVector2D(LevelRooms[i]->GetSize()), Predicate))
		 {
			 NodeRoom = std::shared_ptr<FPlacedLevelRoomLinkedToLevel>(new FPlacedLevelRoomLinkedToLevel(LevelRooms[i], FVector(NodeCoordinate, FlooreNum), DataStorage));
			 return true;
		 }

	 }

	 return false;
 }

 
 

 void FLevelGraphNode::CreateLinks(FVector2D NodeNumber, FLvevelLayer& LevelLayer)
 {

	 CreateLink(NodeNumber.X + 1, NodeNumber.Y, LevelLayer);
	 CreateLink(NodeNumber.X - 1, NodeNumber.Y, LevelLayer);
	 CreateLink(NodeNumber.X, NodeNumber.Y + 1, LevelLayer);
	 CreateLink(NodeNumber.X, NodeNumber.Y - 1, LevelLayer);
	 
 }



 