// Copyright 2018 Pavlov Dmitriy
#include "LevelRooms.h"
#include "LevelCell.h"
#include "LevelGeneratorSettings.h"
#include "LevelGraphNode.h"
#include <tuple>
#include "LevelGenerator.h"
#include "LevelGenPluginRuntime_LogCategory.h"
#include "LevelFloor.h"
#include "PlacedLevelRoom.h"
#include "DataStorage.h"
#include "LevelGenBildingZone.h"

//.........................................
//FLevelRoomBase
//.........................................

FLevelRoomBase::FLevelRoomBase(FLevelRoomBase* Room, EDirection BaseDirection, EDirection NewDirection)
{
	RoomSize = RotateVector(Room->RoomSize, BaseDirection, NewDirection);

	RoomRotation = FLevelRoomBase::GetDeltaRotation(BaseDirection, NewDirection);

	RoomActorClass = Room->RoomActorClass;
}


FVector FLevelRoomBase::RotateVector(const FVector& V, EDirection BaseDirection, EDirection NewDirection)
{
	if (BaseDirection == NewDirection)
	{
		return V;
	}
	if (BaseDirection == EDirection::XP)
	{
		if (NewDirection == EDirection::XM)
		{
			return FVector(-V.X,-V.Y,V.Z);
		}
		else if (NewDirection == EDirection::YP)
		{
			return FVector(-V.Y, V.X, V.Z); //Clockwise
		}
		else if (NewDirection == EDirection::YM)
		{
			return FVector(V.Y, -V.X, V.Z); // counterclock-wise
		}

	}
	else if (BaseDirection == EDirection::XM)
	{
		if (NewDirection == EDirection::XP)
		{
			return FVector(-V.X, -V.Y, V.Z);;
		}
		else if (NewDirection == EDirection::YP)
		{
			return FVector(V.Y, -V.X, V.Z); // counterclock-wise
			
		}
		else if (NewDirection == EDirection::YM)
		{
			return FVector(-V.Y, V.X, V.Z); //Clockwise
		}
	}
	else if (BaseDirection == EDirection::YP)
	{
		if (NewDirection == EDirection::YM)
		{
			return FVector(-V.X, -V.Y, V.Z);
		}
		else if (NewDirection == EDirection::XP)
		{
			return FVector(V.Y, -V.X, V.Z); // counterclock-wise
		}
		else if (NewDirection == EDirection::XM)
		{
			return FVector(-V.Y, V.X, V.Z); //Clockwise
			
		}
	}
	else if (BaseDirection == EDirection::YM)
	{
		if (NewDirection == EDirection::YP)
		{
			return FVector(-V.X, -V.Y, V.Z);
		}
		else if (NewDirection == EDirection::XP)
		{
			return FVector(-V.Y, V.X, V.Z); //Clockwise
		}
		else if (NewDirection == EDirection::XM)
		{
			return FVector(V.Y, -V.X, V.Z); // counterclock-wise
			
		}
	}

	throw;
}

int FLevelRoomBase::GetDeltaRotation(EDirection BaseDirection, EDirection NewDirection)
{
	if (BaseDirection == NewDirection)
	{
		return 0;
	}
	if (BaseDirection == EDirection::XP)
	{
		if (NewDirection == EDirection::XM)
		{
			return 180;
		}
		else if (NewDirection == EDirection::YP)
		{
			return 90; // counterclock-wise
		}
		else if (NewDirection == EDirection::YM)
		{
			return 270; //Clockwise
		}

	}
	else if (BaseDirection == EDirection::XM)
	{
		if (NewDirection == EDirection::XP)
		{
			return 180;
		}
		else if (NewDirection == EDirection::YP)
		{
			return 270; //Clockwise
		}
		else if (NewDirection == EDirection::YM)
		{
			return 90; // counterclock-wise
		}
	}
	else if (BaseDirection == EDirection::YP)
	{
		if (NewDirection == EDirection::YM)
		{
			return 180;
		}
		else if (NewDirection == EDirection::XP)
		{
			return 270; //Clockwise
		}
		else if (NewDirection == EDirection::XM)
		{
			return 90; // counterclock-wise
		}
	}
	else if (BaseDirection == EDirection::YM)
	{
		if (NewDirection == EDirection::YP)
		{
			return 180;
		}
		else if (NewDirection == EDirection::XP)
		{
			return 90; // counterclock-wise
		}
		else if (NewDirection == EDirection::XM)
		{
			return 270; //Clockwise
		}
	}
	throw;
}

FRotator FLevelRoomBase::GetNewRotation(FRotator R, EDirection BaseDirection, EDirection NewDirection)
{
	return R + FRotator(0, GetDeltaRotation(BaseDirection, NewDirection), 0);
}






