// Copyright 2018 Pavlov Dmitriy
#pragma once
#include "CoreMinimal.h"
#include "UObject/Class.h"  //For UENUM


class FPlacedLevelRoomLinkedToLevel;
struct FLevelGeneratorSettings;
struct FJointPart;
class FLevelGenRoadBase;
struct FLevelCellBase;
struct FLevelCellProxy;
struct FLevelFloorBase;
struct FPlacedLevelBilding;
struct FLevelCellBase;
struct FLevelFloorData;
struct FLevelCellData;
struct FCellFlloor;
enum class ERoomType : uint8;

UENUM()
enum class ELevelCellType : uint8
{
	Road,
	MainRoad,
	Bilding,
	Tower,
	ThroughCell,
	end
};

FLevelCellBase* ELevelCellTypeCreate(ELevelCellType Type, FLevelCellData& CellData);

struct FLevelCellData
{
public:

	FLevelCellData(const FLevelGeneratorSettings& Settings, FVector2D CellCoordinate, const FDataStorage& DataStorage);

	std::vector<FLevelFloorData> Floors;

	int UpperBaseLevelNum = -1;

	int LowerBaseLevelNum = -1;
	
	bool BaseLevelNumFirstIniciate = false;

	const FVector2D CellCoordinate;

	std::shared_ptr<FPlacedLevelBilding> LevelBilding;

	std::shared_ptr<FLevelCellBase> CellInst = nullptr;

	const FLevelGeneratorSettings& LevelSettings;

	FLevelGenBildingZoneBase* LinkedBildingZone = nullptr;

	std::shared_ptr<FCellFlloor> ZeroFlooreDown;

	std::shared_ptr<FCellFlloor> LastRoofUp;

};

struct FLevelCellBase
{
	FLevelCellBase(FLevelCellData& _CellData) : CellData(_CellData) {}

	virtual ELevelCellType GetCellType() const = 0;

	virtual bool CanEdgeCut() const = 0;

	virtual bool CanRoomPlacedOnCell(ERoomType RoomType) const = 0;

	virtual void SetBaseLevelNum() {}

	virtual int GetBildingCellHeight() const = 0;

	virtual bool CanPlayerSpawn() const = 0;

	virtual bool CanBaseLevelNumUseForNearestCell() const = 0;

	virtual bool NeedCreateFloors() const = 0;

	virtual bool NeedCreateRoadWall() const = 0;

	virtual ~FLevelCellBase() {}

protected:

	FLevelCellData& CellData;

	
};


struct FLevelCellRoad : public  FLevelCellBase
{
	FLevelCellRoad(FLevelCellData& CellData) : FLevelCellBase(CellData) {}

	virtual ELevelCellType GetCellType() const override { return ELevelCellType::Road; }

	virtual bool CanEdgeCut() const override { return false; }

	virtual bool CanRoomPlacedOnCell(ERoomType RoomType) const override;

	virtual int GetBildingCellHeight() const { return 1; }

	virtual bool CanPlayerSpawn() const override { return false; }

	virtual bool CanBaseLevelNumUseForNearestCell() const override { return false; }

	virtual bool NeedCreateFloors() const override { return false; }

	virtual bool NeedCreateRoadWall() const override { return false; }
};


struct FLevelCellMainRoad : public  FLevelCellRoad
{
	FLevelCellMainRoad(FLevelCellData& CellData) : FLevelCellRoad(CellData) {}


	virtual ELevelCellType GetCellType() const override { return ELevelCellType::MainRoad; }

	virtual int GetBildingCellHeight() const { return 0; }

};



struct FLevelCellBilding : public  FLevelCellBase
{
	FLevelCellBilding(FLevelCellData& CellData) : FLevelCellBase(CellData) {}

	virtual ELevelCellType GetCellType() const override { return ELevelCellType::Bilding; }

	virtual bool CanEdgeCut() const override { return true; }

	virtual bool CanRoomPlacedOnCell(ERoomType RoomType) const override;

	virtual void SetBaseLevelNum() override;

	virtual int GetBildingCellHeight() const { return 2; }

	virtual bool CanPlayerSpawn() const override { return true; }

	virtual bool CanBaseLevelNumUseForNearestCell() const override { return true; }

	virtual bool NeedCreateFloors() const override { return true; }

	virtual bool NeedCreateRoadWall() const override { return true; }
};


struct FLevelCellTower : public  FLevelCellBase
{
	FLevelCellTower(FLevelCellData& CellData) : FLevelCellBase(CellData) {}

	virtual ELevelCellType GetCellType() const override { return ELevelCellType::Tower; }

	virtual bool CanEdgeCut() const override { return false; }

	virtual bool CanRoomPlacedOnCell(ERoomType RoomType) const override;

	virtual void SetBaseLevelNum() override;

	virtual int GetBildingCellHeight() const { return 0; }

	virtual bool CanPlayerSpawn() const override { return true; }

	virtual bool CanBaseLevelNumUseForNearestCell() const override { return false; }

	virtual bool NeedCreateFloors() const override { return true; }

	virtual bool NeedCreateRoadWall() const override { return true; }
};

struct FLevelCellThroughCell : public  FLevelCellBase
{

	FLevelCellThroughCell(FLevelCellBilding& LastCell) : FLevelCellBase(LastCell) {}
	
	virtual ELevelCellType GetCellType() const override { return ELevelCellType::ThroughCell; }

	virtual bool CanEdgeCut() const override { return false; }

	virtual bool CanRoomPlacedOnCell(ERoomType RoomType) const override { return false; }

	virtual void SetBaseLevelNum() override;

	virtual int GetBildingCellHeight() const { return 2; }

	virtual bool CanPlayerSpawn() const override { return false; }

	virtual bool CanBaseLevelNumUseForNearestCell() const override { return false; }

	virtual bool NeedCreateFloors() const override { return false; }

	virtual bool NeedCreateRoadWall() const override { return false; }
};

struct FLvevelLayer
{
	FLvevelLayer(std::pair<int, int> _LowerLayerFlorsInterval, std::pair<int, int> _UpperLayerFlorsInterval) :
		UpperLayerFlorsInterval(_UpperLayerFlorsInterval), LowerLayerFlorsInterval(_LowerLayerFlorsInterval)
	{
	}

	std::vector<std::vector<std::shared_ptr<FLevelGraphNode>>> Graph;

	

	std::pair<int, int> UpperLayerFlorsInterval;

	std::pair<int, int> LowerLayerFlorsInterval;

};

