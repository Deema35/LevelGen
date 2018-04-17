// Copyright 2018 Pavlov Dmitriy
#include "HoverCarActor.h"
#include "HoverCar.h"
#include "CarTrackGraph.h"

AHoverCarActor::AHoverCarActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void  AHoverCarActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	std::shared_ptr<FHoverCar>pHoverCar = HoverCar.lock();

	if (!pHoverCar)
	{
		Destroy();
	}
	else
	{
		FVector Coordinate;
		FVector Tangent;

		pHoverCar->GetCoordinate(Coordinate, Tangent);

		SetActorLocationAndRotation(Coordinate, FQuat(Tangent.Rotation()));

	}
}

