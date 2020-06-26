// Copyright 2018 Pavlov Dmitriy
#pragma once

#include "CoreMinimal.h"
#include <vector>
#include <memory>
#include <utility>
#include <tuple>
#include <map>
#include "LevelGenCore.h"

class FPlacedLevelRoomBase;
struct FJointSlot;
struct FDataStorage;
struct FJoint;

struct FPlacedRoomJointSlot
{
	FPlacedRoomJointSlot(const FJointSlot* _JointSlot) : JointSlot(_JointSlot)
	{}

	const FJointSlot* JointSlot = nullptr;

	FJointPart* ConnectedJoinPart = nullptr;
};

struct FJointPart
{
public:
	FJointPart(FVector _Coordinate, EDirection _Direction, FPlacedLevelRoomBase* _JoinRoom, FJoint* _OwnerJoint) :
		Coordinate(_Coordinate), JoinRoom(_JoinRoom), Direction(_Direction),  OwnerJoint(_OwnerJoint), bBusy(_JoinRoom != nullptr)
	{ }
	

	bool IsPartFitForSlot(const FPlacedRoomJointSlot& Slot) const
	{
		if (Direction >= EDirection::end) throw;
		
		if (Slot.JointSlot->Direction == Direction)
		{
			return true;
		}

		return false;
	}

	EDirection GetDirection() const { return Direction; }

	void SetBusy() { bBusy = true; }

	bool IsBusy() const { return bBusy; }

	FVector GetCoordinate() const { return Coordinate; }

	FPlacedLevelRoomBase* GetJoinRoom() const { return JoinRoom; }

	void SetJointRoom(FPlacedLevelRoomBase* _JoinRoom) { JoinRoom = _JoinRoom; }

	inline FJointPart* GetLinkedJointPart() const;

private:

	FVector Coordinate;
	FPlacedLevelRoomBase* JoinRoom = nullptr;
	EDirection Direction;
	FJoint* OwnerJoint = nullptr;

	bool bBusy = false;

};

struct FJoint
{
	FJoint(const FPlacedRoomJointSlot& PortalSlot, FVector _Coordinate, FPlacedLevelRoomBase* _Room) : First(_Coordinate, PortalSlot.JointSlot->Direction, _Room, this),
		Second(_Coordinate + EDirectionGetVector(EDirectionInvert(PortalSlot.JointSlot->Direction)), EDirectionInvert(PortalSlot.JointSlot->Direction), nullptr, this)
	{}


	FJointPart* GetLinkedJointPart(const FJointPart* FirstJointPart)
	{ 
		if (FirstJointPart == &First) return &Second;
		else if (FirstJointPart == &Second) return &First;
		else
		{
			
			throw;
		}
			
		
	}
	FJointPart First;

	FJointPart Second;

};

class FPlacedLevelRoomBase
{
public:
	FPlacedLevelRoomBase(FLevelRoomBase* _Room, FVector _StartCoordinate);
	

	FLevelRoomBase* GetRoom() const { return Room; }

	virtual ~FPlacedLevelRoomBase() {};

	FVector GetStartCoordinate() const { return StartCoordinate; }

	void CreateJointForBlankSlots();

	std::vector<FPlacedRoomJointSlot>& GetJointSlots() {return JointSlots;}

	const std::vector<FPlacedRoomJointSlot>& GetJointSlots() const { return JointSlots; }




protected:

	FVector GetRoomBegin() const;

	FVector GetRoomEnd() const;

protected:
	std::vector<std::shared_ptr<FJoint>> JointStorage;

	std::vector<FPlacedRoomJointSlot> JointSlots;

	FLevelRoomBase* Room;

	FVector StartCoordinate;

};



class FSearchGraphNode : public FPlacedLevelRoomBase
{
public:
	FSearchGraphNode(FLevelRoomLink* Room, FJointPart& JointPart, FVector StartCoordinate, std::weak_ptr<FSearchGraphNode> _LastNode)
		: FPlacedLevelRoomBase(Room, StartCoordinate), LastNode(_LastNode),  InSlot(Room->GetInSlotNum()), OutSlot(Room->GetOutSlotNum())
	{
		 
		if (!JointPart.IsPartFitForSlot(*GetInPutJointSlot()))
		{
			throw;
		}
		GetInPutJointSlot()->ConnectedJoinPart = &JointPart;

	}


	void GetRoomCellsCoordinats(std::vector<FVector>& CellCoordinats) const;

	const FPlacedRoomJointSlot* GetOutPutJointSlot() const;

	FPlacedRoomJointSlot* GetOutPutJointSlot();

	const FPlacedRoomJointSlot* GetInPutJointSlot() const;

	FPlacedRoomJointSlot* GetInPutJointSlot();

	std::weak_ptr<FSearchGraphNode> LastNode;

	bool IsProcessed() { return Processed; }

	void SetProcessed() { Processed = true; }

private:

	bool Processed = false;

	const int InSlot = 0;

	const int OutSlot = 0;
};

class FPlacedLevelRoomLinkedToLevel : public FPlacedLevelRoomBase
{

public:
	FPlacedLevelRoomLinkedToLevel(const FSearchGraphNode& SearchGraphNode, FVector _StartCoordinate, const FDataStorage& DataStorage)
		: FPlacedLevelRoomBase(SearchGraphNode.GetRoom(), _StartCoordinate)
	{
		if (!SearchGraphNode.GetRoom()) throw;
				
		LinkedRoomToLevelCells(DataStorage);
		CreateJointForBlankSlots();
		LinkJointsToLevelCells(DataStorage);
	}

	FPlacedLevelRoomLinkedToLevel(FLevelRoomBase* Room, FVector _StartCoordinate, const FDataStorage& DataStorage) : FPlacedLevelRoomBase(Room, _StartCoordinate)
	{
		if (!Room) throw;

		LinkedRoomToLevelCells(DataStorage);
		CreateJointForBlankSlots();
		LinkJointsToLevelCells(DataStorage);

	}

	bool FindNearestJointPair(std::pair<FJointPart*, FJointPart*>& JointParts, const FPlacedLevelRoomLinkedToLevel& SecondRoom);

	bool IsRoomConectitToPlace(FVector Coordinate, const FDataStorage& DataStorage);

	bool IsNeedCreateWall(FVector OtherCellCoordinate);


private:

	void LinkedRoomToLevelCells(const FDataStorage& DataStorage);

	void LinkJointsToLevelCells(const FDataStorage& DataStorage);
};