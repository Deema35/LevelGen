// Copyright 2018 Pavlov Dmitriy
#include "LevelCell.h"
#include "LevelGeneratorSettings.h"
#include "Rooms/LevelRooms.h"
#include "LevelGenPluginRuntime_LogCategory.h"
#include "LevelFloor.h"
#include "LevelGraphNode.h"
#include "ProceduralFigure.h"
#include "ProceduralMeshActor.h"
#include "Rooms/PlacedLevelRoom.h"

//.........................................
//EDirection
//.........................................

static_assert(ELevelCellType::end == (ELevelCellType)5, "You must update ELevelCellTypeCreate after add new enum");

FLevelCellBase* ELevelCellTypeCreate(ELevelCellType Type, FLevelCellData& CellData)
{
	

	switch (Type)
	{
	case ELevelCellType::Road:

		return new FLevelCellRoad(CellData);

	case ELevelCellType::MainRoad:

		return new FLevelCellMainRoad(CellData);

	case ELevelCellType::Bilding:
	
		return new FLevelCellBilding(CellData);

	case ELevelCellType::Tower:

		return new FLevelCellTower(CellData);


	default: throw;
	}

	
}




//******************************************************
//FLevelCellData
//*******************************************************

FLevelCellData::FLevelCellData(const FLevelGeneratorSettings& Settings, FVector2D _CellCoordinate, const FDataStorage& DataStorage) : LevelSettings(Settings), CellCoordinate(_CellCoordinate)
{
	Floors.resize(Settings.FloorNum, FLevelFloorData(DataStorage));

	for (int i = 0; i < Floors.size(); i++)
	{
		Floors[i].SetCoordinate(FVector(CellCoordinate, i));
	}

}

//******************************************************
//FLevelCellRoad
//*******************************************************

bool FLevelCellRoad::CanRoomPlacedOnCell(ERoomType RoomType) const
 { 
	static_assert((int)ERoomType::end == 4, "Check CanRoomPlacedOnCell");

	switch (RoomType)
	{
	case ERoomType::NodeRoom:

		return false;

	case ERoomType::GroundLinkRoom:

		return false;

	case ERoomType::RoadLinkRoom:

		return true;

	case ERoomType::TerraceLinkRoom:

		return true;

	default: throw;
	}
	return false;
}

//******************************************************
//FLevelCellBilding
//*******************************************************


void FLevelCellBilding::SetBaseLevelNum()
{
	
	std::vector<FPlacedLevelRoomLinkedToLevel*> Rooms;
	Rooms.reserve(CellData.LevelSettings.FloorNum);

	for (int k = 0; k < CellData.LevelSettings.FloorNum; k++)
	{
		if (CellData.Floors[k].PlasedRoom)
		{
			Rooms.push_back(CellData.Floors[k].PlasedRoom);
		}
		
	}
	if (!Rooms.empty())
	{
		
		CellData.LowerBaseLevelNum = (*Rooms.begin())->GetStartCoordinate().Z;
		CellData.UpperBaseLevelNum = Rooms.back()->GetStartCoordinate().Z;
		CellData.BaseLevelNumFirstIniciate = true;
	}
	else
	{
		CellData.BaseLevelNumFirstIniciate = false;
	}
		
}

bool FLevelCellBilding::CanRoomPlacedOnCell(ERoomType RoomType) const
{
	static_assert((int)ERoomType::end == 4, "Check CanRoomPlacedOnCell");

	switch (RoomType)
	{
	case ERoomType::NodeRoom:

		return true;

	case ERoomType::GroundLinkRoom:

		return true;

	case ERoomType::RoadLinkRoom:

		return false;

	case ERoomType::TerraceLinkRoom:

		return false;

	default: throw;
	}
	return false;
}

//******************************************************
//FLevelCellTower
//*******************************************************

void FLevelCellTower::SetBaseLevelNum()
{
	CellData.BaseLevelNumFirstIniciate = true;
	CellData.UpperBaseLevelNum = CellData.LevelSettings.FloorNum;
	CellData.LowerBaseLevelNum = 0;
	
}

bool FLevelCellTower::CanRoomPlacedOnCell(ERoomType RoomType) const
{
	static_assert((int)ERoomType::end == 4, "Check CanRoomPlacedOnCell");

	switch (RoomType)
	{
	case ERoomType::NodeRoom:

		return true;

	case ERoomType::GroundLinkRoom:

		return true;

	case ERoomType::RoadLinkRoom:

		return false;

	case ERoomType::TerraceLinkRoom:

		return false;

	default: throw;
	}
	return false;
}


void FLevelCellThroughCell::SetBaseLevelNum()
{
	CellData.BaseLevelNumFirstIniciate = true;
	CellData.UpperBaseLevelNum = 0;
	CellData.LowerBaseLevelNum = 0;
}