FVector FLevelRoomBase::GetRoomBegin(FVector RoomCoordinate) const
{
	
	return FVector(
		RoomSize.X > 0 ? RoomCoordinate.X : RoomCoordinate.X - abs(RoomSize.X) + 1,
		RoomSize.Y > 0 ? RoomCoordinate.Y : RoomCoordinate.Y - abs(RoomSize.Y) + 1,
		RoomCoordinate.Z);
}

FVector FLevelRoomBase::GetRoomEnd(FVector RoomCoordinate) const
{
	return FVector(
		RoomSize.X > 0 ? RoomCoordinate.X + RoomSize.X : RoomCoordinate.X + 1,
		RoomSize.Y > 0 ? RoomCoordinate.Y + RoomSize.Y : RoomCoordinate.Y + 1,
		RoomCoordinate.Z + RoomSize.Z);

}

void FLevelRoomBase::GetWalls(std::vector<const FJointSlot*>& _RoomWalls) const
{
	_RoomWalls.reserve(RoomWalls.Num());

	for (int i = 0; i < RoomWalls.Num(); i++)
	{
		_RoomWalls.push_back(&RoomWalls[i]);
	}

}


//.........................................
//FLevelRoomNode
//.........................................

bool FLevelRoomNode::IsRoomFit(FVector RoomCoordinate, const FDataStorage& DataStorage)
{

	FVector Begin = GetRoomBegin(RoomCoordinate);
	FVector End = GetRoomEnd(RoomCoordinate);

	for (int i = Begin.X; i < End.X; i++)
	{
	
		for (int j = Begin.Y; j < End.Y; j++)
		{
			for (int k = Begin.Z; k < End.Z; k++)
			{
				
				if (!IsCellFit(i, j, k, DataStorage))
				{
					return false;
				}
			}

		}
	}
	return true;
}

void FLevelRoomNode::GetJointSlots(std::vector<const FJointSlot*>& _JoinSlots) const
{
	_JoinSlots.reserve(JoinSlots.Num());
	
	for (int i = 0; i < JoinSlots.Num(); i++)
	{
		_JoinSlots.push_back(&JoinSlots[i]);
	}

}



bool FLevelRoomNode::IsCellFit(int X, int Y, int Z, const FDataStorage& DataStorage)
{

	FLevelCellData& CurrentCell = *DataStorage.LevelMap.GetCell(X, Y);
	
	if (!CurrentCell.CellInst)
	{
		return false;
	} 
	if(!CurrentCell.CellInst->CanRoomPlacedOnCell(GetType()))
	{
		return false;
	}
	
	if (CurrentCell.Floors[Z].PlasedRoom != nullptr)

	{
		return false;
	}
		

	if (CurrentCell.Floors[Z].JointPart.size() != 0)
	{
		return false;
	}
	

	return true;
}


//.........................................
//FLevelRoomLink
//.........................................

FLevelRoomLink::FLevelRoomLink(FLevelRoomLink* Room, EDirection BaseDirection, EDirection NewDirection) : FLevelRoomBase(Room, BaseDirection, NewDirection)
{
	InSlot = Room->InSlot;
	OutSlot = Room->OutSlot;

	for (int i = 0; i < Room->JoinSlots.Num(); i++)
	{

		JoinSlots.Add(FJointSlot(RotateVector(Room->JoinSlots[i].RelativeCoordinate, BaseDirection, NewDirection),
			EDirectionFromVector(RotateVector(EDirectionGetVector(Room->JoinSlots[i].Direction), BaseDirection, NewDirection))));

	}


	for (int i = 0; i < Room->RoomWalls.Num(); i++)
	{
		RoomWalls.Add(FJointSlot(RotateVector(Room->RoomWalls[i].RelativeCoordinate, BaseDirection, NewDirection),
			EDirectionFromVector(RotateVector(EDirectionGetVector(Room->RoomWalls[i].Direction), BaseDirection, NewDirection))));
	}
}

bool FLevelRoomLink::IsRoomFit(const FVector& RoomCoordinate, const FJointPart* InJointPart, const FJointPart* OutJointPart,
	const FDataStorage& DataStorage, const FLevelGeneratorSettings& LevelSettings)
{

	FVector Begin = FLevelRoomBase::GetRoomBegin(RoomCoordinate);
	FVector End = FLevelRoomBase::GetRoomEnd(RoomCoordinate);
	

	for (int i = Begin.X; i < End.X; i++)
	{
		for (int j = Begin.Y; j < End.Y; j++)
		{
			for (int k = Begin.Z; k < End.Z; k++)
			{
				 
				if (!IsCellFit(i, j, k, InJointPart, OutJointPart, DataStorage, LevelSettings)) return false;
				
			}

		}
	}
	return true;
}


