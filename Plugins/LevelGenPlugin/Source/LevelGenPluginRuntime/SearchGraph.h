// Copyright 2018 Pavlov Dmitriy
#pragma once
#include "Rooms/LevelRooms.h"
#include <map>
#include <set>
#include "LevelGenCore.h" 

struct FLevelGraphLink;
class ALevelGenerator;

struct FValidationResult
{
	FValidationResult() = default;
	FValidationResult(float _HorizontalDist, float _VerticalDist) : HorizontalDist(_HorizontalDist), VerticalDist(_VerticalDist) {}

	float HorizontalDist;
	float VerticalDist;

	float GetSize() const { return std::pow(std::pow(HorizontalDist, 2) + std::pow(VerticalDist, 2), 0.5); }

	bool operator< (const FValidationResult& Ar2) const { return GetSize() < Ar2.GetSize(); }

	static const FValidationResult Zero;
};

class USearchGraph
{
public:
	USearchGraph(FJointPart& In, FJointPart& Out, const FDataStorage& _DataStorage, const ALevelGenerator& _LevelGenerator);

	void GetRoomChain(std::vector<std::shared_ptr<FSearchGraphNode>>& RoomChain);

	

private:
	void CreateNewSearchGraphNods(std::vector<std::pair<FValidationResult, std::shared_ptr<FSearchGraphNode>>>& NewNods, FJointPart& InPortal, const FJointPart& OutPortal,
		std::weak_ptr<FSearchGraphNode> LinkedNode, FValidationResult LastValidationResult);

	bool IsSelfIntersectionPresent(std::shared_ptr<FSearchGraphNode> LastRoom);

	

	inline FValidationResult ValuationFunction(const FSearchGraphNode& CurrentNode, const FJointPart& SecondPart, FValidationResult LastValidationResult);

private:

	std::multimap<FValidationResult, std::shared_ptr<FSearchGraphNode>> Graph;

	const ALevelGenerator& LevelGenerator;
	
	const FDataStorage& DataStorage;
};