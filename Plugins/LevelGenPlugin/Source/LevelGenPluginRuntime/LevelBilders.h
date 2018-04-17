// Copyright 2018 Pavlov Dmitriy
#pragma once
#include "CoreMinimal.h"
#include "LevelGenCore.h"
#include <vector>
#include <set>
#include "UObject/Class.h"  //For UENUM
#include"GenerateMashine.h"
#include "DataStorage.h"

class AActor;
class UStaticMesh;
struct FLevelGeneratorSettings;
class FLevelBilderBase;
class FPlacedLevelRoomLinkedToLevel;
class FProceduralFigureBase;
class ALevelGenProceduralMeshActor;
struct FLevelGeneratorMaterialSettings;
struct FDataStorage;
struct FPlacedLevelBilding;
class FCarTrackGraphBase;
struct FLevelCellData;
struct FRoadCrossing;
struct FCellWall;
class FProceduralFigurBuffer;
class FLevelGenBildingZoneBase;
class AHoverCarActor;
class FLevelGenBildingZoneLink;
class ALevelGenerator;

UENUM()
enum class ELevelBilder : uint8
{
	Rooms,
	BildingsZone,
	Bildings,
	Roads,
	CarTrack,
	Tower,
	End
};



FLevelBilderBase* ELevelBilderCreate(ELevelBilder Type, FDataStorage& DataStorage, const FLevelGeneratorSettings&  LevelSettings, const ALevelGenerator& ParentActor);



struct FLevelBilderCell 
{
	FLevelBilderCell() {}

	std::vector<AActor*> CreatedMeshActors;

	std::vector<FCarTrackGraphBase*> ActiveTracs;

	bool IsBildet = false;

};

struct FBildArea
{
	FBildArea(FVector2D _CenterCoordinate, int _Radius) : CenterCoordinate(_CenterCoordinate), Radius(_Radius) {}

	FVector2D CenterCoordinate;

	int Radius;
};



class FLevelBilderBase : public FGenerateObject
{
public:

	FLevelBilderBase(FDataStorage& _DataStorage, const FLevelGeneratorSettings& _LevelSettings, const ALevelGenerator& _ParentActor)
		: DataStorage(_DataStorage), LevelSettings(_LevelSettings), ParentActor(_ParentActor){}

	~FLevelBilderBase() {}

	virtual void BildLevel(FVector2D CenterCoordinate, int Radius, AVirtualSpawner* VirtualSpawner);

	virtual ELevelBilder GetBilderType() const = 0;

	virtual void Generate(FVector2D CenterCoordinate, int Radius, AVirtualSpawner* VirtualSpawner) override
	{
		GetRadiusAndCentrCoordinateForGenerate(GetLevelMapType(), CenterCoordinate, Radius);
		BildLevel(CenterCoordinate, Radius, VirtualSpawner);
	}

	virtual FString GetModuleName() const override  { return FString("Level Bilder ") + GetEnumValueAsString<ELevelBilder>("ELevelBilder", GetBilderType()); }

	

	virtual ELevelMapType GetLevelMapType() const = 0;

	void GetRadiusAndCentrCoordinateForGenerate(ELevelMapType LevelMapType, FVector2D& CenterCoordinate, int& Radius);

protected:
	std::unique_ptr<FBildArea> LastBlidArea;

	std::unique_ptr<FBildArea> NewBlidArea;

	TLevelMap2D<FLevelBilderCell> BildArea;

	FDataStorage& DataStorage;

	const FLevelGeneratorSettings& LevelSettings;

	const ALevelGenerator& ParentActor;

protected:
	void CreateMesh(AVirtualSpawner* VirtualSpawner, FLevelBilderCell& LevelBilderCell, UStaticMesh* Mesh, FName ActorTag, FVector Location, bool Collision = false,
		FRotator Rotation = FRotator::ZeroRotator, FVector Scale = FVector(1,1,1));

