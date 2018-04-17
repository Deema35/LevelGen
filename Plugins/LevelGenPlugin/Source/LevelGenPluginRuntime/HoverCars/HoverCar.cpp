// Copyright 2018 Pavlov Dmitriy
#include "HoverCar.h"
#include "CarTrackGraph.h"
#include "HoverCarActor.h"


void FHoverCar::GetCoordinate(FVector& _Coordinate, FVector& _Normale) const
{
	_Coordinate.X = X;
	_Coordinate.Y = Y;
	_Coordinate.Z = Z;

	_Normale.X = NormaleX;
	_Normale.Y = NormaleY;
	_Normale.Z = NormaleZ;
}

void FHoverCar::SetCoordinate(FVector _Coordinate, FVector _Normale)
{
	X = _Coordinate.X;

	Y = _Coordinate.Y;

	Z = _Coordinate.Z;

	NormaleX = _Normale.X;

	NormaleY = _Normale.Y;

	NormaleZ = _Normale.Z;
}

