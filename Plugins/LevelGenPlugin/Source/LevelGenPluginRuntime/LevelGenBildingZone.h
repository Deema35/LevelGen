// Copyright 2018 Pavlov Dmitriy
#pragma once
#include "CoreMinimal.h"
#include <vector>
#include <memory>
#include <map>

struct FLevelGeneratorSettings;
struct FPlacedLevelTower;
struct FDataStorage;
class ALevelGenerator;
class FProceduralFigurBuffer;

enum class EBildingZoneState : uint8
{
	Created,
	FlooreAndWallCreated,
	UnPlasebel

};

enum class EBildingZoneLinkState
{
	Create,
	LinksCreate
	
};

struct FPossibleLink
{
	FPossibleLink(std::pair<FLevelGraphNodeBase*, FLevelGraphNodeBase*> _LinkNode, std::pair<FJointPart*, FJointPart*> _JointParts) : LinkNode(_LinkNode), JointParts(_JointParts) {}

	std::pair<FLevelGraphNodeBase*, FLevelGraphNodeBase*> LinkNode;
	std::pair<FJointPart*, FJointPart*> JointParts;
};

class FLevelGenBildingZoneLink
{
public:
	FLevelGenBildingZoneLink(EDirection _LinkDirection, int _LinkBegin, int _LinkSize,  std::pair<int, int> _LinkInterval, int LayersNum)
		: LinkDirection(_LinkDirection), LinkInterval(_LinkInterval), LinkBegin(_LinkBegin), LinkSize(_LinkSize)
	{
	}

	EDirection LinkDirection;

	std::pair<int, int> LinkInterval;

	int LinkBegin;

	int LinkSize;

	std::vector<std::shared_ptr<FLevelGraphLink>> Links;


	EBildingZoneLinkState GetState() { return BildingZoneLinkState; }

	void SetState(EBildingZoneLinkState NewState) { BildingZoneLinkState = NewState; }

private:

	EBildingZoneLinkState BildingZoneLinkState = EBildingZoneLinkState::Create;
};

class FLevelGenBildingZoneBase
{
public:
	FLevelGenBildingZoneBase(bool UnPlacebelBilding, FVector2D Start, FVector2D End, FDataStorage& DataStorage,
		const FLevelGeneratorSettings& _LevelSettings, const ALevelGenerator& _LevelGenerator);

	virtual ~FLevelGenBildingZoneBase() {}

	void CreateGraphLayers();
		

	FVector2D GetBeginCoordinate() const { return Coordinate; }

	FVector2D GetSenterCoordinate() const { return Coordinate + (Size - 1) * 0.5; }

	FVector2D GetSize() const { return Size; }

	std::vector<std::vector<std::shared_ptr<FPlacedLevelTower>>>& GetTowers() { return BildingTowers; }

	virtual void ConnectBildingToLevelCells(FDataStorage& DataStorage) = 0;

	

	EBildingZoneState GetState() const { return BildingZoneState; }

	void SetState(EBildingZoneState _BildingZoneState) { BildingZoneState = _BildingZoneState; }

	std::pair<int, int> GetCoordinateRange(EDirection Direction);

	void AddLink(FLevelGenBildingZoneBase* BildingZone, std::shared_ptr<FLevelGenBildingZoneLink> Link) { Links.insert(std::make_pair(BildingZone, Link)); }

	std::map<FLevelGenBildingZoneBase*, std::shared_ptr<FLevelGenBildingZoneLink>>& GetLinks() { return Links; }

	void GetFasedeWallCellsCoordinats(std::vector<FVector2D>& WallCells, EDirection Direction);

	void CreateLinksBetweenBildingZone();

	void CreateNods();

	void SetNodsRoom();

	void GetPossibreLinksBetweenBildingZone(std::map<float, FPossibleLink>& PossibleLinks, FLevelGenBildingZoneBase* OtherBildingZone, int FloorNum);

	void CreateLinksRooms();

	int GetGraphLayersNum() { return GraphLayers.size(); }

	void SetBaseLevelNum();

	void SmoothBildingZoneRoofs();

	void CreateBaseWalls();

	virtual void CreateProceduralFigure();

	void EdgeCut();

	const std::vector<FLvevelLayer>& GetGraphLayers() const { return GraphLayers; }
public:

	std::shared_ptr<FProceduralFigurBuffer> BildingAreaFigure;

protected:

	void CreateTowers(const FLevelGeneratorSettings& LevelSettings, const ALevelGenerator& LevelGenerator, FDataStorage& DataStorage);

	std::shared_ptr<FLevelGraphLink> CreateLinkBetvineJointParts(FJointPart* First, FJointPart* Second);

	void CreateFloorInstance();

	void CreateFloorInstanceForCurrentCell(int X, int Y);

	void CreateFloorInstanceForEncirclingCells();

	void CreateRoomWalls(FLevelCellData& CurrentCell);

	void CreateFloors(FLevelCellData& CurrentCell);

