// Copyright 2018 Pavlov Dmitriy
#include "HoverCarStorage.h"
#include "HoverCarActor.h"
#include "ActorContainer.h"

void UHoverCarStorage::GetHoverCars(std::vector<UClass *>& _HoverCars) const
{
	_HoverCars.reserve(HoverCars.Num());

	for (int i = 0; i < HoverCars.Num(); i++)
	{
		if (HoverCars[i])
		{
			AHoverCarActor* CurrentHoverCar = Cast<AHoverCarActor>(HoverCars[i]->HoverCar->GetDefaultObject());
			
			_HoverCars.push_back(CurrentHoverCar->GetClass());
		}

	}
}