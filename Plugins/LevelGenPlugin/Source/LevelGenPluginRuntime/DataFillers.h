// Copyright 2018 Pavlov Dmitriy
#pragma once
#include "CoreMinimal.h"
#include <vector>
#include <memory>
#include <utility>
#include <tuple>
#include <list>
#include <set>
#include "LevelGenCore.h"
#include "UObject/Class.h"  //For UENUM
#include"GenerateMashine.h"

struct FLevelGeneratorSettings;
enum class ELevelCellType : uint8;
class FDataFillerBase;
struct FLevelCellBase;
struct FLvevelLayer;
struct FLevelGraphNode;
class ALevelGenerator;
class FDataFillerBase;
class FLevelGenRoadBase;
struct FMainRoad;
struct FDataStorage;
struct FLevelBilding;
class FCarTrackNode;
struct FRoadCrossing;
struct FLevelCellData;
struct FLevelGeneratorMaterialSettings;
class FProceduralFigurBuffer;
class FLevelGenBildingZoneBase;
enum class EFloorName : uint8;
struct FWallCorner;
struct FCartTracsContainer;
enum class EHoverCarTrackDirection : uint8;
struct FCellWall;
struct FCellFlloor;
struct FLevelGraphLink;
struct FJointPart;
struct FLevelBildingZoneLinkRoom;
class FLevelGenBildingZoneLink;

UENUM()
enum class EDataFiller : uint8
{
	RoadMap,
	MainRoadMap,
	Graph,
	BildingsZoneCreater,
	BildingDisposer,
	CarTrack,
	End
};

FDataFillerBase* EDataFillerCreate(EDataFiller ComponentType, FDataStorage& DataStorage, const FLevelGeneratorSettings& LevelSettings, const ALevelGenerator& LevelGenerator);



class FDataFillerBase : public FGenerateObject
{
public:
	FDataFillerBase(FDataStorage& _DataStorage, const FLevelGeneratorSettings& _LevelSettings, const ALevelGenerator& _LevelGenerator) :
		DataStorage(_DataStorage), LevelSettings(_LevelSettings), LevelGenerator(_LevelGenerator){}

	virtual ~FDataFillerBase() {}

	virtual void FillData(FVector2D Start, FVector2D End) = 0;

	virtual EDataFiller GetType() const = 0;

	virtual void Generate(FVector2D CenterCoordinate, int Radius, AVirtualSpawner* VirtualSpawner) override { FillData(CenterCoordinate - FVector2D(Radius, Radius), CenterCoordinate + FVector2D(Radius, Radius)); }

	virtual FString GetModuleName() const override  { return FString("Data Filler ") + GetEnumValueAsString<EDataFiller>("EDataFiller", GetType()); }

protected:

	FDataStorage& DataStorage;

	const FLevelGeneratorSettings& LevelSettings;

	const ALevelGenerator& LevelGenerator;
};

class FDataFillerMainRoadMap : public FDataFillerBase
{
public:

	FDataFillerMainRoadMap(FDataStorage& _DataStorage, const FLevelGeneratorSettings& _LevelSettings, const ALevelGenerator& _LevelGenerator);

	~FDataFillerMainRoadMap();

	virtual void FillData(FVector2D Start, FVector2D End) override;

	virtual EDataFiller GetType() const override { return EDataFiller::MainRoadMap; }

	virtual int GetDeltaOffsetGenerateArea() const override;

private:

	void CreateMainRoads(FVector2D StartRoadCells, FVector2D EndRoadCells);

	void SetMainRoads(FVector2D StartRoadCells, FVector2D EndRoadCells);


private:

	TLevelMap1D<FMainRoad> MainRoadsX;

	std::map<int, FMainRoad*> MainRoadsXMap;

	TLevelMap1D<FMainRoad> MainRoadsY;

	std::map<int, FMainRoad*> MainRoadsYMap;

};





class FDataFillerRoadMap : public FDataFillerBase
{
	

public:
	FDataFillerRoadMap(FDataStorage& _DataStorage, const FLevelGeneratorSettings& _LevelSettings, const ALevelGenerator& _LevelGenerator) :
		FDataFillerBase(_DataStorage, _LevelSettings, _LevelGenerator) {}

	virtual void FillData(FVector2D Start, FVector2D End) override;

	virtual EDataFiller GetType() const override { return EDataFiller::RoadMap; }

private:

	virtual int GetDeltaOffsetGenerateArea() const override;