	void CreateRoofs(FLevelCellData& CurrentCell);

	void CreateRoadWalls();

	void SetWallCornersForDirection(FLevelCellData& CurrentCell, EDirection Direction);

	void SetCornersForCurrentWall(FWallCorner& WallCorner, const FLevelCellData& CurrentCell, int Floor, EDirection Direction, EDirection WallOffsetDirection);

	void GetWallRotationAndOffset(const FLevelCellData& CurrentCell, FVector& WallOffset, FRotator& WallRotation, EDirection Direction) const;

	void CreateWallSegmentsForDirection(FLevelCellData& CurrentCell, EDirection Direction);

	void CreateEdgeSegmentsForTwoSideWall(FCellWall& CurrentWall, FVector WallCoordinateBaseSegment, FVector2D WallSizeBaseSegment, FRotator WallRotate, EDirection WallDirection,
		const FLevelGeneratorMaterialSettings& BaseSegmentMaterial);

	void CreateSegmentsFor90DegresCorner(FCellWall& CurrentWall, FVector WallCoordinateBaseSegment, FVector2D WallSizeBaseSegment, FRotator WallRotate, EDirection WallDirection);

	void SetFlooreCorners(FLevelCellData& CurrentCell);

	FWallCorner SetFlooreCornersForCurrentDirectionForZeroFlooreDown(FLevelCellData& CurrentCell, EDirection Direction);

	FWallCorner SetFlooreCornersForCurrentDirectionForLastRoof(FLevelCellData& CurrentCell, EDirection Direction);

	FWallCorner SetFlooreCornersForCurrentDirection(FLevelCellData& CurrentCell, int Floor, EDirection Direction, EFloorName FloorName);

	void CreateFloorSegments(FLevelCellData& CurrentCell);

	void CreateTriangelSegmentInCorner(FLevelCellData& CurrentCell, EFloorName FloorName, int Floor, FVector CoordinateBaseSegment, const FLevelGeneratorMaterialSettings& BaseSegmentMaterial);

	void CreateFloorSegmentsForCurrentDirection(FLevelCellData& CurrentCell, EDirection Direction,
		FVector CoordinateBaseSegment, FVector2D SizeBaseSegment, const FLevelGeneratorMaterialSettings& BaseSegmentMaterial, EFloorName FloorName, FCellFlloor& CurrentFloor);

	void CreateSkirting(FLevelCellData& CurrentCell, int Floor, EDirection Direction,
		FVector CoordinateBaseSegment, FVector2D SizeBaseSegment, const FLevelGeneratorMaterialSettings& BaseSegmentMaterial, EFloorName FloorName);

	void CreateFasadeWall(FProceduralFigurBuffer& WallBuffer, EDirection Direction);

	void CreateLevelCell(FProceduralFigurBuffer& WallBuffer, FVector2D CellCoordinate);

	void SetUpperBaseLevelNum(int NewBaseLevelNum, int X, int Y);

	bool CheckCell(FVector2D Coordinate);

	bool CheckCellFloors(FVector2D Coordinate);

	void ReplaceCell(FVector2D Coordinate);

	void SetNumForCollomX();

	void SetNumForCollomY();

protected:
	
	const FVector2D Coordinate;

	const FVector2D Size;

	std::vector<FLvevelLayer> GraphLayers;

	std::vector<std::vector<std::shared_ptr<FPlacedLevelTower>>> BildingTowers;

	const FLevelGeneratorSettings& LevelSettings;

	EBildingZoneState BildingZoneState = EBildingZoneState::Created;

	std::map<FLevelGenBildingZoneBase*, std::shared_ptr<FLevelGenBildingZoneLink>> Links;

	const ALevelGenerator& LevelGenerator;

	FDataStorage& DataStorage;

};

class FLevelGenBildingZone : public FLevelGenBildingZoneBase
{
public:
	FLevelGenBildingZone(bool UnPlacebelBilding, FVector2D Start, FVector2D End, FDataStorage& DataStorage,
		const FLevelGeneratorSettings& _LevelSettings, const ALevelGenerator& _LevelGenerator);

	virtual void CreateProceduralFigure() override;

private:

	virtual void ConnectBildingToLevelCells(FDataStorage& DataStorage) override;
};

class FLevelGenBildingZoneTower : public FLevelGenBildingZoneBase
{
public:
	FLevelGenBildingZoneTower(bool UnPlacebelBilding, FVector2D Start, FVector2D End, FDataStorage& DataStorage,
		const FLevelGeneratorSettings& _LevelSettings, const ALevelGenerator& _LevelGenerator);

	virtual void CreateProceduralFigure() override;

private:

	virtual void ConnectBildingToLevelCells(FDataStorage& DataStorage) override;

	void CreateTowerFasedeWallSegment(FVector WallCoordinate, FVector WallNormalVector, FVector WallXVector, float SegmentWeight, float TowerHeight);
};