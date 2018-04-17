// Copyright 2018 Pavlov Dmitriy
#include "LevelFloor.h"
#include "LevelCell.h"
#include "Rooms/LevelRooms.h"
#include "Rooms/PlacedLevelRoom.h"
#include "LevelGenPluginRuntime_LogCategory.h"
#include "DataStorage.h"

static_assert(ELevelFloorType::end == (ELevelFloorType)5, "You must update ELevelFloorTypeCreate after add new enum");

FLevelFloorBase* ELevelFloorTypeCreate(ELevelFloorType FloorType, FLevelFloorData& FloorData)
{
	switch (FloorType)
	{

	case ELevelFloorType::Room:

		return new FLevelFloorRoom(FloorData);

	case ELevelFloorType::Ground:

		return new FLevelFloorGround(FloorData);

	case ELevelFloorType::LowerVoid:

		return new FLevelFloorLowerVoid(FloorData);

	case ELevelFloorType::UnderGroundRoom:

		return new FLevelFloorUnderGroundRoom(FloorData);

	case ELevelFloorType::Void:

		return new FLevelFloorVoid(FloorData);

	default: throw;

	}
}




//******************************************************
//FLevelFloorRoom
//*******************************************************
bool FLevelFloorRoom::NeedCreateFloor(FLevelFloorData* OtherFloor) const
{
	if (OtherFloor->FloorInst->GetType() == ELevelFloorType::Ground || OtherFloor->FloorInst->GetType() == ELevelFloorType::LowerVoid ||
		OtherFloor->FloorInst->GetType() == ELevelFloorType::UnderGroundRoom)
	{
		return true;
	}

	else return false;
	
}

bool FLevelFloorRoom::NeedCreateWall(FLevelFloorData* OtherFloor, EWallType WallType) const
{
	if (!OtherFloor->FloorInst) return false;

	switch (WallType)
	{
	case EWallType::InternalWall:
	case EWallType::OuterWall:

		if (OtherFloor->FloorInst->GetType() == ELevelFloorType::Ground || OtherFloor->FloorInst->GetType() == ELevelFloorType::LowerVoid)
		{
			return true;
		}

		else if (OtherFloor->FloorInst->GetType() == ELevelFloorType::UnderGroundRoom)
		{
			if (FloorData.PlasedRoom != OtherFloor->PlasedRoom && !FloorData.PlasedRoom->IsRoomConectitToPlace(OtherFloor->GetCoordinate(), FloorData.DataStorage))
			{
				return true;
			}

			else return false;


		}

		else return false;

	case EWallType::RoadWall:

		if (OtherFloor->FloorInst->GetType() == ELevelFloorType::Ground) return true;

		else if (OtherFloor->FloorInst->GetType() == ELevelFloorType::UnderGroundRoom)
		{
			if (FloorData.PlasedRoom != OtherFloor->PlasedRoom && !FloorData.PlasedRoom->IsRoomConectitToPlace(OtherFloor->GetCoordinate(), FloorData.DataStorage))
			{
				return true;
			}

			else return false;

		}

		else return false;

	default: throw;
	}
	
}

//******************************************************
//FLevelFloorUnderGroundRoom
//*******************************************************

bool FLevelFloorUnderGroundRoom::NeedCreateFloor(FLevelFloorData* OtherFloor) const
{
	if (OtherFloor->FloorInst->GetType() == ELevelFloorType::UnderGroundRoom)
	{
		if (OtherFloor->PlasedRoom != FloorData.PlasedRoom)
		{
			return true;
		}

		else return false;
		
	}

	else if (OtherFloor->FloorInst->GetType() == ELevelFloorType::Ground || OtherFloor->FloorInst->GetType() == ELevelFloorType::LowerVoid ||
		OtherFloor->FloorInst->GetType() == ELevelFloorType::Room || OtherFloor->FloorInst->GetType() == ELevelFloorType::Void)
	{
		return true;
	}

	else return false;
	
}