	void CreateRoads(FVector2D StartRoadCells, FVector2D EndRoadCells);

	void CheckRoadPlase(FVector2D StartRoadCells, FVector2D EndRoadCells);
	
};

class FDataFillerBildingsZoneCreater : public FDataFillerBase
{
public:

	FDataFillerBildingsZoneCreater(FDataStorage& _DataStorage, const FLevelGeneratorSettings& _LevelSettings, const ALevelGenerator& _LevelGenerator) :
		FDataFillerBase(_DataStorage, _LevelSettings, _LevelGenerator) {}

	virtual void FillData(FVector2D Start, FVector2D End) override;

	virtual EDataFiller GetType() const override { return EDataFiller::BildingsZoneCreater; }


private:

	void CreateBildings(FVector2D StartBilldingCells, FVector2D EndBildingCells);

	virtual int GetDeltaOffsetGenerateArea() const override { return 1; }

	void CreateBildingsZoneLinks(FVector2D StartBilldingCells, FVector2D EndBildingCells);

	void CreateLinksForCurrentBildingZone(FLevelGenBildingZoneBase& CurrentBildingZone, FVector2D CurrentBildingCoordinate, FVector2D OtherBildingDelta);
	
};



class FDataFillerGraph : public FDataFillerBase
{
public:	

	FDataFillerGraph(FDataStorage& _DataStorage, const FLevelGeneratorSettings& _LevelSettings, const ALevelGenerator& _LevelGenerator) : FDataFillerBase(_DataStorage, _LevelSettings, _LevelGenerator) {}

	virtual void FillData(FVector2D Start, FVector2D End) override;

	virtual EDataFiller GetType() const override { return EDataFiller::Graph; }

private:

	void LinkJointPartsToRooms(FLevelGenBildingZoneBase& CurrentBildingZone);

};




class FDataFillerBildingDisposer : public FDataFillerBase
{
public:

	FDataFillerBildingDisposer(FDataStorage& _DataStorage, const FLevelGeneratorSettings& _LevelSettings, const ALevelGenerator& _LevelGenerator) :
		FDataFillerBase(_DataStorage, _LevelSettings, _LevelGenerator) {}

	virtual void FillData(FVector2D Start, FVector2D End) override;

	virtual EDataFiller GetType() const override { return EDataFiller::BildingDisposer; }

private:

	void CreateBilding(const FLevelBilding& Bilding, FVector2D Coordinate, bool On90Deg);

};

class FDataFillerCarTrack : public FDataFillerBase
{
public:

	FDataFillerCarTrack(FDataStorage& _DataStorage, const FLevelGeneratorSettings& _LevelSettings, const ALevelGenerator& _LevelGenerator) :
		FDataFillerBase(_DataStorage, _LevelSettings, _LevelGenerator) {}

	virtual void FillData(FVector2D Start, FVector2D End) override;

	virtual EDataFiller GetType() const override { return EDataFiller::CarTrack; }

	

private:

	virtual void Generate(FVector2D CenterCoordinate, int Radius, AVirtualSpawner* VirtualSpawner) override;

	void CreateTrackNods(FVector2D StartRoadCells, FVector2D EndRoadCells);

	void CreateTrackLayerNods(int X, int Y, int LayerNum, int TrackHeight, EHoverCarTrackDirection TrackDirection);

	void CreateTrackLinks(FVector2D StartRoadCells, FVector2D EndRoadCells);

	void CreateTrackLayerLinks(int X, int Y, int LayerNum);

	std::pair<FCartTracsContainer*, FCartTracsContainer*> GetNearesTracksOppositDirection(int X, int Y, int LayerNum, FLevelGenRoadBase& CurrentRoad);

	std::pair<FCartTracsContainer*, FCartTracsContainer*> GetNearesTracksSameDirection(int X, int Y, int LayerNum, FLevelGenRoadBase& CurrentRoad);

	void FindPossibleTrackLinksForRoads(FCartTracsContainer& TracksContainer, FCartTracsContainer& NeareRoad);

	void CreateCarTrackLink(FCarTrackNode* FirstCarTrackNode, FCarTrackNode* SecondCarTrackNode, FVector JointCoordinate);

	void SetSpawnCarTracks(FVector2D CenterCoordinate, int Radius);

	void SetSpawnCarTrackForLayer(FVector2D CenterCoordinate, int Radius, int LayerNum);
};





