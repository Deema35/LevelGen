// Copyright 2018 Pavlov Dmitriy
#pragma once
#include <memory>
#include <utility>
#include <tuple>
#include <map>
#include "LevelGenCore.h"

struct FLevelTower;
struct FLevelBilding;
struct FDataStorage;
struct FLevelGeneratorSettings;
struct FLevelTowerBase;

struct FPlacedLevelTowerBase
{
	FPlacedLevelTowerBase(FVector2D _StartCoordinate, EYawTurn _Yaw, FDataStorage& DataStorage, const FLevelGeneratorSettings& _LevelSettings)
		:  StartCoordinate(_StartCoordinate), Yaw(_Yaw) {}
public:

	FVector2D GetStartCoordinate() { return StartCoordinate;}

	virtual const FLevelTowerBase* GetTower() const { throw FString("Error"); }

	virtual ~FPlacedLevelTowerBase() {}

public:
	FVector2D StartCoordinate;

	EYawTurn Yaw;

};

struct FPlacedLevelTower : FPlacedLevelTowerBase
{
	FPlacedLevelTower(const FLevelTower* _Tower, FVector2D _StartCoordinate, EYawTurn _Yaw, FDataStorage& DataStorage, const FLevelGeneratorSettings& _LevelSettings)
		: Tower(_Tower), FPlacedLevelTowerBase(_StartCoordinate, _Yaw, DataStorage, _LevelSettings)
	{
		LinkedTowerToLevelCells(DataStorage, _LevelSettings);
	}
	
	virtual const FLevelTowerBase* GetTower() const override { return Tower; }

private:
	const FLevelTower* Tower = nullptr;

	void LinkedTowerToLevelCells(FDataStorage& DataStorage, const FLevelGeneratorSettings& LevelSettings);

};

struct FPlacedLevelBilding : FPlacedLevelTowerBase
{
	FPlacedLevelBilding(const FLevelBilding* _Bilding, FVector2D _StartCoordinate, EYawTurn _Yaw, FDataStorage& DataStorage, const FLevelGeneratorSettings& _LevelSettings)
		: Bilding(_Bilding), FPlacedLevelTowerBase(_StartCoordinate, _Yaw, DataStorage, _LevelSettings)
	{
		LinkedTowerToLevelCells(DataStorage, _LevelSettings);
	}
	
	virtual const FLevelTowerBase* GetTower() const override { return Bilding; }

private:

	const FLevelBilding* Bilding = nullptr;

	void LinkedTowerToLevelCells(FDataStorage& DataStorage, const FLevelGeneratorSettings& LevelSettings);

};