bool FLevelFloorUnderGroundRoom::NeedCreateWall(FLevelFloorData* OtherFloor, EWallType WallType) const
{
	if (!OtherFloor->FloorInst) return false;

	switch (WallType)
	{
	case EWallType::InternalWall:
	case EWallType::OuterWall:

		if (OtherFloor->FloorInst->GetType() == ELevelFloorType::Ground || OtherFloor->FloorInst->GetType() == ELevelFloorType::Void) return true;

		else if (OtherFloor->FloorInst->GetType() == ELevelFloorType::Room)
		{
			if (FloorData.PlasedRoom != OtherFloor->PlasedRoom && !FloorData.PlasedRoom->IsRoomConectitToPlace(OtherFloor->GetCoordinate(), FloorData.DataStorage))
			{
				return true;
			}

			else return false;

		}

		else return false;

	case EWallType::RoadWall:

		return false;

	default: throw;

	}
	
}



//******************************************************
//FLevelFloorVoid
//*******************************************************

bool FLevelFloorVoid::NeedCreateFloor(FLevelFloorData* OtherFloor) const
{
	if (OtherFloor->FloorInst->GetType() == ELevelFloorType::Ground || OtherFloor->FloorInst->GetType() == ELevelFloorType::LowerVoid ||
		OtherFloor->FloorInst->GetType() == ELevelFloorType::UnderGroundRoom)
	{
		return true;
	}

	else return false;
	
}

bool FLevelFloorVoid::NeedCreateWall(FLevelFloorData* OtherFloor, EWallType WallType) const
{
	if (!OtherFloor->FloorInst) return false;

	switch (WallType)
	{
	case EWallType::InternalWall:
	case EWallType::OuterWall:

		if (OtherFloor->FloorInst->GetType() == ELevelFloorType::Ground || OtherFloor->FloorInst->GetType() == ELevelFloorType::UnderGroundRoom ||
			OtherFloor->FloorInst->GetType() == ELevelFloorType::LowerVoid)
		{
			return true;
		}

		else return false;

	case EWallType::RoadWall:

		if (OtherFloor->FloorInst->GetType() == ELevelFloorType::Ground || OtherFloor->FloorInst->GetType() == ELevelFloorType::UnderGroundRoom)
		{
			return true;
		}

		else return false;

	default: throw;

	}
	
}



//******************************************************
//FLevelFloorLowerVoid
//*******************************************************

bool FLevelFloorLowerVoid::NeedCreateFloor(FLevelFloorData* OtherFloor) const
{
	if (OtherFloor->FloorInst->GetType() == ELevelFloorType::Ground || OtherFloor->FloorInst->GetType() == ELevelFloorType::Void
		|| OtherFloor->FloorInst->GetType() == ELevelFloorType::UnderGroundRoom || OtherFloor->FloorInst->GetType() == ELevelFloorType::Room)
	{
		return true;
	}

	else return false;
	
}

bool FLevelFloorLowerVoid::NeedCreateWall(FLevelFloorData* OtherFloor, EWallType WallType) const
{
	if (!OtherFloor->FloorInst) return false;

	switch (WallType)
	{
	case EWallType::InternalWall:
	case EWallType::OuterWall:

	{
		ELevelCellType OtherCellType = FloorData.DataStorage.LevelMap.GetCell(OtherFloor->GetCoordinate().X, OtherFloor->GetCoordinate().Y)->CellInst->GetCellType();

		if (!OtherFloor->FloorInst) return false;

		if (OtherFloor->FloorInst->GetType() == ELevelFloorType::Ground)
		{
			return true;
		}

		else if ((OtherFloor->FloorInst->GetType() == ELevelFloorType::Void || OtherFloor->FloorInst->GetType() == ELevelFloorType::Room) &&
			OtherCellType != ELevelCellType::Road && OtherCellType != ELevelCellType::MainRoad && OtherCellType != ELevelCellType::ThroughCell)
		{
			return true;
		}

		else return false;
	}

	case EWallType::RoadWall:

		return false;

	default: throw;
	}
	
}