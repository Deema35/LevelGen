// Copyright 2018 Pavlov Dmitriy
#include "CarTrackGraph.h"
#include "LevelGeneratorSettings.h"
#include <map>
#include "LevelGenRoad.h"
#include "HoverCarActor.h"
#include "LevelGenSpline.h"
#include "DataStorage.h"
#include"VirtualSpawner.h"
#include "HoverCar.h"

//*****************************************
//FCarTrackGraphBase
//****************************************

float FCarTrackGraphBase::GetTreckLenght() const
{
	if (Track)
	{
		return Track->GetSplineLength();
	}
	else throw;
}

void FCarTrackGraphBase::SetUnActive()
{
			
	HoverCarsOnTrack.clear();
	IsActive = false;
}

bool FCarTrackGraphBase::GetCoordinateAndTangent(float Distance, FVector& Coordinate, FVector& Tangent)
{
	if (!Track || Track->GetSplineLength() < Distance) return false;

	Coordinate = Track->GetLocationAtDistanceAlongSpline(Distance);

	Tangent = Track->GetTangentAtDistanceAlongSpline(Distance);

	return true;
}

void FCarTrackGraphBase::CreateTrack()
{
	FTrackCoordinate StartTrack = GetStart();
	FTrackCoordinate EndTrack = GetEnd();

	Track = std::shared_ptr<FLevelGenSpline>(new  FLevelGenSpline(StartTrack.TrackCoordinate, StartTrack.TrackTangens, EndTrack.TrackCoordinate, EndTrack.TrackTangens));
}



//*****************************************
//FCarTrackLink
//****************************************

FCarTrackLink::FCarTrackLink(FCarTrackNode* FirstNode, FCarTrackNode* SecondNode, FVector JointCoordinate, const FLevelGeneratorSettings& LevelSettings, const FDataStorage& DataStorage ) :
	FCarTrackGraphBase(LevelSettings, DataStorage)
{
	if (FirstNode->NodeJointCoordinats[(int)ETrackCoordinatAppointment::End] == JointCoordinate)
	{
		StartNode = FirstNode;
		EndNode = SecondNode;

		TrackCoordinats.push_back(FirstNode->GetEnd());
		TrackCoordinats.back().TrackTangens /= TrackCoordinats.back().TrackTangens.Size();
		TrackCoordinats.back().TrackTangens *= SecondNode->GetOwner()->GetSize() * LevelSettings.CellSize;
		

		if (SecondNode->NodeJointCoordinats[(int)ETrackCoordinatAppointment::Start] == JointCoordinate)
		{

			TrackCoordinats.push_back(SecondNode->GetStart());
			TrackCoordinats.back().TrackTangens /= TrackCoordinats.back().TrackTangens.Size();
			TrackCoordinats.back().TrackTangens *= FirstNode->GetOwner()->GetSize() * LevelSettings.CellSize;

		}
		else
		{
			throw FString("Cannt create link");
		}
	}
	else if (FirstNode->NodeJointCoordinats[(int)ETrackCoordinatAppointment::Start] == JointCoordinate)
	{
		EndNode = FirstNode;
		StartNode = SecondNode;

		if (SecondNode->NodeJointCoordinats[(int)ETrackCoordinatAppointment::End] == JointCoordinate)
		{
			TrackCoordinats.push_back(SecondNode->GetEnd());
			TrackCoordinats.back().TrackTangens /= TrackCoordinats.back().TrackTangens.Size();
			TrackCoordinats.back().TrackTangens *= FirstNode->GetOwner()->GetSize() * LevelSettings.CellSize;

		}
		else
		{
			throw FString("Cannt create link");
		}

		TrackCoordinats.push_back(FirstNode->GetStart());
		TrackCoordinats.back().TrackTangens /= TrackCoordinats.back().TrackTangens.Size();
		TrackCoordinats.back().TrackTangens *= SecondNode->GetOwner()->GetSize() * LevelSettings.CellSize;

	}

	
	CreateTrack();
}

FCarTrackGraphBase* FCarTrackLink::GetNextTrack(int LayerNum) const
{ 
	return EndNode;
}

//*****************************************
//FCarTrackNode
//****************************************

