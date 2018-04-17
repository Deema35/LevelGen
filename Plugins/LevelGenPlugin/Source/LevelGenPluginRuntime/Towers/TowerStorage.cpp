// Copyright 2018 Pavlov Dmitriy
#include "TowerStorage.h"
#include <algorithm> 
#include "LevelCell.h"
#include "LevelGenerator.h"
#include "LevelGeneratorSettings.h"
#include "Towers/LevelTowers.h"
#include "Towers/PlacedLevelTower.h"
#include "LevelGenBildingZone.h"
#include "LevelTowerActor.h"
#include "ActorContainer.h"

//............................................
//UTowerStorage
//............................................

void UTowerStorage::GetTowers(std::vector<const FLevelTower*>& _Towers) const
{
	_Towers.reserve(Towers.Num());
	for (int i = 0; i < Towers.Num(); i++)
	{
		if (Towers[i])
		{
			ALevelTowerActorTower* CurrentTower = Cast<ALevelTowerActorTower>(Towers[i]->Tower->GetDefaultObject());
			_Towers.push_back(&CurrentTower->TowerBordersShower->Tower);
		}
		
	}
}


void UTowerStorage::GetBildings(std::vector<const FLevelBilding*>& _Bildings) const
{
	_Bildings.reserve(Bildings.Num());
	for (int i = 0; i < Bildings.Num(); i++)
	{
		if (Bildings[i])
		{
			ALevelTowerActorBilding* CurrentBilding = Cast<ALevelTowerActorBilding>(Bildings[i]->Bilding->GetDefaultObject());
			_Bildings.push_back(&CurrentBilding->BildingBordersShower->Bilding);
		}
		
	}
}