bool FLevelRoomLink::IsCellFit(int X, int Y, int Z, const FJointPart* InJointPart, const FJointPart* OutJointPart,
	const FDataStorage& DataStorage, const FLevelGeneratorSettings& LevelSettings)
{
	FLevelCellData& CurrentCell = *DataStorage.LevelMap.GetCell(X, Y);

	if (Z < 0 || Z >= LevelSettings.FloorNum) return false;

	if (!CurrentCell.CellInst)  return false;

	if (!CurrentCell.CellInst->CanRoomPlacedOnCell(GetType())) return false;

	if (CurrentCell.Floors[Z].PlasedRoom) return false;
	
	if (CurrentCell.LinkedBildingZone && CurrentCell.LinkedBildingZone->GetState() == EBildingZoneState::FlooreAndWallCreated) return false;

	for (int i = 0; i < CurrentCell.Floors[Z].JointPart.size(); i++)
	{
		if (CurrentCell.Floors[Z].FloorInst)  return false;

		if (CurrentCell.Floors[Z].JointPart[i] != InJointPart && CurrentCell.Floors[Z].JointPart[i] != OutJointPart) return false;

	}

	return true;
}

void FLevelRoomLink::GetJointSlots(std::vector<const FJointSlot*>& _JoinSlots) const
{
	_JoinSlots.reserve(JoinSlots.Num());
	
	for (int i = 0; i < JoinSlots.Num(); i++)
	{
		_JoinSlots.push_back(&JoinSlots[i]);
	}


}


//.........................................
//FLevelRoomRoadLink
//.........................................
bool FLevelRoomRoadLink::IsRoomFit(const FVector& RoomCoordinate, const FJointPart* InJointPart, const FJointPart* OutJointPart,
	const FDataStorage& DataStorage, const FLevelGeneratorSettings& LevelSettings)
{
	if (!FLevelRoomLink::IsRoomFit(RoomCoordinate, InJointPart, OutJointPart, DataStorage, LevelSettings))
	{
		return false;
	}

	FVector AbsoluteOutPortalCoordinate = RoomCoordinate + GetOutPutSlot()->RelativeCoordinate + EDirectionGetVector(EDirectionInvert(GetOutPutSlot()->Direction));
	

	FLevelCellData& OutPutCell = *DataStorage.LevelMap.GetCell(AbsoluteOutPortalCoordinate.X, AbsoluteOutPortalCoordinate.Y);

	if (!OutPutCell.CellInst || !OutPutCell.CellInst->CanRoomPlacedOnCell(ERoomType::NodeRoom))
	{
		return false;
	}

	FVector AbsoluteInPortalCoordinate = RoomCoordinate + EDirectionGetVector(EDirectionInvert(GetInPutSlot()->Direction));
	FLevelCellData& InPutCell = *DataStorage.LevelMap.GetCell(AbsoluteInPortalCoordinate.X, AbsoluteInPortalCoordinate.Y);

	if (!InPutCell.CellInst || !InPutCell.CellInst->CanRoomPlacedOnCell(ERoomType::NodeRoom))
	{
		return false;
	}

	return true;
}


//.........................................
//FLevelRoomTerraceLink
//.........................................
bool FLevelRoomTerraceLink::IsRoomFit(const FVector& RoomCoordinate, const FJointPart* InJointPart, const FJointPart* OutJointPart,
	const FDataStorage& DataStorage, const FLevelGeneratorSettings& LevelSettings)
{
	

	FVector Begin = FLevelRoomBase::GetRoomBegin(RoomCoordinate);
	FVector End = FLevelRoomBase::GetRoomEnd(RoomCoordinate);

	FLevelGenBildingZoneBase* CurrentBildingPointer = nullptr;

	for (int i = Begin.X; i < End.X; i++)
	{
		for (int j = Begin.Y; j < End.Y; j++)
		{
			FLevelCellData& CurrentCell = *DataStorage.LevelMap.GetCell(i, j);
			if (CurrentCell.LinkedBildingZone)
			{
				if (!CurrentBildingPointer)
				{
					CurrentBildingPointer = CurrentCell.LinkedBildingZone;
				}
				else
				{
					if (CurrentBildingPointer != CurrentCell.LinkedBildingZone) return false;
						
				}
			}
			else
			{
				return false;
			}
			for (int k = Begin.Z; k < End.Z; k++)
			{

				if (!IsCellFit(i, j, k, InJointPart, OutJointPart, DataStorage, LevelSettings)) return false;

			}
		}
	}

	return true;

	
}