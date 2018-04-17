// Copyright 2018 Pavlov Dmitriy
#pragma once
#include "CoreMinimal.h"
#include <atomic>

class AHoverCarActor;

class FHoverCar
{
public:

	FHoverCar(float _CurrentTrackCoordinate, float _CurrentTrackLenght) : CurrentTrackCoordinate(_CurrentTrackCoordinate), CurrentTrackLenght(_CurrentTrackLenght) {}
	
	void SetSpeed(float _Speed) { Speed = _Speed; }
	
	void GetCoordinate(FVector& _Coordinate, FVector& _Normale) const;

	void SetCoordinate(FVector _Coordinate, FVector _Normale);

	float CurrentTrackCoordinate = 0;

	float CurrentTrackLenght = 0;

	std::atomic<int> X = 0;

	std::atomic<int> Y = 0;

	std::atomic<int> Z = 0;

	std::atomic<int> NormaleX = 0;

	std::atomic<int> NormaleY = 0;

	std::atomic<int> NormaleZ = 0;

	std::atomic<int> Speed = 0;
};