	void CreateMesh(AVirtualSpawner* VirtualSpawner, FLevelBilderCell& LevelBilderCell, UStaticMesh* Mesh, FName ActorTag, FVector ActorLoc, const std::vector<FVector>& Points,
		bool Collision = false, FVector2D Scale = FVector2D(1, 1));

	void CreateProceduralActor(AVirtualSpawner* VirtualSpawner, FLevelBilderCell& LevelBilderCell, const FProceduralFigurBuffer& FigureBufer, FVector AbsLocation, FName ActorTag, bool Collision = false);

	void  BildActor(AVirtualSpawner* VirtualSpawner, FLevelBilderCell& LevelBilderCell, UClass* ActorClass, FVector ActorStartCoordinate, FVector2D ActorSize, int ActorYaw);

	virtual bool BildLevelCell(int X, int Y, FLevelBilderCell& LevelBilderCell, AVirtualSpawner* VirtualSpawner) = 0;

	virtual void DeleteCell(int X, int Y, FLevelBilderCell& LevelBilderCell, AVirtualSpawner* VirtualSpawner);

	virtual void RefreshLastArea(AVirtualSpawner* VirtualSpawner);


};


class FLevelBilderRoads : public FLevelBilderBase
{
public:
	FLevelBilderRoads(FDataStorage& _DataStorage, const FLevelGeneratorSettings& _LevelSettings, const ALevelGenerator& _ParentActor) : FLevelBilderBase(_DataStorage, _LevelSettings, _ParentActor) {}

	virtual ELevelBilder GetBilderType() const override { return ELevelBilder::Roads; }

	virtual ELevelMapType GetLevelMapType() const override { return ELevelMapType::RoadsMap; }

private:

	virtual bool BildLevelCell(int X, int Y, FLevelBilderCell& LevelBilderCell, AVirtualSpawner* VirtualSpawner) override;


	void BildRoadCrossings(std::vector<FRoadCrossing>& Crossings, const FLevelGenRoadBase& CurrentRoad, FLevelBilderCell& LevelBilderCell, AVirtualSpawner* VirtualSpawner);

	void BildRoads(const std::vector<FRoadCrossing>& Crossings, const FLevelGenRoadBase& CurrentRoad, FLevelBilderCell& LevelBilderCell, AVirtualSpawner* VirtualSpawner);

	void FillHoleUnderRoad(const std::vector<FVector>& RoadCoordinats3D, const FLevelGenRoadBase& CurrentRoad, AVirtualSpawner* VirtualSpawner, FLevelBilderCell& LevelBilderCell);

	void FillHoleUnderRoad(FVector2D Coordinate, FVector2D Size, AVirtualSpawner* VirtualSpawner, FLevelBilderCell& LevelBilderCell);

	void BildMainRoadAndNotMaindRoadCrossing(const FLevelGenRoadBase* MainRoad, const FLevelGenRoadBase* NotMainRoad, FVector2D CrossingLocation, FLevelBilderCell& LevelBilderCell, AVirtualSpawner* VirtualSpawner);


};



class FLevelBilderRooms : public FLevelBilderBase
{
public:
	FLevelBilderRooms(FDataStorage& _DataStorage, const FLevelGeneratorSettings& _LevelSettings, const ALevelGenerator& _ParentActor) :  FLevelBilderBase(_DataStorage, _LevelSettings, _ParentActor) {}

	virtual ELevelBilder GetBilderType() const override { return ELevelBilder::Rooms; }

	virtual ELevelMapType GetLevelMapType() const override { return ELevelMapType::BildingMap; }


private:

	virtual bool BildLevelCell(int X, int Y, FLevelBilderCell& LevelBilderCell, AVirtualSpawner* VirtualSpawner) override;

	virtual void DeleteCell(int X, int Y, FLevelBilderCell& LevelBilderCell, AVirtualSpawner* VirtualSpawner) override;

	void BildPlacedRoom(FPlacedLevelRoomLinkedToLevel* PlacedRoom, FLevelBilderCell& LevelBilderCell, AVirtualSpawner* VirtualSpawner);

private:

	std::set<FPlacedLevelRoomLinkedToLevel*> BildetRooms;

};


class FLevelBilderBildingsZone : public FLevelBilderBase
{
public:
	FLevelBilderBildingsZone(FDataStorage& _DataStorage, const FLevelGeneratorSettings& _LevelSettings, const ALevelGenerator& _ParentActor) :
		FLevelBilderBase(_DataStorage, _LevelSettings, _ParentActor) {}

	virtual ELevelBilder GetBilderType() const override { return ELevelBilder::BildingsZone; }


	virtual ELevelMapType GetLevelMapType() const override { return ELevelMapType::BildingMap; }
	
private:


	virtual bool BildLevelCell(int X, int Y, FLevelBilderCell& LevelBilderCell, AVirtualSpawner* VirtualSpawner) override;
	
};

class FLevelBilderTower : public FLevelBilderBase
{
public:
	FLevelBilderTower(FDataStorage& _DataStorage, const FLevelGeneratorSettings& _LevelSettings, const ALevelGenerator& _ParentActor) :
		FLevelBilderBase(_DataStorage, _LevelSettings, _ParentActor) {}

	virtual ELevelBilder GetBilderType() const override { return ELevelBilder::Tower; }

	virtual ELevelMapType GetLevelMapType() const override { return ELevelMapType::BildingMap; }

private:

	virtual bool BildLevelCell(int X, int Y, FLevelBilderCell& LevelBilderCell, AVirtualSpawner* VirtualSpawner) override;

	void BildTowers(FLevelGenBildingZoneBase& CurrentBilding, FLevelBilderCell& LevelBilderCell, AVirtualSpawner* VirtualSpawner);
};

class FLevelBilderBildings : public FLevelBilderBase
{
public:
	FLevelBilderBildings(FDataStorage& _DataStorage, const FLevelGeneratorSettings& _LevelSettings, const ALevelGenerator& _ParentActor) :
		FLevelBilderBase(_DataStorage, _LevelSettings, _ParentActor) {}

	virtual ELevelBilder GetBilderType() const override { return ELevelBilder::Bildings; }

	virtual ELevelMapType GetLevelMapType() const override { return ELevelMapType::LevelMap; }

private:



	virtual bool BildLevelCell(int X, int Y, FLevelBilderCell& LevelBilderCell, AVirtualSpawner* VirtualSpawner) override;

	virtual void DeleteCell(int X, int Y, FLevelBilderCell& LevelBilderCell, AVirtualSpawner* VirtualSpawner) override;

	std::set<FPlacedLevelBilding*> BildetBildings;
};


class FLevelBilderCarTrack : public FLevelBilderBase
{
public:
	FLevelBilderCarTrack(FDataStorage& _DataStorage, const FLevelGeneratorSettings& _LevelSettings, const ALevelGenerator& _ParentActor) :
		FLevelBilderBase(_DataStorage, _LevelSettings, _ParentActor) {}

	virtual void Generate(FVector2D CenterCoordinate, int Radius, AVirtualSpawner* VirtualSpawner) override;

	virtual ELevelBilder GetBilderType() const override { return ELevelBilder::CarTrack; }

	virtual ELevelMapType GetLevelMapType() const override { return ELevelMapType::CarTrackMap; }

	virtual void DeleteCell(int X, int Y, FLevelBilderCell& LevelBilderCell, AVirtualSpawner* VirtualSpawner) override;

private:

	virtual bool BildLevelCell(int X, int Y, FLevelBilderCell& LevelBilderCell, AVirtualSpawner* VirtualSpawner) override;


	void CreateHoverCars(FCarTrackGraphBase& CarTrack, std::vector<UClass*>& HoverCars, FLevelBilderCell& LevelBilderCell, AVirtualSpawner* VirtualSpawner, int OneHowerCarsOnRoadUnits);

	void CreateHoverCar(AVirtualSpawner* VirtualSpawner, FLevelBilderCell& LevelBilderCell, FCarTrackGraphBase& CarTrack, UClass* HoverCarClass, float CarDistance);

};
