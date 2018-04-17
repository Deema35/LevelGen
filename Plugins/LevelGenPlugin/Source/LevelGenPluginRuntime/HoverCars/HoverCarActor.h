// Copyright 2018 Pavlov Dmitriy
#pragma once
#include "GameFramework/Actor.h"
#include <memory>
#include <mutex>
#include "HoverCarActor.generated.h"

class FHoverCar;

UCLASS(Abstract)

class LEVELGENPLUGINRUNTIME_API AHoverCarActor : public AActor
{
	GENERATED_BODY()

public:

	AHoverCarActor();
	
	virtual void Tick(float DeltaSeconds) override;

	

public:
	UPROPERTY(EditAnywhere, meta = (ClampMin = 1))
		int32 Speed = 5000;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 1))
		int32 ForwardCarDistanse = 50;

	
	void SetHoverCar(std::weak_ptr<FHoverCar> _HoverCar) { HoverCar = _HoverCar; }
	
private:

	std::mutex DataLock;

	std::weak_ptr<FHoverCar> HoverCar;
};