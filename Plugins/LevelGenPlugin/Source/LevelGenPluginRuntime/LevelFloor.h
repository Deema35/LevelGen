// Copyright 2018 Pavlov Dmitriy
#pragma once
#include "CoreMinimal.h"
#include "UObject/Class.h"  //For UENUM
#include <vector>
#include <memory>
#include "LevelGenCore.h" 

class FPlacedLevelRoomLinkedToLevel;
struct FJointPart;
struct FLevelFloorBase;
struct FLevelFloorProxy;
struct FDataStorage;
struct FLevelFloorData;
struct FLevelGeneratorMaterialSettings;
struct FCellWall;
class FProceduralFigureRectangle;

struct FCellWallBase
{
	
	std::map<const FLevelGeneratorMaterialSettings*, std::shared_ptr<FProceduralFigureBase>> WallSegments;
	
};

struct FWallCorner
{
	FWallCorner() = default;
	FWallCorner(EWallCornerType _Type, FCellWallBase* _OtherWall) : Type(_Type), OtherWall(_OtherWall) {}
	EWallCornerType Type = EWallCornerType::end;
	FCellWallBase* OtherWall = nullptr;
};

enum class EWallType : uint8
{
	RoadWall,
	OuterWall,
	InternalWall
};

struct FCellWall : public FCellWallBase
{
	FCellWall(EWallType _WallType) : WallType(_WallType) {}
	
	FWallCorner Up;
	FWallCorner Right;
	FWallCorner Down;

	EWallType WallType;
	
};

struct FCellFlloor : public FCellWallBase
{
	FCellFlloor() { FloorJoints.resize(4); }

	std::vector<FWallCorner> FloorJoints;

	
};

UENUM()
enum class ELevelFloorType : uint8
{
	Room,
	Ground,
	UnderGroundRoom,
	Void,
	LowerVoid,
	end
};

enum class EFloorName : uint8
{
	Floore,
	Roof
};

FLevelFloorBase* ELevelFloorTypeCreate(ELevelFloorType FloorType, FLevelFloorData& FloorData);

struct FLevelFloorData
{
public:

	FLevelFloorData(const FDataStorage& _DataStorage) : DataStorage(_DataStorage)
	{
		FloorWalls.resize(2);
	}

	FPlacedLevelRoomLinkedToLevel* PlasedRoom = nullptr;

	std::vector<FJointPart*> JointPart;

	std::shared_ptr<FLevelFloorBase> FloorInst;

	FVector GetCoordinate() { return FloorCoordinate; }

	std::map<EDirection, std::shared_ptr<FCellWall>> Walls;

	std::vector<std::shared_ptr<FCellFlloor>> FloorWalls;

	const FDataStorage& DataStorage;

	void SetCoordinate(FVector _FloorCoordinate) { FloorCoordinate = _FloorCoordinate; }

private:

	FVector FloorCoordinate;
};




struct FLevelFloorBase
{
	FLevelFloorBase(FLevelFloorData& _FloorData) : FloorData(_FloorData) {}

	virtual ~FLevelFloorBase() {}

	virtual ELevelFloorType GetType() const = 0;

	virtual bool NeedCreateFloorOnZeroFloor() const = 0;

	virtual bool NeedCreateRoofOnLastFloor() const = 0;

	virtual bool NeedCreateFloor(FLevelFloorData* OtherFloor) const = 0;

	virtual bool NeedCreateWall(FLevelFloorData* OtherFloor, EWallType WallType) const = 0;

	virtual bool CanPlayerSpawn() const = 0;

	virtual bool NeedCreateWallFromRoom() const = 0;

	FLevelFloorData& FloorData;
};


struct FLevelFloorRoom : public FLevelFloorBase
{
	FLevelFloorRoom(FLevelFloorData& FloorData) : FLevelFloorBase(FloorData) {}


	virtual ELevelFloorType GetType() const override { return ELevelFloorType::Room; }

	virtual bool NeedCreateFloorOnZeroFloor() const override { return true; }

	virtual bool NeedCreateRoofOnLastFloor() const override { return false; }

	virtual bool NeedCreateFloor(FLevelFloorData* OtherFloor) const override;

	virtual bool NeedCreateWall(FLevelFloorData* OtherFloor, EWallType WallType) const override;
	
	virtual bool CanPlayerSpawn() const override { return true; }

	virtual bool NeedCreateWallFromRoom() const override { return true; }
};

struct FLevelFloorGround : public FLevelFloorBase
{
	FLevelFloorGround(FLevelFloorData& FloorData) : FLevelFloorBase(FloorData) {}


	virtual ELevelFloorType GetType() const override { return ELevelFloorType::Ground; }

	virtual bool NeedCreateFloorOnZeroFloor() const override { return false; }

	virtual bool NeedCreateRoofOnLastFloor() const override { return true; }

	virtual bool NeedCreateFloor(FLevelFloorData* OtherFloor) const override { return false; }

	virtual bool NeedCreateWall(FLevelFloorData* OtherFloor, EWallType WallType) const override { return false; }

	virtual bool CanPlayerSpawn() const override { return false; }

	virtual bool NeedCreateWallFromRoom() const override { return false; }
};

struct FLevelFloorUnderGroundRoom : public FLevelFloorBase
{
	FLevelFloorUnderGroundRoom(FLevelFloorData& FloorData) : FLevelFloorBase(FloorData) {}


	virtual ELevelFloorType GetType() const override { return ELevelFloorType::UnderGroundRoom; }

	virtual bool NeedCreateFloorOnZeroFloor() const override { return true; }

	virtual bool NeedCreateRoofOnLastFloor() const override { return true; }

	virtual bool NeedCreateFloor(FLevelFloorData* OtherFloor) const override;

	virtual bool NeedCreateWall(FLevelFloorData* OtherFloor, EWallType WallType) const override;

	virtual bool CanPlayerSpawn() const override { return true; }

	virtual bool NeedCreateWallFromRoom() const override { return true; }
};

struct FLevelFloorVoid : public FLevelFloorBase
{
	FLevelFloorVoid(FLevelFloorData& FloorData) : FLevelFloorBase(FloorData) {}


	virtual ELevelFloorType GetType() const override { return ELevelFloorType::Void; }

	virtual bool NeedCreateFloorOnZeroFloor() const override { return true; }

	virtual bool NeedCreateRoofOnLastFloor() const override { return false; }

	virtual bool NeedCreateFloor(FLevelFloorData* OtherFloor) const override;

	virtual bool NeedCreateWall(FLevelFloorData* OtherFloor, EWallType WallType) const override;

	virtual bool CanPlayerSpawn() const override { return false; }

	virtual bool NeedCreateWallFromRoom() const override { return true; }
};

struct FLevelFloorLowerVoid : public FLevelFloorBase
{
	FLevelFloorLowerVoid(FLevelFloorData& FloorData) : FLevelFloorBase(FloorData) {}


	virtual ELevelFloorType GetType() const override { return ELevelFloorType::LowerVoid; }

	virtual bool NeedCreateFloorOnZeroFloor() const override { return false; }

	virtual bool NeedCreateRoofOnLastFloor() const override { return true; }

	virtual bool NeedCreateFloor(FLevelFloorData* OtherFloor) const override;

	virtual bool NeedCreateWall(FLevelFloorData* OtherFloor, EWallType WallType) const override;

	virtual bool CanPlayerSpawn() const override { return false; }

	virtual bool NeedCreateWallFromRoom() const override { return true; }
};