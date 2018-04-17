// Copyright 2018 Pavlov Dmitriy
#include "LevelTowers.h"
#include "LevelCell.h"

//............................................
//FLevelTower
//............................................

bool FLevelTower::IsPlaceFit(FVector2D Coordinate, EYawTurn Yaw, const FDataStorage& DataStorage) const
{
	switch (Yaw)
	{
	case EYawTurn::Zero:
	case EYawTurn::Pi:
		for (int i = Coordinate.X; i < Coordinate.X + Size.X; i++)
		{
			for (int j = Coordinate.Y; j < Coordinate.Y + Size.Y; j++)
			{

				if (!IsCellFit(i, j, DataStorage)) return false;

			}
		}
		return true;

	case EYawTurn::HalfPi:
	case EYawTurn::PiAndHalf:

		for (int i = Coordinate.X; i < Coordinate.X + Size.Y; i++)
		{
			for (int j = Coordinate.Y; j < Coordinate.Y + Size.X; j++)
			{

				if (!IsCellFit(i, j, DataStorage)) return false;

			}
		}
		return true;

	default: throw FString("Error");
	}

}

bool FLevelTower::IsCellFit(int X, int Y, const FDataStorage& DataStorage) const
{
	if (DataStorage.LevelMap.GetCell(X, Y)->CellInst) return false;

	if (DataStorage.LevelMap.GetCell(X, Y + 1)->CellInst && DataStorage.LevelMap.GetCell(X, Y + 1)->CellInst->GetCellType() == ELevelCellType::Tower) return false;

	if (DataStorage.LevelMap.GetCell(X, Y - 1)->CellInst && DataStorage.LevelMap.GetCell(X, Y - 1)->CellInst->GetCellType() == ELevelCellType::Tower) return false;

	if (DataStorage.LevelMap.GetCell(X + 1, Y + 1)->CellInst && DataStorage.LevelMap.GetCell(X + 1, Y + 1)->CellInst->GetCellType() == ELevelCellType::Tower) return false;

	if (DataStorage.LevelMap.GetCell(X + 1, Y)->CellInst && DataStorage.LevelMap.GetCell(X + 1, Y)->CellInst->GetCellType() == ELevelCellType::Tower) return false;

	if (DataStorage.LevelMap.GetCell(X + 1, Y - 1)->CellInst && DataStorage.LevelMap.GetCell(X + 1, Y - 1)->CellInst->GetCellType() == ELevelCellType::Tower) return false;

	if (DataStorage.LevelMap.GetCell(X - 1, Y + 1)->CellInst && DataStorage.LevelMap.GetCell(X - 1, Y + 1)->CellInst->GetCellType() == ELevelCellType::Tower) return false;

	if (DataStorage.LevelMap.GetCell(X - 1, Y)->CellInst && DataStorage.LevelMap.GetCell(X - 1, Y)->CellInst->GetCellType() == ELevelCellType::Tower) return false;

	if (DataStorage.LevelMap.GetCell(X - 1, Y - 1)->CellInst && DataStorage.LevelMap.GetCell(X - 1, Y - 1)->CellInst->GetCellType() == ELevelCellType::Tower) return false;

	return true;
}
//............................................
//FLevelBilding
//............................................
bool FLevelBilding::IsPlaceFit(FVector2D Coordinate, EYawTurn Yaw, const FDataStorage& DataStorage) const
{
	switch (Yaw)
	{
	case EYawTurn::Zero:
	case EYawTurn::Pi:

		for (int i = Coordinate.X; i < Coordinate.X + Size.X; i++)
		{
			for (int j = Coordinate.Y; j < Coordinate.Y + Size.Y; j++)
			{
				if (!DataStorage.LevelMap.GetCell(i, j)->CellInst) return false;

				if (DataStorage.LevelMap.GetCell(i, j)->LevelBilding) return false;

				if (BildingHeight > DataStorage.LevelMap.GetCell(i, j)->CellInst->GetBildingCellHeight()) return false;
			}
		}

		return true;

	case EYawTurn::HalfPi:
	case EYawTurn::PiAndHalf:

		for (int i = Coordinate.X; i < Coordinate.X + Size.Y; i++)
		{
			for (int j = Coordinate.Y; j < Coordinate.Y + Size.X; j++)
			{
				if (!DataStorage.LevelMap.GetCell(i, j)->CellInst) return false;

				if (DataStorage.LevelMap.GetCell(i, j)->LevelBilding) return false;

				if (BildingHeight > DataStorage.LevelMap.GetCell(i, j)->CellInst->GetBildingCellHeight()) return false;
			}
		}

		return true;

	default: throw FString("Error");
	}


}