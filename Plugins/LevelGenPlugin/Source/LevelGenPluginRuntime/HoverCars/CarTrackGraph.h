// Copyright 2018 Pavlov Dmitriy
#pragma once
#include "CoreMinimal.h"
#include <vector>
#include <memory>
#include <map>
#include <set>
#include "LevelGenCore.h" 

class FLevelGenSpline;
class FCarTrackNode;
struct FLevelGeneratorSettings;
class FHoverCar;
class FLevelGenRoadBase;



enum class ETrackCoordinatAppointment : uint8
{
	Start,
	End
};



struct FTrackCoordinate
{
	FTrackCoordinate() = default;

	explicit FTrackCoordinate(FVector _TrackCoordinate) : TrackCoordinate(_TrackCoordinate) {}

	FVector TrackCoordinate;
	FVector TrackTangens;
};

class FCarTrackGraphBase
{
public:

	FCarTrackGraphBase(const FLevelGeneratorSettings& _LevelSettings, const FDataStorage& _DataStorage) :
		LevelSettings(_LevelSettings),  DataStorage(_DataStorage), RandomGenerator(time(0)) {}

	virtual ~FCarTrackGraphBase() {}

	void SetUnActive();

	void SetActive() { IsActive = true; }
	
	bool IsTrackActive() const { return IsActive; }

	FTrackCoordinate GetStart() const { return TrackCoordinats[(int)ETrackCoordinatAppointment::Start]; }

	FTrackCoordinate GetEnd() const { return TrackCoordinats[(int)ETrackCoordinatAppointment::End]; }

	float GetTreckLenght() const;

	bool GetCoordinateAndTangent(float StartDistance, FVector& Coordinate, FVector& Tangent);

	void AddHoverCar(std::shared_ptr<FHoverCar> HoverCar) { HoverCarsOnTrack.insert(HoverCar); }

	void RemoveHoverCar(std::shared_ptr<FHoverCar> HoverCar) { HoverCarsOnTrack.erase(HoverCar); }

	virtual FCarTrackGraphBase* GetNextTrack(int LayerNum) const = 0;

	std::set<std::shared_ptr<FHoverCar>>& GetHoverCars() { return HoverCarsOnTrack; }

protected:

	void CreateTrack();

	int GetRandomNumber(int From, int To) const
	{
		std::uniform_int_distribution<> distr(From, To);
		return distr(RandomGenerator);
	}


private:

	std::shared_ptr<FLevelGenSpline> Track = nullptr;

	bool IsActive = false;

	

protected:

	std::vector<FTrackCoordinate> TrackCoordinats;

	const FLevelGeneratorSettings& LevelSettings;

	const FDataStorage& DataStorage;
	
	std::set<std::shared_ptr<FHoverCar>> HoverCarsOnTrack;

	mutable std::mt19937 RandomGenerator;

};

class FCarTrackLink : public FCarTrackGraphBase
{
public:

	FCarTrackLink(FCarTrackNode* _FirstNode, FCarTrackNode* _SecondNode, FVector JointCoordinate, const FLevelGeneratorSettings& LevelSettings, const FDataStorage& DataStorage);

	virtual FCarTrackGraphBase* GetNextTrack(int LayerNum) const;

	FCarTrackNode* GetStartNode() { return StartNode; }

	FCarTrackNode* GetEndNode() { return EndNode; }

private:

	FCarTrackNode* StartNode = nullptr;

	FCarTrackNode* EndNode = nullptr;

};

class FCarTrackNode : public FCarTrackGraphBase
{
public:
	FCarTrackNode(EHoverCarTrackDirection Direction, std::pair<FVector, FVector> FirstPoint, std::pair<FVector, FVector> SecondPoint, const FLevelGenRoadBase* TrackRoadOwner,
		const FLevelGeneratorSettings& LevelSettings, const FDataStorage& DataStorage);

	EHoverCarTrackDirection Direction;

	std::vector<FVector> NodeJointCoordinats;

	const FLevelGenRoadBase* GetOwner() const { return TrackRoadOwner; }

	virtual FCarTrackGraphBase* GetNextTrack(int LayerNum) const override;

	const std::map<const FCarTrackNode*, std::shared_ptr<FCarTrackLink>>& GetLinks() const { return Links; }

	void AddLink(std::pair<const FCarTrackNode*, std::shared_ptr<FCarTrackLink>> NewLink) { Links.insert(NewLink); }

private:

	std::map<const FCarTrackNode*, std::shared_ptr<FCarTrackLink>> Links;

	const FLevelGenRoadBase* TrackRoadOwner = nullptr;
};

struct FCartTracsContainer
{

	std::vector<std::shared_ptr<FCarTrackNode>> CarTrackNodes;

	std::multimap<FVector, FCarTrackNode*, FVectorLess> CarTrackNodesJointCoordinats;

};