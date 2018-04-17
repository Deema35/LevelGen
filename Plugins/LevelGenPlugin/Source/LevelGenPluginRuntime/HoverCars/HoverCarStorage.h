// Copyright 2018 Pavlov Dmitriy
#pragma once
#include "CoreMinimal.h"
#include "Rooms/RoomStorage.h"
#include "HoverCarStorage.generated.h"

class AHoverCarActor;
class UHoverCarActorContainer;

UCLASS()
class LEVELGENPLUGINRUNTIME_API UHoverCarStorage : public UActorsStorageBase
{
	GENERATED_BODY()
public:

	void GetHoverCars(std::vector<UClass*>& _HoverCars) const;

public:

	UPROPERTY(EditAnywhere)
	TArray<UHoverCarActorContainer*> HoverCars;
};