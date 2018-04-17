// Copyright 2018 Pavlov Dmitriy
#pragma once
#include "GameFramework/Actor.h"
#include "LevelTowers.h"
#include "TowersBordersShowComponent.h"
#include "Rooms/LevelRoomActor.h"
#include "LevelTowerActor.generated.h"



UCLASS(Abstract)
class LEVELGENPLUGINRUNTIME_API ALevelTowerActorTower : public ALevelGenActorBace
{
	GENERATED_BODY()
public:
	ALevelTowerActorTower();

	virtual ULevelGenBordersShowComponentBace* GetBordersShowComponent() override { return TowerBordersShower; }
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UTowersBordersShowComponentTower* TowerBordersShower = nullptr;

};

UCLASS(Abstract)
class LEVELGENPLUGINRUNTIME_API ALevelTowerActorBilding : public ALevelGenActorBace
{
	GENERATED_BODY()
public:
	ALevelTowerActorBilding();

	virtual ULevelGenBordersShowComponentBace* GetBordersShowComponent() override { return BildingBordersShower; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UTowersBordersShowComponentBilding* BildingBordersShower = nullptr;


};

