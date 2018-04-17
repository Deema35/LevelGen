// Copyright 2018 Pavlov Dmitriy
#include "PlacedLevelRoom.h"
#include "LevelRooms.h"
#include "LevelFloor.h"
#include "LevelCell.h"
#include "DataStorage.h"

//.........................................
//FJointPart
//.........................................

FJointPart* FJointPart::GetLinkedJointPart() const
{
	return OwnerJoint->GetLinkedJointPart(this);
}

//.........................................
//FPlacedLevelRoomBase
//.........................................

FPlacedLevelRoomBase::FPlacedLevelRoomBase(FLevelRoomBase* _Room, FVector _StartCoordinate) : Room(_Room), StartCoordinate(_StartCoordinate)
{
	std::vector<const FJointSlot*> JointSlotLoc;

	Room->GetJointSlots(JointSlotLoc);

	if (JointSlotLoc.size() == 0) throw;

	for (int i = 0; i < JointSlotLoc.size(); i++)
	{
		JointSlots.push_back(FPlacedRoomJointSlot(JointSlotLoc[i]));
	}
}

void FPlacedLevelRoomBase::CreateJointForBlankSlots()
{
	FVector AbsoluteCoordinate;
	
	for (int i = 0; i < JointSlots.size(); i++)
	{
		
		if (!JointSlots[i].ConnectedJoinPart)
		{
			
			AbsoluteCoordinate = JointSlots[i].JointSlot->RelativeCoordinate + StartCoordinate;
			JointStorage.push_back(std::shared_ptr<FJoint>(new FJoint(JointSlots[i], AbsoluteCoordinate, this)));

			JointSlots[i].ConnectedJoinPart = &(JointStorage.back()->First);

		}

	}

	
}

FVector FPlacedLevelRoomBase::GetRoomBegin() const
{ 
	return Room->GetRoomBegin(StartCoordinate);
}

FVector FPlacedLevelRoomBase::GetRoomEnd() const
{ 
	return Room->GetRoomEnd(StartCoordinate);
}



//.........................................
//FSearchGraphNode
//.........................................


void FSearchGraphNode::GetRoomCellsCoordinats(std::vector<FVector>& CellCoordinats) const
{
	FVector Begin = GetRoomBegin();
	FVector End = GetRoomEnd();

	for (int i = Begin.X; i < End.X; i++)
	{

		for (int j = Begin.Y; j < End.Y; j++)
		{
			for (int k = Begin.Z; k < End.Z; k++)
			{
				CellCoordinats.push_back(FVector(i, j, k));

			}

		}
	}
}


const FPlacedRoomJointSlot* FSearchGraphNode::GetOutPutJointSlot() const
{
	return  &JointSlots[OutSlot];
	
}

FPlacedRoomJointSlot* FSearchGraphNode::GetOutPutJointSlot()
{
	return  const_cast<FPlacedRoomJointSlot*>(static_cast<const FSearchGraphNode*>(this)->GetOutPutJointSlot());
}

const FPlacedRoomJointSlot* FSearchGraphNode::GetInPutJointSlot() const
{
	
	return  &JointSlots[InSlot];
}

FPlacedRoomJointSlot* FSearchGraphNode::GetInPutJointSlot()
{
	return  const_cast<FPlacedRoomJointSlot*>(static_cast<const FSearchGraphNode*>(this)->GetInPutJointSlot());
}


//.........................................
//FPlacedLevelRoom
//.........................................



