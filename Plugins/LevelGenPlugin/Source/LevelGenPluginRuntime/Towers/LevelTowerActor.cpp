// Copyright 2018 Pavlov Dmitriy
#include "LevelTowerActor.h"



ALevelTowerActorTower::ALevelTowerActorTower()
{

	TowerBordersShower = CreateDefaultSubobject<UTowersBordersShowComponentTower>(TEXT("RoomBordersShowerNode"));
	RootComponent = TowerBordersShower;
	TowerBordersShower->GetTower()->TowerActorClass = this->GetClass();
#if WITH_EDITORONLY_DATA
	TowerBordersShower->DrawTower();
#endif //WITH_EDITORONLY_DATA
}

ALevelTowerActorBilding::ALevelTowerActorBilding()
{
	BildingBordersShower = CreateDefaultSubobject<UTowersBordersShowComponentBilding>(TEXT("RoomBordersShowerLink"));
	RootComponent = BildingBordersShower;
	BildingBordersShower->GetTower()->TowerActorClass = this->GetClass();
#if WITH_EDITORONLY_DATA
	BildingBordersShower->DrawTower();
#endif //WITH_EDITORONLY_DATA
}

