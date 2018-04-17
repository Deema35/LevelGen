// Copyright 2018 Pavlov Dmitriy
#pragma once
#include "CoreMinimal.h"
#include <vector>
#include <memory>
#include "LevelGenCore.h" 
#include "Rooms/RoomStorage.h"
#include "TowerStorage.generated.h"

class UStaticMesh;
struct FLevelGeneratorSettings;
struct FDataStorage;
class ALevelGenerator;
struct FPlacedLevelTower;
struct FLevelTower;
struct FLevelBilding;
class UTowerActorContainer;
class UBildingActorContainer;

UCLASS()
class LEVELGENPLUGINRUNTIME_API UTowerStorage : public UActorsStorageBase
{
	GENERATED_BODY()

public:

	void GetTowers(std::vector<const FLevelTower*>& _Towers) const;

	void GetBildings(std::vector<const FLevelBilding*>& _Bildings) const;

public:

	UPROPERTY(EditAnywhere)
		TArray<UTowerActorContainer*> Towers;

	UPROPERTY(EditAnywhere)
		TArray<UBildingActorContainer*> Bildings;

};


