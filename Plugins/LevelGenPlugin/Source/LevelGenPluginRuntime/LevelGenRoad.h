// Copyright 2018 Pavlov Dmitriy
#pragma once
#include "CoreMinimal.h"
#include <memory>
#include <vector>
#include <map>
#include <algorithm>
#include "LevelGenCore.h"


class FLevelGenRoadBaseCell;
class FLevelGenRoadBaseMover;
struct FLevelCellBase;
class FCarTrackNode;
class FCarTrackLink;
class FLevelGenRoadBase;
struct FLevelGeneratorSettings;
class ALevelGenerator;
struct FDataStorage;

enum class ERoadCrossingType : uint8
{
	XCrossing,
	TCrossing,
	RoadEdge,
	EndRoad
};

enum class ERoadDirection : uint8
{
	Horizontal,
	Vertical,
	end
};

struct FRoadCrossing
{
	FRoadCrossing(ERoadCrossingType _Type, FVector2D _Coordinate, FLevelGenRoadBase* _CrossingRoad) : Type(_Type), Coordinate(_Coordinate), CrossingRoad(_CrossingRoad) {}

	ERoadCrossingType Type;
	FVector2D Coordinate;
	FLevelGenRoadBase* CrossingRoad = nullptr;
};

struct FMainRoad
{
	FMainRoad(ERoadDirection _Direction, int _Coordinate, int _RoadColumnNumber, int _Size, const FLevelGeneratorSettings& _LevelSettings) :
		Direction(_Direction), Coordinate(_Coordinate), RoadColumnNumber(_RoadColumnNumber), LevelSettings(_LevelSettings), Size(_Size) {}

	ERoadDirection Direction = ERoadDirection::end;

	int Coordinate = 0;

	int RoadColumnNumber = 0;
		
	const FLevelGeneratorSettings& LevelSettings;

	int Size;
};

enum class ERoadState
{
	Created,
	CoordinateCheced,
	ConectedToLevelMap,
	UnPlaseble
};

class FLevelGenRoadBase
{
public:

	FLevelGenRoadBase(int Size, bool _bIsMainRoad, FVector2D _RoadNumber, const TLevelMap2D<FLevelGenRoadBase>& _RoadsMap,
		const FLevelGeneratorSettings& _LevelSettings, const ALevelGenerator& _LevelGenerator) :
		 RoadsMap(_RoadsMap),  LevelSettings(_LevelSettings),   LevelGenerator(_LevelGenerator), RoadSize(Size), bIsMainRoad(_bIsMainRoad), RoadNumber(_RoadNumber)
	{}

	virtual ~FLevelGenRoadBase() {}

	virtual ERoadDirection GetDirection() const = 0;

	int GetCoordinate() const { return RoadCoordinate; }

	int GetSize() const { return RoadSize; }

	virtual void ConnectRoadToLevelCells(FDataStorage& DataStorage) = 0;

	virtual void CheckMainRoadAndRoadCrossings(FDataStorage& DataStorage) = 0;

	ERoadState GetRoadState() const { return RoadState; }

	bool IsMainRoad() const { return bIsMainRoad; }

	virtual bool GetRoadEdges(std::vector<FRoadCrossing>& Crossings) const = 0;

	virtual std::pair<FLevelGenRoadBase*, FLevelGenRoadBase*> GetNearestRoadsOppositDirection() const = 0;

	virtual std::pair<FLevelGenRoadBase*, FLevelGenRoadBase*> GetNearestRoadsSameDirection() const = 0;

	void CheckRoadPlase();

	void SetCoordinate(int Coordinate) { RoadCoordinate = Coordinate; }

	FVector2D GetRoadNumber() const { return RoadNumber; }



protected:

	bool GetBoundaryForRoad(std::pair<int, int>& Coordinates) const;

	virtual std::pair<FLevelGenRoadBase*, FLevelGenRoadBase*> GetCrossingRoads() const = 0;