bool FPlacedLevelRoomLinkedToLevel::FindNearestJointPair(std::pair<FJointPart*, FJointPart*>& JointParts, const FPlacedLevelRoomLinkedToLevel& SecondRoom)
{
	if (!&SecondRoom) return false;

	bool IsBestPortalFinds = false;

	int BestPortal_1 = 0;
	int BestPortal_2 = 0;
	float LastDistance = 0;
	FVector PortalCoordinate;

	std::vector<FPlacedRoomJointSlot>& FirstRoomSlots = GetJointSlots();
	

	const std::vector<FPlacedRoomJointSlot>& SecondRoomSlots = SecondRoom.GetJointSlots();
	
	
	for (int i = 0; i < FirstRoomSlots.size(); i++)
	{
		for (int j = 0; j < SecondRoomSlots.size(); j++)
		{
			
				
			if (FirstRoomSlots[i].ConnectedJoinPart && !FirstRoomSlots[i].ConnectedJoinPart->GetLinkedJointPart()->IsBusy())
			{
				
				if (SecondRoomSlots[j].ConnectedJoinPart && !SecondRoomSlots[j].ConnectedJoinPart->GetLinkedJointPart()->IsBusy())
				{
					
					float CurrentDist = FVector::Dist(FirstRoomSlots[i].ConnectedJoinPart->GetLinkedJointPart()->GetCoordinate(), SecondRoomSlots[j].ConnectedJoinPart->GetLinkedJointPart()->GetCoordinate());
					

					
					if (CurrentDist < LastDistance || !IsBestPortalFinds)
					{
						LastDistance = CurrentDist;
						BestPortal_1 = i;
						BestPortal_2 = j;
						IsBestPortalFinds = true;

					}

				}
			}
			

		}
	}

	if (!IsBestPortalFinds) return false;
	
	JointParts.first = FirstRoomSlots[BestPortal_1].ConnectedJoinPart->GetLinkedJointPart();
	
	JointParts.second = SecondRoomSlots[BestPortal_2].ConnectedJoinPart->GetLinkedJointPart();
	return true;
}

bool FPlacedLevelRoomLinkedToLevel::IsRoomConectitToPlace(FVector Coordinate, const FDataStorage& DataStorage)
{
	
	FLevelCellData& Cell = *DataStorage.LevelMap.GetCell(Coordinate.X, Coordinate.Y);


	for (int j = 0; j < Cell.Floors[Coordinate.Z].JointPart.size(); j++)
	{
		const FJointPart* CurrentJointPart = Cell.Floors[Coordinate.Z].JointPart[j];

		

		if (CurrentJointPart->GetJoinRoom()  && CurrentJointPart->GetLinkedJointPart() && CurrentJointPart->GetLinkedJointPart()->GetJoinRoom() == this)
		{
			return true;
		}
		
	}
	return false;
	
}


void FPlacedLevelRoomLinkedToLevel::LinkedRoomToLevelCells(const FDataStorage& DataStorage)
{
	FVector Begin = GetRoomBegin();
	FVector End = GetRoomEnd();

	for (int i = Begin.X; i < End.X; i++)
	{
		for (int j = Begin.Y; j < End.Y; j++)
		{
			for (int k = Begin.Z; k < End.Z; k++)
			{
				
				DataStorage.LevelMap.GetCell(i, j)->Floors[k].PlasedRoom = this;
			}
		}
	}

}

void FPlacedLevelRoomLinkedToLevel::LinkJointsToLevelCells(const FDataStorage& DataStorage)
{
	std::vector<FPlacedRoomJointSlot>& Slots = GetJointSlots();
	

	for (int i = 0; i < Slots.size(); i++)
	{

		if (Slots[i].ConnectedJoinPart)
		{
			DataStorage.LevelMap.GetCell(FVector2D(Slots[i].ConnectedJoinPart->GetCoordinate()))->Floors[Slots[i].ConnectedJoinPart->GetCoordinate().Z].JointPart.push_back(Slots[i].ConnectedJoinPart);

			
			DataStorage.LevelMap.GetCell(FVector2D(Slots[i].ConnectedJoinPart->GetLinkedJointPart()->GetCoordinate()))->
				Floors[Slots[i].ConnectedJoinPart->GetLinkedJointPart()->GetCoordinate().Z].JointPart.push_back(Slots[i].ConnectedJoinPart->GetLinkedJointPart());

		}

	}

}




bool FPlacedLevelRoomLinkedToLevel::IsNeedCreateWall(FVector OtherCellCoordinate)
{
	
	std::vector<const FJointSlot*> RoomWalls;

	GetRoom()->GetWalls(RoomWalls);

	for (int i = 0; i < RoomWalls.size(); i++)
	{
		if (GetStartCoordinate() + RoomWalls[i]->RelativeCoordinate + EDirectionGetVector(EDirectionInvert(RoomWalls[i]->Direction)) == OtherCellCoordinate)
		{

			return true;
		}
	}

	return false;
}