// Copyright 2018 Pavlov Dmitriy
#pragma once

#include "CoreMinimal.h"
#include <vector>
#include <memory>
#include <utility>
#include <tuple>
#include <map>
#include "LevelGenCore.h"
#include "LevelTowers.generated.h"

struct FDataStorage;

USTRUCT(BlueprintType)
struct FLevelTowerBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FVector2D Size = FVector2D(1, 1);

	

	UPROPERTY()
	UClass* TowerActorClass = nullptr;

	FVector2D GetSize() const { return Size; }

	UClass* GetTowerActorClass() const { return TowerActorClass; }

	virtual bool IsPlaceFit(FVector2D Coordinate, EYawTurn Yaw, const FDataStorage& DataStorage) const { return false; }
};

USTRUCT()
struct FLevelTower : public FLevelTowerBase
{
	GENERATED_BODY()

	virtual bool IsPlaceFit(FVector2D Coordinate, EYawTurn Yaw, const FDataStorage& DataStorage) const override;

private:
	bool IsCellFit(int X, int Y, const FDataStorage& DataStorage) const;
};

USTRUCT(BlueprintType)
struct FLevelBilding : public FLevelTowerBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int32 BildingHeight;

	bool IsPlaceFit(FVector2D Coordinate, EYawTurn Yaw, const FDataStorage& DataStorage) const override;

};