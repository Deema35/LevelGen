// Copyright 2018 Pavlov Dmitriy
#pragma once

#include "Components/SplineComponent.h"

class FLevelGenSpline
{
public:
	FLevelGenSpline(FVector Coordinate_1, FVector Tangens_1, FVector Coordinate_2, FVector Tangens_2);

	FVector GetLocationAtDistanceAlongSpline(float Distance) const;

	FVector GetTangentAtDistanceAlongSpline(float Distance) const;

	float GetSplineLength() const { return Curve.GetSplineLength(); }

private:

	FVector GetLocationAtSplineInputKey(float InKey) const;

	FVector GetTangentAtSplineInputKey(float InKey) const;

private:

	int ReparamStepsPerSegment = 10;

	FSplineCurves Curve;

	FVector SplineCoordinate;
};