	bool GetRangeCoordinatForRoad(std::pair<int, int>& CoordinateRange) const;

	void SetRoadState(ERoadState NewState) { RoadState = NewState; }

protected:

	const TLevelMap2D<FLevelGenRoadBase>& RoadsMap;

	const FLevelGeneratorSettings& LevelSettings;

	const ALevelGenerator& LevelGenerator;

	
private:

	bool GetRangeCoordinatForRoadFromRoadsCoordinats(std::pair<int, int>& CoordinateRange, FVector2D RoadCoordinate1, FVector2D RoadCoordinate2) const;

private:

	int RoadCoordinate = 0;

	int RoadSize = 0;

	bool bIsMainRoad = false;

	FVector2D RoadNumber;

	ERoadState RoadState = ERoadState::Created;

	
};




class FLevelGenRoadHorizontal : public FLevelGenRoadBase
{
public:
	FLevelGenRoadHorizontal(int Coordinate, int Size, bool IsMainRoad, FVector2D RoadNumber, const TLevelMap2D<FLevelGenRoadBase>& RoadsMap,
		const FLevelGeneratorSettings& LevelSettings, const ALevelGenerator& LevelGenerator);

	FLevelGenRoadHorizontal(int Size, bool IsMainRoad, FVector2D RoadNumber, const TLevelMap2D<FLevelGenRoadBase>& RoadsMap,
		const FLevelGeneratorSettings& LevelSettings, const ALevelGenerator& LevelGenerator);

	virtual ERoadDirection GetDirection() const override { return ERoadDirection::Horizontal; }

	virtual void ConnectRoadToLevelCells(FDataStorage& DataStorage) override;

	virtual void CheckMainRoadAndRoadCrossings(FDataStorage& DataStorage) override;

	virtual bool GetRoadEdges(std::vector<FRoadCrossing>& Crossings) const override;


protected:


	virtual std::pair<FLevelGenRoadBase*, FLevelGenRoadBase*> GetNearestRoadsOppositDirection() const override;

	virtual std::pair<FLevelGenRoadBase*, FLevelGenRoadBase*> GetNearestRoadsSameDirection() const override;

	virtual std::pair<FLevelGenRoadBase*, FLevelGenRoadBase*> GetCrossingRoads() const override;

};


class FLevelGenRoadVertical : public FLevelGenRoadBase
{
public:
	FLevelGenRoadVertical(int Coordinate, int Size, bool IsMainRoad, FVector2D RoadNumber, const TLevelMap2D<FLevelGenRoadBase>& RoadsMap,
		const FLevelGeneratorSettings& LevelSettings, const ALevelGenerator& LevelGenerator) : FLevelGenRoadBase(Size, IsMainRoad, RoadNumber, RoadsMap, LevelSettings, LevelGenerator)
	{
		SetCoordinate(Coordinate);
	}

	FLevelGenRoadVertical(int Size, bool IsMainRoad, FVector2D RoadNumber, const TLevelMap2D<FLevelGenRoadBase>& RoadsMap,
		const FLevelGeneratorSettings& LevelSettings, const ALevelGenerator& LevelGenerator);

	virtual ERoadDirection GetDirection() const override { return ERoadDirection::Vertical; }

	virtual void ConnectRoadToLevelCells(FDataStorage& DataStorage) override;

	virtual void CheckMainRoadAndRoadCrossings(FDataStorage& DataStorage) override;

	virtual bool GetRoadEdges(std::vector<FRoadCrossing>& Crossings) const override;


protected:

	virtual std::pair<FLevelGenRoadBase*, FLevelGenRoadBase*> GetNearestRoadsOppositDirection() const override;

	virtual std::pair<FLevelGenRoadBase*, FLevelGenRoadBase*> GetNearestRoadsSameDirection() const override;

	virtual std::pair<FLevelGenRoadBase*, FLevelGenRoadBase*> GetCrossingRoads() const override;

};