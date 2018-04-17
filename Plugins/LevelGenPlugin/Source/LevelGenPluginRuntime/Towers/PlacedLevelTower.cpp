// Copyright 2018 Pavlov Dmitriy
#include "PlacedLevelTower.h"
#include "LevelCell.h"
#include "LevelTowers.h"

//............................................
//FPlacedLevelTower
//............................................


void FPlacedLevelTower::LinkedTowerToLevelCells(FDataStorage& DataStorage, const FLevelGeneratorSettings& LevelSettings)
{

	for (int i = StartCoordinate.X; i < StartCoordinate.X + Tower->Size.X; i++)
	{
		for (int j = StartCoordinate.Y; j < StartCoordinate.Y + Tower->Size.Y; j++)
		{
			if (!DataStorage.LevelMap.GetCell(i, j)->CellInst)
			{
				DataStorage.LevelMap.GetCell(i, j)->CellInst = std::shared_ptr<FLevelCellBase>(ELevelCellTypeCreate(ELevelCellType::Tower, *DataStorage.LevelMap.GetCell(i, j)));

			}
			else
			{
				
				UE_LOG(LogTemp, Error, TEXT("FPlacedLevelTower::LinkedTowerToLevelCells bad cell type %s"),
					*GetEnumValueAsString<ELevelCellType>("ELevelCellType", DataStorage.LevelMap.GetCell(i, j)->CellInst->GetCellType()));
				
			}

		}
	}
}

//............................................
//FPlacedLevelBilding
//............................................


void FPlacedLevelBilding::LinkedTowerToLevelCells(FDataStorage& DataStorage, const FLevelGeneratorSettings& LevelSettings)
{
	std::shared_ptr<FPlacedLevelBilding> CurrentPointer(const_cast<FPlacedLevelBilding*>(this));

	switch (Yaw)
	{
	case EYawTurn::Zero:
	case EYawTurn::Pi:

		for (int i = StartCoordinate.X; i < StartCoordinate.X + Bilding->Size.X; i++)
		{
			for (int j = StartCoordinate.Y; j < StartCoordinate.Y + Bilding->Size.Y; j++)
			{
				DataStorage.LevelMap.GetCell(i, j)->LevelBilding = CurrentPointer;
			}
		}

		break;

	case EYawTurn::HalfPi:
	case EYawTurn::PiAndHalf:

		for (int i = StartCoordinate.X; i < StartCoordinate.X + Bilding->Size.Y; i++)
		{
			for (int j = StartCoordinate.Y; j < StartCoordinate.Y + Bilding->Size.X; j++)
			{
				DataStorage.LevelMap.GetCell(i, j)->LevelBilding = CurrentPointer;
			}
		}

		break;

	default: throw;
	}
}