FCarTrackNode::FCarTrackNode(EHoverCarTrackDirection _Direction, std::pair<FVector, FVector> FirstPoint, std::pair<FVector, FVector> SecondPoint, const FLevelGenRoadBase* _TrackRoadOwner,
	const FLevelGeneratorSettings& LevelSettings, const FDataStorage& DataStorage)
	: FCarTrackGraphBase(LevelSettings, DataStorage), Direction(_Direction), TrackRoadOwner(_TrackRoadOwner)
{

	switch (Direction)
	{
	case EHoverCarTrackDirection::Pozitive:
	{
		if (FirstPoint.first.X == SecondPoint.first.X)
		{
			TrackCoordinats.push_back(FTrackCoordinate(FirstPoint.first.Y < SecondPoint.first.Y ? FirstPoint.first : SecondPoint.first));
			NodeJointCoordinats.push_back(FirstPoint.first.Y < SecondPoint.first.Y ? FirstPoint.second : SecondPoint.second);

			TrackCoordinats.push_back(FTrackCoordinate(FirstPoint.first.Y < SecondPoint.first.Y ? SecondPoint.first : FirstPoint.first));
			NodeJointCoordinats.push_back(FirstPoint.first.Y < SecondPoint.first.Y ? SecondPoint.second : FirstPoint.second);
		}
		else
		{
			TrackCoordinats.push_back(FTrackCoordinate(FirstPoint.first.X < SecondPoint.first.X ? FirstPoint.first : SecondPoint.first));
			NodeJointCoordinats.push_back(FirstPoint.first.X < SecondPoint.first.X ? FirstPoint.second : SecondPoint.second);

			TrackCoordinats.push_back(FTrackCoordinate(FirstPoint.first.X < SecondPoint.first.X ? SecondPoint.first : FirstPoint.first));
			NodeJointCoordinats.push_back(FirstPoint.first.X < SecondPoint.first.X ? SecondPoint.second : FirstPoint.second);
		}

		break;
	}

	case EHoverCarTrackDirection::Negotive:
	{


		if (FirstPoint.first.X == SecondPoint.first.X)
		{
			TrackCoordinats.push_back(FTrackCoordinate(FirstPoint.first.Y > SecondPoint.first.Y ? FirstPoint.first : SecondPoint.first));
			NodeJointCoordinats.push_back(FirstPoint.first.Y > SecondPoint.first.Y ? FirstPoint.second : SecondPoint.second);

			TrackCoordinats.push_back(FTrackCoordinate(FirstPoint.first.Y > SecondPoint.first.Y ? SecondPoint.first : FirstPoint.first));
			NodeJointCoordinats.push_back(FirstPoint.first.Y > SecondPoint.first.Y ? SecondPoint.second : FirstPoint.second);
		}
		else
		{
			TrackCoordinats.push_back(FTrackCoordinate(FirstPoint.first.X > SecondPoint.first.X ? FirstPoint.first : SecondPoint.first));
			NodeJointCoordinats.push_back(FirstPoint.first.X > SecondPoint.first.X ? FirstPoint.second : SecondPoint.second);

			TrackCoordinats.push_back(FTrackCoordinate(FirstPoint.first.X > SecondPoint.first.X ? SecondPoint.first : FirstPoint.first));
			NodeJointCoordinats.push_back(FirstPoint.first.X > SecondPoint.first.X ? SecondPoint.second : FirstPoint.second);
		}

		break;
	}

	default: throw;

	}

	TrackCoordinats[(int)ETrackCoordinatAppointment::Start].TrackTangens = GetEnd().TrackCoordinate - GetStart().TrackCoordinate;
	TrackCoordinats[(int)ETrackCoordinatAppointment::End].TrackTangens = GetStart().TrackTangens;

	CreateTrack();
}


FCarTrackGraphBase* FCarTrackNode::GetNextTrack(int LayerNum) const
{
	std::vector<const FCarTrackNode*> ActiveLink;


	for (auto CurrentLink = Links.begin(); CurrentLink != Links.end(); CurrentLink++)
	{
		if (CurrentLink->first->IsTrackActive())
		{
			ActiveLink.push_back(CurrentLink->first);
		}
	}

	if (ActiveLink.size() == 0)
	{
		return DataStorage.SpawnCarTrack[LayerNum][GetRandomNumber(0, DataStorage.SpawnCarTrack[(int)Direction].size() - 1)];
	}

	return Links.find(ActiveLink[GetRandomNumber(0, ActiveLink.size() - 1)])->second.get();
	
}