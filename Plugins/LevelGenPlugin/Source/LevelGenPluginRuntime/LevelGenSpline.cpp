// Copyright 2018 Pavlov Dmitriy
#include "LevelGenSpline.h"

FLevelGenSpline::FLevelGenSpline(FVector Coordinate_1, FVector Tangens_1, FVector Coordinate_2, FVector Tangens_2) : SplineCoordinate(Coordinate_1)
{
	Curve.Position.Points.Reset(2);
	Curve.Rotation.Points.Reset(2);
	Curve.Scale.Points.Reset(2);

	FVector SecondCoordinate = Coordinate_2 - Coordinate_1;

	Curve.Position.Points.Emplace(0.0f, FVector(0, 0, 0), Tangens_1, Tangens_1, CIM_CurveUser);
	Curve.Rotation.Points.Emplace(0.0f, FQuat::Identity, FQuat::Identity, FQuat::Identity, CIM_CurveAuto);
	Curve.Scale.Points.Emplace(0.0f, FVector(1.0f), FVector::ZeroVector, FVector::ZeroVector, CIM_CurveAuto);

	Curve.Position.Points.Emplace(1.0f, SecondCoordinate, Tangens_2, Tangens_2, CIM_CurveUser);
	Curve.Rotation.Points.Emplace(1.0f, FQuat::Identity, FQuat::Identity, FQuat::Identity, CIM_CurveUser);
	Curve.Scale.Points.Emplace(1.0f, FVector(1.0f), FVector::ZeroVector, FVector::ZeroVector, CIM_CurveUser);

	Curve.UpdateSpline(false, false, ReparamStepsPerSegment, false, 0, FVector(1.0f));

	
}

FVector FLevelGenSpline::GetLocationAtDistanceAlongSpline(float Distance) const
{
	const float Param = Curve.ReparamTable.Eval(Distance, 0.0f);
	return GetLocationAtSplineInputKey(Param);
}

FVector FLevelGenSpline::GetLocationAtSplineInputKey(float InKey) const
{
	FVector Location = Curve.Position.Eval(InKey, FVector::ZeroVector) + SplineCoordinate;

	return Location;
}

FVector FLevelGenSpline::GetTangentAtDistanceAlongSpline(float Distance) const
{
	const float Param = Curve.ReparamTable.Eval(Distance, 0.0f);

	return GetTangentAtSplineInputKey(Param);
}

FVector FLevelGenSpline::GetTangentAtSplineInputKey(float InKey) const
{
	FVector Tangent = Curve.Position.EvalDerivative(InKey, FVector::ZeroVector);

	return Tangent;
}

