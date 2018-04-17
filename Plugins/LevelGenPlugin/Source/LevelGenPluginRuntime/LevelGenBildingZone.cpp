// Copyright 2018 Pavlov Dmitriy
#include "LevelGenBildingZone.h"
#include "LevelCell.h"
#include "LevelGenerator.h"
#include "LevelGeneratorSettings.h"
#include "Towers/LevelTowers.h"
#include "Towers/PlacedLevelTower.h"
#include "DataStorage.h"
#include "ProceduralMeshActor.h"
#include "ProceduralFigure.h"
#include "LevelGraphNode.h"
#include "Rooms/PlacedLevelRoom.h"
#include "LevelFloor.h"
//............................................
//FLevelGenBildingZone
//............................................

FLevelGenBildingZoneBase::FLevelGenBildingZoneBase(bool UnPlacebelBilding, FVector2D Start, FVector2D End, FDataStorage& _DataStorage, const FLevelGeneratorSettings& _LevelSettings,
	const ALevelGenerator& _LevelGenerator)
	: Coordinate(Start), Size(End - Start), LevelSettings(_LevelSettings), LevelGenerator(_LevelGenerator), DataStorage(_DataStorage)
{

	if (UnPlacebelBilding)
	{
		for (int i = Coordinate.X; i < Coordinate.X + Size.X; i++)
		{
			for (int j = Coordinate.Y; j < Coordinate.Y + Size.Y; j++)
			{
				if (!DataStorage.LevelMap.GetCell(i, j)->CellInst)
				{
					DataStorage.LevelMap.GetCell(i, j)->CellInst = std::shared_ptr<FLevelCellBase>(ELevelCellTypeCreate(ELevelCellType::Bilding, *DataStorage.LevelMap.GetCell(i, j)));

				}
				
			}
		}
		SetState(EBildingZoneState::UnPlasebel);
		return;
	}
		
}

void FLevelGenBildingZoneBase::CreateGraphLayers()
{
	int FloorsIntervalNum = LevelSettings.GraphLayerNum + 1; // if 2 layer we need 3 intervals, becose upper and lower interval intersec



	int FloorsForInterval = LevelSettings.FloorNum / FloorsIntervalNum;
	int FloorsForIntervalOst = LevelSettings.FloorNum % FloorsIntervalNum;

	std::vector<std::pair<int, int>> Intervals;

	for (int i = 0; i < FloorsIntervalNum; i++)
	{

		int Down = i * FloorsForInterval;
		int Up = ((i + 1) * FloorsForInterval - 1);

		if (i == FloorsIntervalNum - 1)
		{
			Up += FloorsForIntervalOst;
		}


		Intervals.push_back(std::make_pair(Down, Up));

	}

	GraphLayers.reserve(LevelSettings.GraphLayerNum);

	for (int i = 0; i < LevelSettings.GraphLayerNum; i++)
	{

		GraphLayers.push_back(FLvevelLayer(Intervals[i], Intervals[i + 1]));

	}
}

void FLevelGenBildingZoneBase::CreateTowers(const FLevelGeneratorSettings& LevelSettings, const ALevelGenerator& LevelGenerator, FDataStorage& DataStorage)
{
	FVector2D NumTowers(RoundNumber(Size.X / LevelSettings.TowerFrequency, ERoundType::Up), RoundNumber(Size.Y / LevelSettings.TowerFrequency, ERoundType::Up));

	std::vector<const FLevelTower*> TowersFromStorage;
	LevelGenerator.GetTowerStorage()->GetTowers(TowersFromStorage);


	std::shuffle(TowersFromStorage.begin(), TowersFromStorage.end(), LevelGenerator.GetRandomGenerator().GetGenerator());

	UObjectsDisposer TowerDisposer(std::make_pair(Coordinate, Coordinate + Size), LevelGenerator.GetRandomGenerator().GetGenerator(), LevelSettings);

	if (NumTowers.X < 0 || NumTowers.Y < 0) throw;

	BildingTowers.resize(NumTowers.X);

	for (int i = 0; i < NumTowers.X; i++)
	{


		BildingTowers[i].resize(NumTowers.Y);
		for (int j = 0; j < NumTowers.Y; j++)
		{
			for (int k = 0; k < TowersFromStorage.size(); k++)
			{
				auto Predicate = [&](FVector Coordinate)
				{
					return TowersFromStorage[k]->IsPlaceFit(FVector2D(Coordinate), EYawTurn::Zero, DataStorage);
				};

				FVector2D ToverCoordinate(0,0);

				int CellOffsetX = LevelGenerator.GetRandomGenerator().GetRandomNumber(-LevelSettings.TowerFrequency / 2, LevelSettings.TowerFrequency / 2);
				int CellOffsetY = LevelGenerator.GetRandomGenerator().GetRandomNumber(-LevelSettings.TowerFrequency / 2, LevelSettings.TowerFrequency / 2);


				FVector2D SearchStartCoordinate = Coordinate + FVector2D(i * LevelSettings.TowerFrequency + CellOffsetX, j * LevelSettings.TowerFrequency + CellOffsetY);

				if (TowerDisposer.GetLocation(ToverCoordinate, SearchStartCoordinate, 0, TowersFromStorage[k]->Size, Predicate))
				{
					BildingTowers[i][j] = std::shared_ptr<FPlacedLevelTower>(new FPlacedLevelTower(TowersFromStorage[k], ToverCoordinate, EYawTurn::Zero, DataStorage, LevelSettings));
					break;
				}
			}

		}
	}
}



std::pair<int, int> FLevelGenBildingZoneBase::GetCoordinateRange(EDirection Direction)
{

	switch (Direction)
	{
	case EDirection::XP:
	case EDirection::XM:

		return std::pair<int, int>(Coordinate.Y, Coordinate.Y + Size.Y - 1);


	case EDirection::YP:
	case EDirection::YM:

		return std::pair<int, int>(Coordinate.X, Coordinate.X + Size.X - 1);

	default: throw;

	}

	
}


void FLevelGenBildingZoneBase::GetFasedeWallCellsCoordinats(std::vector<FVector2D>& WallCells, EDirection Direction)
{
	switch (Direction)
	{
	case EDirection::XP:

		for (int i = 0; i < GetSize().Y; i++)
		{
			WallCells.push_back(FVector2D(GetBeginCoordinate().X - 1, GetBeginCoordinate().Y + i));
		}

		break;

	case EDirection::XM:

		for (int i = 0; i < GetSize().Y; i++)
		{
			WallCells.push_back(FVector2D(GetBeginCoordinate().X + GetSize().X, GetBeginCoordinate().Y + GetSize().Y - i - 1));
		}

		break;

	case EDirection::YP:

		for (int i = 0; i < GetSize().X; i++)
		{
			WallCells.push_back(FVector2D(GetBeginCoordinate().X + GetSize().X - i - 1, GetBeginCoordinate().Y - 1));
		}

		break;

	case EDirection::YM:

		for (int i = 0; i < GetSize().X; i++)
		{
			WallCells.push_back(FVector2D(GetBeginCoordinate().X + i, GetBeginCoordinate().Y + GetSize().Y));
		}

		break;
	}
}




void FLevelGenBildingZoneBase::CreateNods()
{
	
	
		std::pair<FVector2D, FVector2D> GeneratedArea = std::make_pair(GetBeginCoordinate(), GetBeginCoordinate() + GetSize() - FVector2D(1, 1));


		FVector2D NodNum(RoundNumber(GetSize().X / LevelSettings.GraphNodsFrequency, ERoundType::Up), RoundNumber(GetSize().Y / LevelSettings.GraphNodsFrequency, ERoundType::Up));


		for (int k = 0; k < GraphLayers.size(); k++)
		{
			GraphLayers[k].Graph.resize(NodNum.X);

			for (int i = 0; i < NodNum.X; i++)
			{
				GraphLayers[k].Graph[i].resize(NodNum.Y);

				for (int j = 0; j < NodNum.Y; j++)
				{
					bool InUpperInterval = LevelGenerator.GetRandomGenerator().GetRandomNumber(0, 1) == 1;

					if (!InUpperInterval && k != 0 && GraphLayers[k - 1].Graph[i][j]->InUpperLayer && !GraphLayers[k - 1].Graph[i][j]->FromLastGraph)
					{
						GraphLayers[k - 1].Graph[i][j]->FromLastGraph = true;
						GraphLayers[k - 1].Graph[i][j]->CreateLinks(FVector2D(i, j), GraphLayers[k]);
						GraphLayers[k].Graph[i][j] = GraphLayers[k - 1].Graph[i][j];
					}
					else
					{
						std::shared_ptr<FLevelGraphNode> NewNode = std::shared_ptr<FLevelGraphNode>(new FLevelGraphNode(InUpperInterval, GeneratedArea, DataStorage, LevelSettings, LevelGenerator));
						NewNode->CreateLinks(FVector2D(i, j), GraphLayers[k]);
						GraphLayers[k].Graph[i][j] = NewNode;

					}

				}
			}

		}


		
	

}




void FLevelGenBildingZoneBase::SetNodsRoom()
{
	
	for (int k = 0; k < GraphLayers.size(); k++)
	{
		for (int i = 0; i < GraphLayers[k].Graph.size(); i++)
		{
			for (int j = 0; j < GraphLayers[k].Graph[i].size(); j++)
			{
				FLevelGraphNode& CurrentNode = *GraphLayers[k].Graph[i][j];

					
				std::pair<int, int> FloorsInterval;
				if (CurrentNode.InUpperLayer)
				{
					FloorsInterval = GraphLayers[k].UpperLayerFlorsInterval;
				}
				else
				{

					FloorsInterval = GraphLayers[k].LowerLayerFlorsInterval;

				}

				FVector2D SearchStart = GetBeginCoordinate() + FVector2D(i * LevelSettings.GraphNodsFrequency, j * LevelSettings.GraphNodsFrequency);

				if (!CurrentNode.SetRoom(SearchStart, FloorsInterval))
				{
					UE_LOG(LogTemp, Warning, TEXT("Couldn't set room for node"));
				}
						
						
					
			}
		}
	}

	


}

void FLevelGenBildingZoneBase::CreateLinksRooms()
{
	
	for (int k = 0; k < GraphLayers.size(); k++)
	{
		for (int i = 0; i < GraphLayers[k].Graph.size(); i++)
		{
			for (int j = 0; j < GraphLayers[k].Graph[i].size(); j++)
			{
				FLevelGraphNodeBase& CurrentNode = *GraphLayers[k].Graph[i][j];
				for (auto CurrentNodeLinkIT = CurrentNode.Links.begin(); CurrentNodeLinkIT != CurrentNode.Links.end(); CurrentNodeLinkIT++)
				{
					if (!CurrentNodeLinkIT->second)
					{
						FLevelGraphNodeBase* OutGraphNode = CurrentNodeLinkIT->first;

						if (!CurrentNode.NodeRoom || !OutGraphNode->NodeRoom) continue;

						std::pair<FJointPart*, FJointPart*> JointParts;

						if (CurrentNode.NodeRoom->FindNearestJointPair(JointParts, *OutGraphNode->NodeRoom))
						{
							std::shared_ptr<FLevelGraphLink> NewLink = CreateLinkBetvineJointParts(JointParts.first, JointParts.second);
							CurrentNodeLinkIT->second = NewLink;
							OutGraphNode->Links[&CurrentNode] = NewLink;
						}

					}
				}

			}
		}
	}


}

void FLevelGenBildingZoneBase::CreateLinksBetweenBildingZone()
{

	for (auto CurrentLinkIT = Links.begin(); CurrentLinkIT != Links.end(); CurrentLinkIT++)
	{
		if (CurrentLinkIT->second && CurrentLinkIT->second->GetState() == EBildingZoneLinkState::Create)
		{
			
			int NodsNumber = RoundNumber((float)(std::abs((CurrentLinkIT->second->LinkInterval.second - CurrentLinkIT->second->LinkInterval.first)) + 1) / (float)LevelSettings.GraphNodsFrequency, ERoundType::Up);

			for (int k = 0; k < GraphLayers.size(); k++)
			{
				for (int i = 0; i < NodsNumber; i++)
				{
					std::map<float, FPossibleLink> PossibleLinks;
					
					GetPossibreLinksBetweenBildingZone(PossibleLinks, CurrentLinkIT->first, k);
					
					for (auto CurrentPossibleLinkIT = PossibleLinks.begin(); CurrentPossibleLinkIT != PossibleLinks.end(); CurrentPossibleLinkIT++)
					{
						std::shared_ptr<FLevelGraphLink> NewLink = CreateLinkBetvineJointParts(CurrentPossibleLinkIT->second.JointParts.first, CurrentPossibleLinkIT->second.JointParts.second);
						if (NewLink)
						{
							CurrentLinkIT->second->Links.push_back(NewLink);
							break;
						}
					}
					

				}
			}

			CurrentLinkIT->second->SetState(EBildingZoneLinkState::LinksCreate);
		}
	}

	

	
}

std::shared_ptr<FLevelGraphLink> FLevelGenBildingZoneBase::CreateLinkBetvineJointParts(FJointPart* First, FJointPart* Second)
{
	try
	{
		std::shared_ptr<FLevelGraphLink> NewLink = std::make_shared<FLevelGraphLink>(*First, *Second, DataStorage, LevelSettings, LevelGenerator);

		First->SetBusy();
		Second->SetBusy();
		return NewLink;
	}
	catch (FString error)
	{
		return std::shared_ptr<FLevelGraphLink>();
		//UE_LOG(LogLevelGen, Warning, TEXT("Couldn't create link for node: %s"), *error);
	}
}

void FLevelGenBildingZoneBase::SetBaseLevelNum()
{
	
		
	for (int x = GetBeginCoordinate().X; x < GetBeginCoordinate().X + GetSize().X; x++)
	{
		for (int y = GetBeginCoordinate().Y; y < GetBeginCoordinate().Y + GetSize().Y; y++)
		{
			DataStorage.LevelMap.GetCell(x, y)->CellInst->SetBaseLevelNum();
		}
	}

	SmoothBildingZoneRoofs();

	for (int x = GetBeginCoordinate().X; x < GetBeginCoordinate().X + GetSize().X; x++)
	{
		for (int y = GetBeginCoordinate().Y; y < GetBeginCoordinate().Y + GetSize().Y; y++)
		{
			if (DataStorage.LevelMap.GetCell(x, y)->LowerBaseLevelNum == -1)
			{
					
				DataStorage.LevelMap.GetCell(x, y)->LowerBaseLevelNum = DataStorage.LevelMap.GetCell(x, y)->UpperBaseLevelNum;

					
			}

		}
	}
		
		
	CreateFloorInstance();

	CreateFloorInstanceForEncirclingCells();

	
}

void FLevelGenBildingZoneBase::SmoothBildingZoneRoofs()
{
	
	SetNumForCollomX();
	SetNumForCollomY();
	SetNumForCollomX();
	SetNumForCollomY();
	
}

void FLevelGenBildingZoneBase::SetNumForCollomX()
{
	std::vector<std::vector<std::pair<int, int>>> Shears;
	std::vector<std::vector<bool>> ShearSet;
	
	Shears.resize(LevelSettings.FloorNum);
	ShearSet.resize(LevelSettings.FloorNum);
	
	for (int k = 0; k < LevelSettings.FloorNum; k++)
	{
		Shears[k].resize(GetSize().X, std::pair<int, int>(0, 0));
		ShearSet[k].resize(GetSize().X, false);
	}

	for (int x = 0; x < GetSize().X; x++)
	{
		for (int y = GetBeginCoordinate().Y; y < GetBeginCoordinate().Y + GetSize().Y; y++)
		{
			for (int k = 0; k < LevelSettings.FloorNum; k++)
			{
				FLevelCellData& CurrentCell = *DataStorage.LevelMap.GetCell(x + GetBeginCoordinate().X, y);

				if (CurrentCell.CellInst->CanBaseLevelNumUseForNearestCell() && CurrentCell.UpperBaseLevelNum == k)
				{
					for (int i = 0; i <= k; i++)
					{
						if (!ShearSet[i][x])
						{
							Shears[i][x].first = y;
							Shears[i][x].second = y;
							ShearSet[i][x] = true;
						}
						else
						{
							Shears[i][x].second = y;
						}
					}

				}
			}

		}
	}





	for (int x = GetBeginCoordinate().X; x < GetBeginCoordinate().X + GetSize().X; x++)
	{
		for (int y = GetBeginCoordinate().Y; y < GetBeginCoordinate().Y + GetSize().Y; y++)
		{
			bool flag = false;
			int XNum = 0;

			for (int i = LevelSettings.FloorNum - 1; i >= 0; i--)
			{
				if (ShearSet[i][x - GetBeginCoordinate().X] && y >= Shears[i][x - GetBeginCoordinate().X].first && y <= Shears[i][x - GetBeginCoordinate().X].second)
				{
					XNum = i;
					flag = true;
					break;
				}
			}


			if (flag) SetUpperBaseLevelNum(XNum, x, y);

		}
	}
}

void FLevelGenBildingZoneBase::SetNumForCollomY()
{
	std::vector<std::vector<std::pair<int, int>>> Shears;
	std::vector<std::vector<bool>> ShearSet;

	Shears.resize(LevelSettings.FloorNum);
	ShearSet.resize(LevelSettings.FloorNum);

	for (int k = 0; k < LevelSettings.FloorNum; k++)
	{
		Shears[k].resize(GetSize().Y, std::pair<int, int>(0, 0));
		ShearSet[k].resize(GetSize().Y, false);
	}

	for (int y = 0; y < GetSize().Y; y++)
	{
		for (int x = GetBeginCoordinate().X; x < GetBeginCoordinate().X + GetSize().X; x++)
		{
			for (int k = 0; k < LevelSettings.FloorNum; k++)
			{
				FLevelCellData& CurrentCell = *DataStorage.LevelMap.GetCell(x, y + GetBeginCoordinate().Y);

				if (CurrentCell.CellInst->CanBaseLevelNumUseForNearestCell() && CurrentCell.UpperBaseLevelNum == k)
				{
					for (int i = 0; i <= k; i++)
					{
						if (!ShearSet[i][y])
						{
							Shears[i][y].first = x;
							Shears[i][y].second = x;
							ShearSet[i][y] = true;
						}
						else
						{
							Shears[i][y].second = x;
						}
					}

				}
			}

		}
	}



	for (int x = GetBeginCoordinate().X; x < GetBeginCoordinate().X + GetSize().X; x++)
	{
		for (int y = GetBeginCoordinate().Y; y < GetBeginCoordinate().Y + GetSize().Y; y++)
		{
			bool flag = false;
			int YNum = 0;

			for (int i = LevelSettings.FloorNum - 1; i >= 0; i--)
			{
				if (ShearSet[i][y - GetBeginCoordinate().Y] && x >= Shears[i][y - GetBeginCoordinate().Y].first && x <= Shears[i][y - GetBeginCoordinate().Y].second)
				{
					YNum = i;
					flag = true;
					break;
				}
			}

			if (flag) SetUpperBaseLevelNum(YNum, x, y);

		}
	}

}

void FLevelGenBildingZoneBase::SetUpperBaseLevelNum(int NewBaseLevelNum, int X, int Y)
{
	FLevelCellData& CurrentCell = *DataStorage.LevelMap.GetCell(X, Y);
	if (CurrentCell.UpperBaseLevelNum < NewBaseLevelNum && CurrentCell.CellInst->CanBaseLevelNumUseForNearestCell())
	{
		if (!(NewBaseLevelNum > 0 && DataStorage.LevelMap.GetCell(X, Y)->Floors[NewBaseLevelNum - 1].PlasedRoom && DataStorage.LevelMap.GetCell(X, Y)->Floors[NewBaseLevelNum].PlasedRoom &&
			DataStorage.LevelMap.GetCell(X, Y)->Floors[NewBaseLevelNum - 1].PlasedRoom == DataStorage.LevelMap.GetCell(X, Y)->Floors[NewBaseLevelNum].PlasedRoom))
		{
			DataStorage.LevelMap.GetCell(X, Y)->UpperBaseLevelNum = NewBaseLevelNum;
		}
		
	}
}


void FLevelGenBildingZoneBase::CreateFloorInstance()
{
	for (int i = GetBeginCoordinate().X; i < GetBeginCoordinate().X + GetSize().X; i++)
	{
		for (int j = GetBeginCoordinate().Y; j < GetBeginCoordinate().Y + GetSize().Y; j++)
		{
			CreateFloorInstanceForCurrentCell(i, j);
		}
	}
}

void FLevelGenBildingZoneBase::CreateFloorInstanceForCurrentCell(int X, int Y)
{
	for (int k = 0; k < DataStorage.LevelMap.GetCell(X, Y)->Floors.size(); k++)
	{

		if (k < DataStorage.LevelMap.GetCell(X, Y)->LowerBaseLevelNum)
		{
			if (!DataStorage.LevelMap.GetCell(X, Y)->Floors[k].FloorInst)
			{
				if (DataStorage.LevelMap.GetCell(X, Y)->Floors[k].PlasedRoom)
				{
					DataStorage.LevelMap.GetCell(X, Y)->Floors[k].FloorInst = std::shared_ptr<FLevelFloorBase>(ELevelFloorTypeCreate(ELevelFloorType::UnderGroundRoom, DataStorage.LevelMap.GetCell(X, Y)->Floors[k]));

				}
				else
				{
					DataStorage.LevelMap.GetCell(X, Y)->Floors[k].FloorInst = std::shared_ptr<FLevelFloorBase>(ELevelFloorTypeCreate(ELevelFloorType::LowerVoid, DataStorage.LevelMap.GetCell(X, Y)->Floors[k]));

				}
			}

		}
		else if (k < DataStorage.LevelMap.GetCell(X, Y)->UpperBaseLevelNum)
		{
			if (!DataStorage.LevelMap.GetCell(X, Y)->Floors[k].FloorInst)
			{
				if (DataStorage.LevelMap.GetCell(X, Y)->Floors[k].PlasedRoom)
				{
					DataStorage.LevelMap.GetCell(X, Y)->Floors[k].FloorInst = std::shared_ptr<FLevelFloorBase>(ELevelFloorTypeCreate(ELevelFloorType::UnderGroundRoom, DataStorage.LevelMap.GetCell(X, Y)->Floors[k]));

				}
				else
				{
					DataStorage.LevelMap.GetCell(X, Y)->Floors[k].FloorInst = std::shared_ptr<FLevelFloorBase>(ELevelFloorTypeCreate(ELevelFloorType::Ground, DataStorage.LevelMap.GetCell(X, Y)->Floors[k]));

				}
			}
		}
		else
		{
			if (!DataStorage.LevelMap.GetCell(X, Y)->Floors[k].FloorInst)
			{
				if (DataStorage.LevelMap.GetCell(X, Y)->Floors[k].PlasedRoom)
				{
					DataStorage.LevelMap.GetCell(X, Y)->Floors[k].FloorInst = std::shared_ptr<FLevelFloorBase>(ELevelFloorTypeCreate(ELevelFloorType::Room, DataStorage.LevelMap.GetCell(X, Y)->Floors[k]));
				}
				else
				{
					DataStorage.LevelMap.GetCell(X, Y)->Floors[k].FloorInst = std::shared_ptr<FLevelFloorBase>(ELevelFloorTypeCreate(ELevelFloorType::Void, DataStorage.LevelMap.GetCell(X, Y)->Floors[k]));
				}
			}
		}


	}
}

void FLevelGenBildingZoneBase::CreateFloorInstanceForEncirclingCells()
{
	for (int i = 0; i < (int)EDirection::end; i++)
	{
		std::vector<FVector2D> WallCellsCoordinate;

		GetFasedeWallCellsCoordinats(WallCellsCoordinate, (EDirection)i);

		for (auto CurrentCoordinate = WallCellsCoordinate.begin(); CurrentCoordinate != WallCellsCoordinate.end(); CurrentCoordinate++)
		{

			if (DataStorage.LevelMap.IsCellExist(*CurrentCoordinate))
			{
				FLevelCellData& CurrentCell = *DataStorage.LevelMap.GetCell(*CurrentCoordinate);
				if (CurrentCell.CellInst)
				{
					CurrentCell.CellInst->SetBaseLevelNum();
				}
				else
				{
					CurrentCell.UpperBaseLevelNum = 0;
					CurrentCell.LowerBaseLevelNum = 0;
				}
				CreateFloorInstanceForCurrentCell(CurrentCoordinate->X, CurrentCoordinate->Y);
			}

		}
	}

}

void FLevelGenBildingZoneBase::CreateBaseWalls()
{
	
	for (int x = GetBeginCoordinate().X; x < GetBeginCoordinate().X + GetSize().X; x++)
	{
		for (int y = GetBeginCoordinate().Y; y < GetBeginCoordinate().Y + GetSize().Y; y++)
		{
			FLevelCellData& CurrentCell = *DataStorage.LevelMap.GetCell(x, y);

			if (CurrentCell.CellInst)
			{
				CreateRoomWalls(CurrentCell);

				CreateFloors(CurrentCell);

				CreateRoofs(CurrentCell);
			}


		}
	}

	CreateRoadWalls();

	for (int x = GetBeginCoordinate().X; x < GetBeginCoordinate().X + GetSize().X; x++)
	{
		for (int y = GetBeginCoordinate().Y; y < GetBeginCoordinate().Y + GetSize().Y; y++)
		{
			FLevelCellData& CurrentCell = *DataStorage.LevelMap.GetCell(x, y);

			if (CurrentCell.CellInst)
			{
				for (int l = 0; l < (int)EDirection::end; l++)
				{
					SetWallCornersForDirection(CurrentCell, (EDirection)l);

					CreateWallSegmentsForDirection(CurrentCell, (EDirection)l);
				}

				SetFlooreCorners(CurrentCell);

				CreateFloorSegments(CurrentCell);

			}

		}
	}

	//RoadWalls
	for (int l = 0; l < (int)EDirection::end; l++)
	{
		std::vector<FVector2D> WallCellsCoordinate;

		GetFasedeWallCellsCoordinats(WallCellsCoordinate, (EDirection)l);

		for (auto CurrentCoordinate : WallCellsCoordinate)
		{
			if (DataStorage.LevelMap.IsCellExist(CurrentCoordinate))
			{
				FLevelCellData& CurrentCell = *DataStorage.LevelMap.GetCell(CurrentCoordinate);

				SetWallCornersForDirection(CurrentCell, (EDirection)l);
				CreateWallSegmentsForDirection(CurrentCell, (EDirection)l);

			}
		}
	}

		
}

void FLevelGenBildingZoneBase::CreateRoomWalls(FLevelCellData& CurrentCell)
{
	
	for (int l = 0; l < (int)EDirection::end; l++)
	{
		FVector2D OtherCellCoordinate = CurrentCell.CellCoordinate + FVector2D(EDirectionGetVector((EDirection)l));

		if (!DataStorage.LevelMap.IsCellExist(OtherCellCoordinate) || !DataStorage.LevelMap.GetCell(OtherCellCoordinate)->CellInst) return;

		FLevelCellData* OtherCell = DataStorage.LevelMap.GetCell(OtherCellCoordinate).get();

		for (int k = 0; k < CurrentCell.Floors.size(); k++)
		{
			if (CurrentCell.Floors[k].Walls.find((EDirection)l) == CurrentCell.Floors[k].Walls.end())
			{
				

				if (CurrentCell.CellInst->GetCellType() != ELevelCellType::Tower && CurrentCell.Floors[k].FloorInst->GetType() != ELevelFloorType::UnderGroundRoom
					&& OtherCell->CellInst->GetCellType() == ELevelCellType::Tower)
				{
					if (CurrentCell.Floors[k].FloorInst->NeedCreateWall(&OtherCell->Floors[k], EWallType::OuterWall) ||
						(CurrentCell.Floors[k].PlasedRoom && CurrentCell.Floors[k].PlasedRoom->IsNeedCreateWall(FVector(OtherCellCoordinate, k))) ||
						(CurrentCell.Floors[k].FloorInst->NeedCreateWallFromRoom() && OtherCell->Floors[k].PlasedRoom &&
							OtherCell->Floors[k].PlasedRoom->IsNeedCreateWall(FVector(CurrentCell.CellCoordinate, k))))
					{
						CurrentCell.Floors[k].Walls.insert(std::make_pair((EDirection)l, std::shared_ptr<FCellWall>(new FCellWall(EWallType::OuterWall))));
					}
					
				}
				else if (CurrentCell.CellInst->GetCellType() == ELevelCellType::ThroughCell)
				{
					if (CurrentCell.Floors[k].FloorInst->NeedCreateWall(&OtherCell->Floors[k], EWallType::RoadWall) ||
						(CurrentCell.Floors[k].PlasedRoom && CurrentCell.Floors[k].PlasedRoom->IsNeedCreateWall(FVector(OtherCellCoordinate, k))) ||
						(CurrentCell.Floors[k].FloorInst->NeedCreateWallFromRoom() && OtherCell->Floors[k].PlasedRoom &&
							OtherCell->Floors[k].PlasedRoom->IsNeedCreateWall(FVector(CurrentCell.CellCoordinate, k))))
					{
						CurrentCell.Floors[k].Walls.insert(std::make_pair((EDirection)l, std::shared_ptr<FCellWall>(new FCellWall(EWallType::RoadWall))));
					}
				}
				else
				{
					if (CurrentCell.Floors[k].FloorInst->NeedCreateWall(&OtherCell->Floors[k], EWallType::InternalWall) ||
						(CurrentCell.Floors[k].PlasedRoom && CurrentCell.Floors[k].PlasedRoom->IsNeedCreateWall(FVector(OtherCellCoordinate, k))) ||
						(CurrentCell.Floors[k].FloorInst->NeedCreateWallFromRoom() && OtherCell->Floors[k].PlasedRoom &&
							OtherCell->Floors[k].PlasedRoom->IsNeedCreateWall(FVector(CurrentCell.CellCoordinate, k))))
					{
						CurrentCell.Floors[k].Walls.insert(std::make_pair((EDirection)l, std::shared_ptr<FCellWall>(new FCellWall(EWallType::InternalWall))));
					}
				}


			}
		}
	}

}

void FLevelGenBildingZoneBase::CreateFloors(FLevelCellData& CurrentCell)
{

	if (CurrentCell.CellInst->NeedCreateFloors())
	{

		for (int k = 0; k < CurrentCell.Floors.size(); k++)
		{
			if (!CurrentCell.Floors[k].FloorWalls[(int)EFloorName::Floore])
			{
				if (k == 0)
				{
					if (CurrentCell.Floors[0].FloorInst->NeedCreateFloorOnZeroFloor())
					{
						CurrentCell.Floors[0].FloorWalls[(int)EFloorName::Floore] = std::shared_ptr<FCellFlloor>(new FCellFlloor());

						if (CurrentCell.CellInst->GetCellType() != ELevelCellType::Tower)
						{
							CurrentCell.ZeroFlooreDown = std::shared_ptr<FCellFlloor>(new FCellFlloor());
						}
					}

				}
				else
				{
					
						if (CurrentCell.Floors[k].FloorInst->NeedCreateFloor(&CurrentCell.Floors[k - 1]))
						{
							CurrentCell.Floors[k].FloorWalls[(int)EFloorName::Floore] = std::shared_ptr<FCellFlloor>(new FCellFlloor());

						}
					
					

				}

			}

		}
	}


}

void FLevelGenBildingZoneBase::CreateRoofs(FLevelCellData& CurrentCell)
{

	if (CurrentCell.CellInst->NeedCreateFloors())
	{

		for (int k = 0; k < CurrentCell.Floors.size(); k++)
		{

			if (!CurrentCell.Floors[k].FloorWalls[(int)EFloorName::Roof])
			{



				if (k == CurrentCell.Floors.size() - 1)
				{
					if (CurrentCell.Floors[CurrentCell.Floors.size() - 1].FloorInst->NeedCreateRoofOnLastFloor())
					{
						CurrentCell.Floors[CurrentCell.Floors.size() - 1].FloorWalls[(int)EFloorName::Roof] = std::shared_ptr<FCellFlloor>(new FCellFlloor());

						if (CurrentCell.CellInst->GetCellType() != ELevelCellType::Tower)
						{
							CurrentCell.LastRoofUp = std::shared_ptr<FCellFlloor>(new FCellFlloor());
						}

					}

				}
				else
				{
					
					if (CurrentCell.Floors[k].FloorInst->NeedCreateFloor(&CurrentCell.Floors[k + 1]))
					{
						CurrentCell.Floors[k].FloorWalls[(int)EFloorName::Roof] = std::shared_ptr<FCellFlloor>(new FCellFlloor());

					}
					

				}

			}
		}
	}

}

void FLevelGenBildingZoneBase::CreateRoadWalls()
{

	for (int i = 0; i < (int)EDirection::end; i++)
	{
		std::vector<FVector2D> WallCellsCoordinate;

		GetFasedeWallCellsCoordinats(WallCellsCoordinate, (EDirection)i);
		for (auto CurrentCoordinate = WallCellsCoordinate.begin(); CurrentCoordinate != WallCellsCoordinate.end(); CurrentCoordinate++)
		{
			if (DataStorage.LevelMap.IsCellExist(*CurrentCoordinate))
			{
				FLevelCellData& CurrentCell = *DataStorage.LevelMap.GetCell(*CurrentCoordinate);
				for (int k = 0; k < CurrentCell.Floors.size(); k++)
				{

					FVector2D OtherCellCoordinate = CurrentCell.CellCoordinate + FVector2D(EDirectionGetVector((EDirection)i));

					if (!DataStorage.LevelMap.IsCellExist(OtherCellCoordinate) || !DataStorage.LevelMap.GetCell(OtherCellCoordinate)->CellInst) return;

					FLevelCellData* OtherCell = DataStorage.LevelMap.GetCell(OtherCellCoordinate.X, OtherCellCoordinate.Y).get();
					if (!OtherCell->CellInst) return;


					if (OtherCell->CellInst->NeedCreateRoadWall())
					{
						if (CurrentCell.Floors[k].FloorInst->NeedCreateWall(&OtherCell->Floors[k], EWallType::RoadWall) ||
							(CurrentCell.Floors[k].PlasedRoom && CurrentCell.Floors[k].PlasedRoom->IsNeedCreateWall(FVector(OtherCellCoordinate, k))) ||
							(CurrentCell.Floors[k].FloorInst->NeedCreateWallFromRoom() && OtherCell->Floors[k].PlasedRoom &&
								OtherCell->Floors[k].PlasedRoom->IsNeedCreateWall(FVector(CurrentCell.CellCoordinate, k))))
						{
							CurrentCell.Floors[k].Walls.insert(std::make_pair((EDirection)i, std::shared_ptr<FCellWall>(new FCellWall(EWallType::RoadWall))));
						}

					}
				}

			}
		}
	}
}

void FLevelGenBildingZoneBase::SetWallCornersForDirection(FLevelCellData& CurrentCell, EDirection Direction)
{
	for (int k = 0; k < CurrentCell.Floors.size(); k++)
	{

		if (CurrentCell.Floors[k].Walls.find(Direction) != CurrentCell.Floors[k].Walls.end())
		{
			FCellWall& CurrentWall = *CurrentCell.Floors[k].Walls.find(Direction)->second;

			SetCornersForCurrentWall(CurrentWall.Right, CurrentCell, k, Direction, EDirectionGetRight(Direction));


			//Roof
			if (CurrentCell.Floors[k].FloorWalls[(int)EFloorName::Roof])
			{
				CurrentWall.Up.Type = EWallCornerType::NegativeOffset;
			}
			else if (k != CurrentCell.Floors.size() - 1 && CurrentCell.Floors[k + 1].Walls.find(Direction) != CurrentCell.Floors[k + 1].Walls.end())
			{
				CurrentWall.Up.Type = EWallCornerType::PositiveOffset;
				CurrentWall.Up.OtherWall = CurrentCell.Floors[k + 1].Walls.find(Direction)->second.get();
			}
			else if (k == CurrentCell.Floors.size() - 1)
			{
				CurrentWall.Up.Type = EWallCornerType::PositiveOffset;
			}
			else if (DataStorage.LevelMap.IsCellExist(CurrentCell.CellCoordinate + FVector2D(EDirectionGetVector(Direction))) &&
				DataStorage.LevelMap.GetCell(CurrentCell.CellCoordinate + FVector2D(EDirectionGetVector(Direction)))->Floors[k + 1].FloorWalls[(int)EFloorName::Floore])
			{
				CurrentWall.Up.Type = EWallCornerType::Normal;
			}
			else
			{

				CurrentWall.Up.Type = EWallCornerType::NormalWithEdge;
			}



		}

	}

}

void FLevelGenBildingZoneBase::SetCornersForCurrentWall(FWallCorner& WallCorner, const FLevelCellData& CurrentCell, int Floor, EDirection Direction, EDirection WallOffsetDirection)
{

	FCellWall& CurrentWall = *CurrentCell.Floors[Floor].Walls.find(Direction)->second;

	FVector2D RightCellCoordinate = CurrentCell.CellCoordinate + FVector2D(EDirectionGetVector(WallOffsetDirection));

	FVector2D RightAndForwardCellCoordinate = CurrentCell.CellCoordinate + FVector2D(EDirectionGetVector(WallOffsetDirection)) +
		FVector2D(EDirectionGetVector(Direction));


	if (CurrentCell.Floors[Floor].Walls.find(WallOffsetDirection) != CurrentCell.Floors[Floor].Walls.end())
	{
		WallCorner.Type = EWallCornerType::NegativeOffset;
		WallCorner.OtherWall = CurrentCell.Floors[Floor].Walls.find(WallOffsetDirection)->second.get();
		return;
	}

	else if (DataStorage.LevelMap.IsCellExist(RightCellCoordinate) &&
		DataStorage.LevelMap.GetCell(RightCellCoordinate)->Floors[Floor].Walls.find(Direction) !=
		DataStorage.LevelMap.GetCell(RightCellCoordinate)->Floors[Floor].Walls.end())
	{
		WallCorner.Type = EWallCornerType::Normal;
		WallCorner.OtherWall = DataStorage.LevelMap.GetCell(RightCellCoordinate)->Floors[Floor].Walls.find(Direction)->second.get();
		return;
	}

	else if (DataStorage.LevelMap.IsCellExist(RightAndForwardCellCoordinate) &&
		DataStorage.LevelMap.GetCell(RightAndForwardCellCoordinate)->Floors[Floor].Walls.find(EDirectionInvert(WallOffsetDirection)) !=
		DataStorage.LevelMap.GetCell(RightAndForwardCellCoordinate)->Floors[Floor].Walls.end())
	{
		WallCorner.Type = EWallCornerType::PositiveOffset;
		WallCorner.OtherWall = DataStorage.LevelMap.GetCell(RightAndForwardCellCoordinate)->Floors[Floor].Walls.find(EDirectionInvert(WallOffsetDirection))->second.get();
		return;
	}

	else
	{
		FVector2D OtherWallCoordinate = CurrentCell.CellCoordinate + FVector2D(EDirectionGetVector(Direction));

		WallCorner.Type = EWallCornerType::NormalWithEdge;
		if (DataStorage.LevelMap.GetCell(OtherWallCoordinate)->Floors[Floor].Walls.find(EDirectionInvert(Direction)) != DataStorage.LevelMap.GetCell(OtherWallCoordinate)->Floors[Floor].Walls.end())
		{
			WallCorner.OtherWall = DataStorage.LevelMap.GetCell(OtherWallCoordinate)->Floors[Floor].Walls.find(EDirectionInvert(Direction))->second.get();
		}
		return;
	}
}

void FLevelGenBildingZoneBase::GetWallRotationAndOffset(const FLevelCellData& CurrentCell, FVector& WallOffset, FRotator& WallRotation, EDirection Direction) const
{

	switch ((EDirection)Direction)
	{
	case EDirection::XP:

		WallRotation = FRotator(0, 180, 0);
		WallOffset = FVector(LevelSettings.CellSize / 2 - LevelSettings.WallThickness / 2, 0, 0);
		break;

	case EDirection::XM:

		WallRotation = FRotator(0, 0, 0);
		WallOffset = FVector(-LevelSettings.CellSize / 2 + LevelSettings.WallThickness / 2, 0, 0);
		break;

	case EDirection::YP:

		WallRotation = FRotator(0, 270, 0);
		WallOffset = FVector(0, LevelSettings.CellSize / 2 - LevelSettings.WallThickness / 2, 0);
		break;

	case EDirection::YM:

		WallRotation = FRotator(0, 90, 0);
		WallOffset = FVector(0, -LevelSettings.CellSize / 2 + LevelSettings.WallThickness / 2, 0);
		break;

	default:

		throw;
	}
}



void FLevelGenBildingZoneBase::CreateWallSegmentsForDirection(FLevelCellData& CurrentCell, EDirection Direction)
{
	for (int k = 0; k < CurrentCell.Floors.size(); k++)
	{

		if (CurrentCell.Floors[k].Walls.find(Direction) != CurrentCell.Floors[k].Walls.end())
		{
			FRotator WallRotate;
			FVector WallOffset;

			GetWallRotationAndOffset(CurrentCell, WallOffset, WallRotate, Direction);

			FCellWall& CurrentWall = *CurrentCell.Floors[k].Walls.find(Direction)->second;

			FVector WallCoordinateBaseSegment = FVector(0, 0, (k + 0.5)  * LevelSettings.CellHeight - LevelSettings.WallThickness / 2) + WallOffset;

			FVector2D WallSizeBaseSegment(LevelSettings.CellSize - LevelSettings.WallThickness, LevelSettings.CellHeight - LevelSettings.WallThickness);
			const FLevelGeneratorMaterialSettings& BaseSegmentMaterial = CurrentWall.WallType == EWallType::RoadWall || CurrentWall.WallType == EWallType::OuterWall ?
				LevelSettings.BildingWallMaterial : LevelSettings.WallsMaterial;


			//Main segment............................................

			CurrentWall.WallSegments.insert(std::make_pair(&BaseSegmentMaterial, std::shared_ptr<FProceduralFigureRectangle>(new
				FProceduralFigureRectangle(WallCoordinateBaseSegment, WallRotate.Vector(), (WallRotate + FRotator(0, 90, 0)).Vector(), WallSizeBaseSegment, BaseSegmentMaterial))));


			//Right................................................
			switch (CurrentWall.Right.Type)
			{
			case EWallCornerType::NormalWithEdge:

				CreateEdgeSegmentsForTwoSideWall(CurrentWall, WallCoordinateBaseSegment, WallSizeBaseSegment, WallRotate, Direction, BaseSegmentMaterial);

				break;

			case EWallCornerType::Normal:
			{

				FProceduralFigureRectangle NewSegment1(WallCoordinateBaseSegment + EDirectionGetVector(EDirectionGetRight(Direction)) * LevelSettings.CellSize / 2, WallRotate.Vector(),
					(WallRotate + FRotator(0, 90, 0)).Vector(), FVector2D(LevelSettings.WallThickness, WallSizeBaseSegment.Y), BaseSegmentMaterial);

				if (CurrentWall.Up.Type == EWallCornerType::PositiveOffset || CurrentWall.Up.Type == EWallCornerType::Normal)
				{
					FProceduralFigureRectangle NewSegment2(WallCoordinateBaseSegment + FVector(0, 0, LevelSettings.CellHeight / 2) + EDirectionGetVector(EDirectionGetRight(Direction)) *
						LevelSettings.CellSize / 2, WallRotate.Vector(), (WallRotate + FRotator(0, 90, 0)).Vector(), FVector2D(LevelSettings.WallThickness, LevelSettings.WallThickness),
						BaseSegmentMaterial);

					NewSegment1.Merge(NewSegment2);
				}

				CurrentWall.WallSegments[&BaseSegmentMaterial]->Merge(NewSegment1);

			}
			break;

			case EWallCornerType::PositiveOffset:

				CreateSegmentsFor90DegresCorner(CurrentWall, WallCoordinateBaseSegment, WallSizeBaseSegment, WallRotate, Direction);

				break;
			}


			//Up..................................................
			switch (CurrentWall.Up.Type)
			{

			case EWallCornerType::Normal:
			case EWallCornerType::PositiveOffset:
			{
				FProceduralFigureRectangle NewSegment1(WallCoordinateBaseSegment + FVector(0, 0, LevelSettings.CellHeight / 2), WallRotate.Vector(), (WallRotate + FRotator(0, 90, 0)).Vector(),
					FVector2D(WallSizeBaseSegment.X, LevelSettings.WallThickness), BaseSegmentMaterial);

				CurrentWall.WallSegments[&BaseSegmentMaterial]->Merge(NewSegment1);
			}

			break;

			case EWallCornerType::NormalWithEdge:
			{
				FProceduralFigureRectangle NewSegment1(WallCoordinateBaseSegment + FVector(0, 0, LevelSettings.CellHeight / 2), WallRotate.Vector(), (WallRotate + FRotator(0, 90, 0)).Vector(),
					FVector2D(WallSizeBaseSegment.X, LevelSettings.WallThickness), BaseSegmentMaterial);

				CurrentWall.WallSegments[&BaseSegmentMaterial]->Merge(NewSegment1);

				if (Direction == EDirection::XP || Direction == EDirection::YP)
				{
					std::shared_ptr<FProceduralFigureRectangle> NewSegment2(new FProceduralFigureRectangle(WallCoordinateBaseSegment + FVector(0, 0, LevelSettings.CellHeight / 2 +
						LevelSettings.WallThickness / 2) +
						EDirectionGetVector(Direction) * LevelSettings.WallThickness / 2, FVector(0, 0, 1), EDirectionGetVector(EDirectionGetRight(Direction)),
						FVector2D(WallSizeBaseSegment.X, LevelSettings.WallThickness), LevelSettings.WallsMaterial));


					if (CurrentWall.WallSegments.find(&LevelSettings.WallsMaterial) != CurrentWall.WallSegments.end())
					{
						auto MatIt = CurrentWall.WallSegments.find(&LevelSettings.WallsMaterial);
						MatIt->second->Merge(*NewSegment2);

					}
					else
					{
						CurrentWall.WallSegments.insert(std::make_pair(&LevelSettings.WallsMaterial, NewSegment2));
					}


				}

			}

			break;

			}
		}
	}
}

void FLevelGenBildingZoneBase::CreateEdgeSegmentsForTwoSideWall(FCellWall& CurrentWall, FVector WallCoordinateBaseSegment, FVector2D WallSizeBaseSegment, FRotator WallRotate, EDirection WallDirection,
	const FLevelGeneratorMaterialSettings& BaseSegmentMaterial)
{
	if (CurrentWall.Right.OtherWall && ((CurrentWall.WallType != EWallType::InternalWall && static_cast<FCellWall*>(CurrentWall.Right.OtherWall)->WallType == EWallType::InternalWall) ||
		(CurrentWall.WallType == EWallType::InternalWall && static_cast<FCellWall*>(CurrentWall.Right.OtherWall)->WallType != EWallType::InternalWall)))
	{

		if (CurrentWall.WallType != EWallType::InternalWall)
		{
			FProceduralFigureRectangle NewSegment1(WallCoordinateBaseSegment +
				EDirectionGetVector(EDirectionGetRight(WallDirection)) * (LevelSettings.CellSize) / 2 + EDirectionGetVector(WallDirection) * LevelSettings.WallThickness / 2,
				(WallRotate + FRotator(0, -45, 0)).Vector(), (WallRotate + FRotator(0, 45, 0)).Vector(), FVector2D(LevelSettings.WallThickness * sqrt(2), WallSizeBaseSegment.Y),
				BaseSegmentMaterial);


			FProceduralFigureRectangle NewSegment2(WallCoordinateBaseSegment + FVector(0, 0, LevelSettings.CellHeight / 2) +
				EDirectionGetVector(EDirectionGetRight(WallDirection)) * (LevelSettings.CellSize) / 2 + EDirectionGetVector(WallDirection) * LevelSettings.WallThickness / 2,
				(WallRotate + FRotator(0, -45, 0)).Vector(), (WallRotate + FRotator(0, 45, 0)).Vector(), FVector2D(LevelSettings.WallThickness * sqrt(2), LevelSettings.WallThickness),
				BaseSegmentMaterial);

			CurrentWall.WallSegments[&BaseSegmentMaterial]->Merge(NewSegment1);
			CurrentWall.WallSegments[&BaseSegmentMaterial]->Merge(NewSegment2);

			std::shared_ptr<FProceduralFigureRectangle> NewSegment3(new FProceduralFigureRectangle(WallCoordinateBaseSegment +
				EDirectionGetVector(EDirectionGetRight(WallDirection)) * LevelSettings.CellSize / 2 + EDirectionGetVector(WallDirection) * LevelSettings.WallThickness,
				-WallRotate.Vector(), -(WallRotate + FRotator(0, 90, 0)).Vector(), FVector2D(LevelSettings.WallThickness, WallSizeBaseSegment.Y), BaseSegmentMaterial));

			if (CurrentWall.WallSegments.find(&LevelSettings.WallsMaterial) != CurrentWall.WallSegments.end())
			{
				CurrentWall.WallSegments[&LevelSettings.WallsMaterial]->Merge(*NewSegment3);

			}
			else
			{
				CurrentWall.WallSegments.insert(std::make_pair(&LevelSettings.WallsMaterial, NewSegment3));
			}
		}
		else
		{
			std::shared_ptr<FProceduralFigureRectangle> NewSegment1(new FProceduralFigureRectangle(WallCoordinateBaseSegment +
				EDirectionGetVector(EDirectionGetRight(WallDirection)) * (LevelSettings.CellSize) / 2 + EDirectionGetVector(WallDirection) * LevelSettings.WallThickness / 2,
				(WallRotate + FRotator(0, -135, 0)).Vector(), (WallRotate + FRotator(0, -225, 0)).Vector(), FVector2D(LevelSettings.WallThickness * sqrt(2), WallSizeBaseSegment.Y),
				LevelSettings.BildingWallMaterial));


			FProceduralFigureRectangle NewSegment2(WallCoordinateBaseSegment + FVector(0, 0, LevelSettings.CellHeight / 2) +
				EDirectionGetVector(EDirectionGetRight(WallDirection)) * (LevelSettings.CellSize) / 2 + EDirectionGetVector(WallDirection) * LevelSettings.WallThickness / 2,
				(WallRotate + FRotator(0, -135, 0)).Vector(), (WallRotate + FRotator(0, -225, 0)).Vector(), FVector2D(LevelSettings.WallThickness * sqrt(2), LevelSettings.WallThickness),
				LevelSettings.BildingWallMaterial);

			if (CurrentWall.WallSegments.find(&LevelSettings.BildingWallMaterial) != CurrentWall.WallSegments.end())
			{
				CurrentWall.WallSegments[&BaseSegmentMaterial]->Merge(*NewSegment1);
				CurrentWall.WallSegments[&BaseSegmentMaterial]->Merge(NewSegment2);
			}
			else
			{
				NewSegment1->Merge(NewSegment2);
				CurrentWall.WallSegments.insert(std::make_pair(&LevelSettings.BildingWallMaterial, NewSegment1));
			}

			FProceduralFigureRectangle NewSegment3(WallCoordinateBaseSegment + EDirectionGetVector(EDirectionGetRight(WallDirection)) * LevelSettings.CellSize / 2,
				WallRotate.Vector(), (WallRotate + FRotator(0, 90, 0)).Vector(), FVector2D(LevelSettings.WallThickness, WallSizeBaseSegment.Y), BaseSegmentMaterial);

			CurrentWall.WallSegments[&BaseSegmentMaterial]->Merge(NewSegment3);
		}
	}
	else
	{
		std::shared_ptr<FProceduralFigureRectangle> NewSegment1(new FProceduralFigureRectangle(WallCoordinateBaseSegment + EDirectionGetVector(EDirectionGetRight(WallDirection)) *
			(LevelSettings.CellSize - LevelSettings.WallThickness) / 2 + EDirectionGetVector(WallDirection) * LevelSettings.WallThickness / 2,
			(WallRotate + FRotator(0, -90, 0)).Vector(), WallRotate.Vector(), FVector2D(LevelSettings.WallThickness, WallSizeBaseSegment.Y), LevelSettings.WallsMaterial));

		FProceduralFigureRectangle NewSegment2(WallCoordinateBaseSegment + FVector(0, 0, LevelSettings.CellHeight / 2)
			+ EDirectionGetVector(EDirectionGetRight(WallDirection)) *
			(LevelSettings.CellSize - LevelSettings.WallThickness) / 2 + EDirectionGetVector(WallDirection) * LevelSettings.WallThickness / 2,
			(WallRotate + FRotator(0, -90, 0)).Vector(), WallRotate.Vector(), FVector2D(LevelSettings.WallThickness, LevelSettings.WallThickness), LevelSettings.WallsMaterial);



		if (CurrentWall.WallSegments.find(&LevelSettings.WallsMaterial) != CurrentWall.WallSegments.end())
		{
			CurrentWall.WallSegments[&LevelSettings.WallsMaterial]->Merge(*NewSegment1);
			CurrentWall.WallSegments[&LevelSettings.WallsMaterial]->Merge(NewSegment2);
		}
		else
		{
			NewSegment1->Merge(NewSegment2);
			CurrentWall.WallSegments.insert(std::make_pair(&LevelSettings.WallsMaterial, NewSegment1));
		}
	}
}

void FLevelGenBildingZoneBase::CreateSegmentsFor90DegresCorner(FCellWall& CurrentWall, FVector WallCoordinateBaseSegment, FVector2D WallSizeBaseSegment, FRotator WallRotate, EDirection WallDirection)
{
	const FLevelGeneratorMaterialSettings* SegmentMaterial;

	if (CurrentWall.Right.OtherWall && (CurrentWall.WallType != EWallType::InternalWall || static_cast<FCellWall*>(CurrentWall.Right.OtherWall)->WallType != EWallType::InternalWall))
	{
		SegmentMaterial = &LevelSettings.BildingWallMaterial;
	}
	else
	{
		SegmentMaterial = &LevelSettings.WallsMaterial;
	}


	std::shared_ptr<FProceduralFigureRectangle> NewSegment1(new FProceduralFigureRectangle(WallCoordinateBaseSegment +
		EDirectionGetVector(EDirectionGetRight(WallDirection)) * (LevelSettings.CellSize) / 2 + EDirectionGetVector(WallDirection) * LevelSettings.WallThickness / 2,
		(WallRotate + FRotator(0, -45, 0)).Vector(), (WallRotate + FRotator(0, 45, 0)).Vector(), FVector2D(LevelSettings.WallThickness * sqrt(2), WallSizeBaseSegment.Y),
		*SegmentMaterial));


	FProceduralFigureRectangle NewSegment2(WallCoordinateBaseSegment + FVector(0, 0, LevelSettings.CellHeight / 2) +
		EDirectionGetVector(EDirectionGetRight(WallDirection)) * (LevelSettings.CellSize) / 2 + EDirectionGetVector(WallDirection) * LevelSettings.WallThickness / 2,
		(WallRotate + FRotator(0, -45, 0)).Vector(), (WallRotate + FRotator(0, 45, 0)).Vector(), FVector2D(LevelSettings.WallThickness * sqrt(2), LevelSettings.WallThickness),
		*SegmentMaterial);


	if (CurrentWall.WallSegments.find(SegmentMaterial) != CurrentWall.WallSegments.end())
	{
		CurrentWall.WallSegments[SegmentMaterial]->Merge(*NewSegment1);
		CurrentWall.WallSegments[SegmentMaterial]->Merge(NewSegment2);

	}
	else
	{
		CurrentWall.WallSegments.insert(std::make_pair(SegmentMaterial, NewSegment1));
		CurrentWall.WallSegments[SegmentMaterial]->Merge(NewSegment2);
	}
}

void FLevelGenBildingZoneBase::SetFlooreCorners(FLevelCellData& CurrentCell)
{

	for (int k = 0; k < CurrentCell.Floors.size(); k++)
	{
		if (CurrentCell.Floors[k].FloorWalls[(int)EFloorName::Floore])
		{
			CurrentCell.Floors[k].FloorWalls[(int)EFloorName::Floore]->FloorJoints[(int)EDirection::XP] = SetFlooreCornersForCurrentDirection(CurrentCell, k, EDirection::XP, EFloorName::Floore);

			CurrentCell.Floors[k].FloorWalls[(int)EFloorName::Floore]->FloorJoints[(int)EDirection::YP] = SetFlooreCornersForCurrentDirection(CurrentCell, k, EDirection::YP, EFloorName::Floore);

			CurrentCell.Floors[k].FloorWalls[(int)EFloorName::Floore]->FloorJoints[(int)EDirection::XM] = SetFlooreCornersForCurrentDirection(CurrentCell, k, EDirection::XM, EFloorName::Floore);

			CurrentCell.Floors[k].FloorWalls[(int)EFloorName::Floore]->FloorJoints[(int)EDirection::YM] = SetFlooreCornersForCurrentDirection(CurrentCell, k, EDirection::YM, EFloorName::Floore);

		}
		if (CurrentCell.Floors[k].FloorWalls[(int)EFloorName::Roof])
		{
			CurrentCell.Floors[k].FloorWalls[(int)EFloorName::Roof]->FloorJoints[(int)EDirection::XP] = SetFlooreCornersForCurrentDirection(CurrentCell, k, EDirection::XP, EFloorName::Roof);

			CurrentCell.Floors[k].FloorWalls[(int)EFloorName::Roof]->FloorJoints[(int)EDirection::YP] = SetFlooreCornersForCurrentDirection(CurrentCell, k, EDirection::YP, EFloorName::Roof);

			CurrentCell.Floors[k].FloorWalls[(int)EFloorName::Roof]->FloorJoints[(int)EDirection::XM] = SetFlooreCornersForCurrentDirection(CurrentCell, k, EDirection::XM, EFloorName::Roof);

			CurrentCell.Floors[k].FloorWalls[(int)EFloorName::Roof]->FloorJoints[(int)EDirection::YM] = SetFlooreCornersForCurrentDirection(CurrentCell, k, EDirection::YM, EFloorName::Roof);
		}

	}

	if (CurrentCell.LastRoofUp)
	{
		CurrentCell.LastRoofUp->FloorJoints[(int)EDirection::XP] = SetFlooreCornersForCurrentDirectionForLastRoof(CurrentCell, EDirection::XP);
		CurrentCell.LastRoofUp->FloorJoints[(int)EDirection::YP] = SetFlooreCornersForCurrentDirectionForLastRoof(CurrentCell, EDirection::YP);
		CurrentCell.LastRoofUp->FloorJoints[(int)EDirection::XM] = SetFlooreCornersForCurrentDirectionForLastRoof(CurrentCell, EDirection::XM);
		CurrentCell.LastRoofUp->FloorJoints[(int)EDirection::YM] = SetFlooreCornersForCurrentDirectionForLastRoof(CurrentCell, EDirection::YM);
	}
	if (CurrentCell.ZeroFlooreDown)
	{
		CurrentCell.ZeroFlooreDown->FloorJoints[(int)EDirection::XP] = SetFlooreCornersForCurrentDirectionForZeroFlooreDown(CurrentCell, EDirection::XP);
		CurrentCell.ZeroFlooreDown->FloorJoints[(int)EDirection::YP] = SetFlooreCornersForCurrentDirectionForZeroFlooreDown(CurrentCell, EDirection::YP);
		CurrentCell.ZeroFlooreDown->FloorJoints[(int)EDirection::XM] = SetFlooreCornersForCurrentDirectionForZeroFlooreDown(CurrentCell, EDirection::XM);
		CurrentCell.ZeroFlooreDown->FloorJoints[(int)EDirection::YM] = SetFlooreCornersForCurrentDirectionForZeroFlooreDown(CurrentCell, EDirection::YM);
	}

}

FWallCorner FLevelGenBildingZoneBase::SetFlooreCornersForCurrentDirectionForZeroFlooreDown(FLevelCellData& CurrentCell, EDirection Direction)
{

	if (DataStorage.LevelMap.IsCellExist(CurrentCell.CellCoordinate + FVector2D(EDirectionGetVector(Direction))) &&
		DataStorage.LevelMap.GetCell(CurrentCell.CellCoordinate + FVector2D(EDirectionGetVector(Direction)))->ZeroFlooreDown)
	{
		return  FWallCorner(EWallCornerType::PositiveOffset, DataStorage.LevelMap.GetCell(CurrentCell.CellCoordinate + FVector2D(EDirectionGetVector(Direction)))->ZeroFlooreDown.get());
	}


	else if (DataStorage.LevelMap.IsCellExist(CurrentCell.CellCoordinate + FVector2D(EDirectionGetVector(Direction))) &&
		DataStorage.LevelMap.GetCell(CurrentCell.CellCoordinate + FVector2D(EDirectionGetVector(Direction)))->Floors[0].Walls.find(EDirectionInvert(Direction)) !=
		DataStorage.LevelMap.GetCell(CurrentCell.CellCoordinate + FVector2D(EDirectionGetVector(Direction)))->Floors[0].Walls.end())
	{
		return FWallCorner(EWallCornerType::Normal, DataStorage.LevelMap.GetCell(CurrentCell.CellCoordinate + FVector2D(EDirectionGetVector(Direction)))->Floors[0].Walls.find(EDirectionInvert(Direction))->second.get());
	}
	else
	{
		return FWallCorner(EWallCornerType::NormalWithEdge, nullptr);
	}
}


FWallCorner FLevelGenBildingZoneBase::SetFlooreCornersForCurrentDirectionForLastRoof(FLevelCellData& CurrentCell, EDirection Direction)
{
	if (DataStorage.LevelMap.IsCellExist(CurrentCell.CellCoordinate + FVector2D(EDirectionGetVector(Direction))) &&
		DataStorage.LevelMap.GetCell(CurrentCell.CellCoordinate + FVector2D(EDirectionGetVector(Direction)))->LastRoofUp)
	{
		return  FWallCorner(EWallCornerType::PositiveOffset, DataStorage.LevelMap.GetCell(CurrentCell.CellCoordinate + FVector2D(EDirectionGetVector(Direction)))->LastRoofUp.get());
	}
	else if (DataStorage.LevelMap.IsCellExist(CurrentCell.CellCoordinate + FVector2D(EDirectionGetVector(Direction))) &&
		DataStorage.LevelMap.GetCell(CurrentCell.CellCoordinate + FVector2D(EDirectionGetVector(Direction)))->Floors[LevelSettings.FloorNum - 1].Walls.find(EDirectionInvert(Direction)) !=
		DataStorage.LevelMap.GetCell(CurrentCell.CellCoordinate + FVector2D(EDirectionGetVector(Direction)))->Floors[LevelSettings.FloorNum - 1].Walls.end())
	{
		return FWallCorner(EWallCornerType::Normal, DataStorage.LevelMap.GetCell(CurrentCell.CellCoordinate + FVector2D(EDirectionGetVector(Direction)))->Floors[LevelSettings.FloorNum - 1].Walls.find(EDirectionInvert(Direction))->second.get());
	}
	else
	{
		return FWallCorner(EWallCornerType::NormalWithEdge, nullptr);
	}
}

FWallCorner FLevelGenBildingZoneBase::SetFlooreCornersForCurrentDirection(FLevelCellData& CurrentCell, int Floor, EDirection Direction, EFloorName FloorName)
{
	if (CurrentCell.Floors[Floor].Walls.find(Direction) != CurrentCell.Floors[Floor].Walls.end())
	{
		return FWallCorner(EWallCornerType::NegativeOffset, nullptr);
	}
	else if (DataStorage.LevelMap.IsCellExist(CurrentCell.CellCoordinate + FVector2D(EDirectionGetVector(Direction))) &&
		DataStorage.LevelMap.GetCell(CurrentCell.CellCoordinate + FVector2D(EDirectionGetVector(Direction)))->Floors[Floor].FloorWalls[(int)FloorName])
	{
		return  FWallCorner(EWallCornerType::PositiveOffset, DataStorage.LevelMap.GetCell(CurrentCell.CellCoordinate + FVector2D(EDirectionGetVector(Direction)))->Floors[Floor].FloorWalls[(int)FloorName].get());
	}

	else if (FloorName == EFloorName::Floore && Floor > 0 && DataStorage.LevelMap.IsCellExist(CurrentCell.CellCoordinate + FVector2D(EDirectionGetVector(Direction))) &&
		DataStorage.LevelMap.GetCell(CurrentCell.CellCoordinate + FVector2D(EDirectionGetVector(Direction)))->Floors[Floor - 1].Walls.find(EDirectionInvert(Direction)) !=
		DataStorage.LevelMap.GetCell(CurrentCell.CellCoordinate + FVector2D(EDirectionGetVector(Direction)))->Floors[Floor - 1].Walls.end())
	{
		return FWallCorner(EWallCornerType::Normal, DataStorage.LevelMap.GetCell(CurrentCell.CellCoordinate + FVector2D(EDirectionGetVector(Direction)))->Floors[Floor - 1].Walls.find(EDirectionInvert(Direction))->second.get());
	}
	else if (FloorName == EFloorName::Roof && Floor < CurrentCell.Floors.size() - 1 && DataStorage.LevelMap.IsCellExist(CurrentCell.CellCoordinate + FVector2D(EDirectionGetVector(Direction))) &&
		DataStorage.LevelMap.GetCell(CurrentCell.CellCoordinate + FVector2D(EDirectionGetVector(Direction)))->Floors[Floor + 1].Walls.find(EDirectionInvert(Direction)) !=
		DataStorage.LevelMap.GetCell(CurrentCell.CellCoordinate + FVector2D(EDirectionGetVector(Direction)))->Floors[Floor + 1].Walls.end())
	{
		return FWallCorner(EWallCornerType::Normal, DataStorage.LevelMap.GetCell(CurrentCell.CellCoordinate + FVector2D(EDirectionGetVector(Direction)))->Floors[Floor + 1].Walls.find(EDirectionInvert(Direction))->second.get());
	}
	else
	{
		return FWallCorner(EWallCornerType::NormalWithEdge, nullptr);
	}
}

void FLevelGenBildingZoneBase::CreateFloorSegments(FLevelCellData& CurrentCell)
{

	for (int k = 0; k < CurrentCell.Floors.size(); k++)
	{

		if (CurrentCell.Floors[k].FloorWalls[(int)EFloorName::Floore] && CurrentCell.Floors[k].FloorWalls[(int)EFloorName::Floore]->WallSegments.size() == 0)
		{
			FVector FloorCoordinateBaseSegment = FVector(0, 0, k  * LevelSettings.CellHeight);
			FVector2D FloorSizeBaseSegment(LevelSettings.CellSize - LevelSettings.WallThickness, LevelSettings.CellSize - LevelSettings.WallThickness);
			const FLevelGeneratorMaterialSettings& FloorSegmentMaterial = LevelSettings.FloorMaterial;

			CurrentCell.Floors[k].FloorWalls[(int)EFloorName::Floore]->WallSegments.insert(std::make_pair(&FloorSegmentMaterial, std::shared_ptr<FProceduralFigureRectangle>(
				new FProceduralFigureRectangle(FloorCoordinateBaseSegment, FVector(0, 0, 1), FVector(1, 0, 0), FloorSizeBaseSegment, FloorSegmentMaterial))));



			CreateFloorSegmentsForCurrentDirection(CurrentCell, EDirection::XP, FloorCoordinateBaseSegment, FloorSizeBaseSegment, FloorSegmentMaterial, EFloorName::Floore,
				*CurrentCell.Floors[k].FloorWalls[(int)EFloorName::Floore]);

			CreateFloorSegmentsForCurrentDirection(CurrentCell, EDirection::YP, FloorCoordinateBaseSegment, FloorSizeBaseSegment, FloorSegmentMaterial, EFloorName::Floore,
				*CurrentCell.Floors[k].FloorWalls[(int)EFloorName::Floore]);

			CreateSkirting(CurrentCell, k, EDirection::XP, FloorCoordinateBaseSegment, FloorSizeBaseSegment, FloorSegmentMaterial, EFloorName::Floore);

			CreateSkirting(CurrentCell, k, EDirection::YP, FloorCoordinateBaseSegment, FloorSizeBaseSegment, FloorSegmentMaterial, EFloorName::Floore);

			CreateSkirting(CurrentCell, k, EDirection::XM, FloorCoordinateBaseSegment, FloorSizeBaseSegment, FloorSegmentMaterial, EFloorName::Floore);

			CreateSkirting(CurrentCell, k, EDirection::YM, FloorCoordinateBaseSegment, FloorSizeBaseSegment, FloorSegmentMaterial, EFloorName::Floore);

			CreateTriangelSegmentInCorner(CurrentCell, EFloorName::Floore, k, FloorCoordinateBaseSegment, FloorSegmentMaterial);
		}


		if (CurrentCell.Floors[k].FloorWalls[(int)EFloorName::Roof] && CurrentCell.Floors[k].FloorWalls[(int)EFloorName::Roof]->WallSegments.size() == 0)
		{
			FVector RoofCoordinateBaseSegment = FVector(0, 0, (k + 1)  * LevelSettings.CellHeight - LevelSettings.WallThickness);
			FVector2D RoofSizeBaseSegment(LevelSettings.CellSize - LevelSettings.WallThickness, LevelSettings.CellSize - LevelSettings.WallThickness);
			const FLevelGeneratorMaterialSettings& RoofSegmentMaterial = LevelSettings.RoofMaterial;

			CurrentCell.Floors[k].FloorWalls[(int)EFloorName::Roof]->WallSegments.insert(std::make_pair(&RoofSegmentMaterial, std::shared_ptr<FProceduralFigureRectangle>(
				new FProceduralFigureRectangle(RoofCoordinateBaseSegment, FVector(0, 0, -1), FVector(1, 0, 0), RoofSizeBaseSegment, RoofSegmentMaterial))));

			CreateFloorSegmentsForCurrentDirection(CurrentCell, EDirection::XP, RoofCoordinateBaseSegment, RoofSizeBaseSegment, RoofSegmentMaterial, EFloorName::Roof,
				*CurrentCell.Floors[k].FloorWalls[(int)EFloorName::Roof]);

			CreateFloorSegmentsForCurrentDirection(CurrentCell, EDirection::YP, RoofCoordinateBaseSegment, RoofSizeBaseSegment, RoofSegmentMaterial, EFloorName::Roof,
				*CurrentCell.Floors[k].FloorWalls[(int)EFloorName::Roof]);


			CreateSkirting(CurrentCell, k, EDirection::XP, RoofCoordinateBaseSegment, RoofSizeBaseSegment, RoofSegmentMaterial, EFloorName::Roof);

			CreateSkirting(CurrentCell, k, EDirection::YP, RoofCoordinateBaseSegment, RoofSizeBaseSegment, RoofSegmentMaterial, EFloorName::Roof);

			CreateSkirting(CurrentCell, k, EDirection::XM, RoofCoordinateBaseSegment, RoofSizeBaseSegment, RoofSegmentMaterial, EFloorName::Roof);

			CreateSkirting(CurrentCell, k, EDirection::YM, RoofCoordinateBaseSegment, RoofSizeBaseSegment, RoofSegmentMaterial, EFloorName::Roof);

			CreateTriangelSegmentInCorner(CurrentCell, EFloorName::Roof, k, RoofCoordinateBaseSegment, RoofSegmentMaterial);
		}
	}

	if (CurrentCell.LastRoofUp)
	{
		FVector CoordinateBaseSegment(0, 0, (CurrentCell.Floors.size())  * LevelSettings.CellHeight);
		FVector2D SizeBaseSegment(LevelSettings.CellSize - LevelSettings.WallThickness, LevelSettings.CellSize - LevelSettings.WallThickness);

		CurrentCell.LastRoofUp->WallSegments.insert(std::make_pair(&LevelSettings.FloorMaterial, std::shared_ptr<FProceduralFigureRectangle>(
			new FProceduralFigureRectangle(CoordinateBaseSegment, FVector(0, 0, 1), FVector(1, 0, 0), SizeBaseSegment, LevelSettings.FloorMaterial))));

		CreateFloorSegmentsForCurrentDirection(CurrentCell, EDirection::XP, CoordinateBaseSegment, SizeBaseSegment, LevelSettings.FloorMaterial, EFloorName::Floore,
			*CurrentCell.LastRoofUp);

		CreateFloorSegmentsForCurrentDirection(CurrentCell, EDirection::YP, CoordinateBaseSegment, SizeBaseSegment, LevelSettings.FloorMaterial, EFloorName::Floore,
			*CurrentCell.LastRoofUp);

	}

	if (CurrentCell.ZeroFlooreDown)
	{
		FVector CoordinateBaseSegment(0, 0, -LevelSettings.WallThickness);

		FVector2D SizeBaseSegment(LevelSettings.CellSize - LevelSettings.WallThickness, LevelSettings.CellSize - LevelSettings.WallThickness);

		CurrentCell.ZeroFlooreDown->WallSegments.insert(std::make_pair(&LevelSettings.RoofMaterial, std::shared_ptr<FProceduralFigureRectangle>(
			new FProceduralFigureRectangle(CoordinateBaseSegment, FVector(0, 0, -1), FVector(1, 0, 0), SizeBaseSegment, LevelSettings.RoofMaterial))));


		CreateFloorSegmentsForCurrentDirection(CurrentCell, EDirection::XP, CoordinateBaseSegment, SizeBaseSegment, LevelSettings.RoofMaterial, EFloorName::Roof,
			*CurrentCell.ZeroFlooreDown);

		CreateFloorSegmentsForCurrentDirection(CurrentCell, EDirection::YP, CoordinateBaseSegment, SizeBaseSegment, LevelSettings.RoofMaterial, EFloorName::Roof,
			*CurrentCell.ZeroFlooreDown);
	}


}

void FLevelGenBildingZoneBase::CreateTriangelSegmentInCorner(FLevelCellData& CurrentCell, EFloorName FloorName, int Floor, FVector CoordinateBaseSegment, const FLevelGeneratorMaterialSettings& BaseSegmentMaterial)
{
	FVector SegmentNormal;

	if (FloorName == EFloorName::Floore) SegmentNormal = FVector(0, 0, 1);

	else if (FloorName == EFloorName::Roof) SegmentNormal = FVector(0, 0, -1);

	for (int l = 0; l < (int)EDirection::end; l++)
	{
		EDirection Direction = (EDirection)l;

		if ((CurrentCell.Floors[Floor].FloorWalls[(int)FloorName]->FloorJoints[(int)Direction].Type == EWallCornerType::Normal ||
			CurrentCell.Floors[Floor].FloorWalls[(int)FloorName]->FloorJoints[(int)Direction].Type == EWallCornerType::NormalWithEdge) &&
			(CurrentCell.Floors[Floor].FloorWalls[(int)FloorName]->FloorJoints[(int)EDirectionGetRight(Direction)].Type == EWallCornerType::Normal ||
				CurrentCell.Floors[Floor].FloorWalls[(int)FloorName]->FloorJoints[(int)EDirectionGetRight(Direction)].Type == EWallCornerType::NormalWithEdge))
		{
			FVector VertexCoordinate_1 = CoordinateBaseSegment + EDirectionGetVector(EDirectionGetRight(Direction)) * (LevelSettings.CellSize / 2 - LevelSettings.WallThickness / 2) +
				EDirectionGetVector(Direction) * (LevelSettings.CellSize / 2 - LevelSettings.WallThickness / 2);

			FProceduralFigureTriangle TriangleSegment(VertexCoordinate_1,
				VertexCoordinate_1 + EDirectionGetVector(Direction) * LevelSettings.WallThickness,
				VertexCoordinate_1 + EDirectionGetVector(EDirectionGetRight(Direction)) * LevelSettings.WallThickness,
				BaseSegmentMaterial, SegmentNormal, FVector(1, 0, 0));

			CurrentCell.Floors[Floor].FloorWalls[(int)FloorName]->WallSegments[&BaseSegmentMaterial]->Merge(TriangleSegment);
		}
	}
}


void FLevelGenBildingZoneBase::CreateFloorSegmentsForCurrentDirection(FLevelCellData& CurrentCell, EDirection Direction,
	FVector CoordinateBaseSegment, FVector2D SizeBaseSegment, const FLevelGeneratorMaterialSettings& BaseSegmentMaterial, EFloorName FloorName, FCellFlloor& CurrentFloor)
{
	FVector SegmentNormal;

	if (FloorName == EFloorName::Floore) SegmentNormal = FVector(0, 0, 1);

	else if (FloorName == EFloorName::Roof) SegmentNormal = FVector(0, 0, -1);

	else throw;


	FVector2D SizeSegment;

	if (Direction == EDirection::XP) SizeSegment = FVector2D(LevelSettings.WallThickness, SizeBaseSegment.X);

	else if (Direction == EDirection::YP) SizeSegment = FVector2D(SizeBaseSegment.Y, LevelSettings.WallThickness);

	else throw;


	if (CurrentFloor.FloorJoints[(int)Direction].Type == EWallCornerType::PositiveOffset ||
		CurrentFloor.FloorJoints[(int)Direction].Type == EWallCornerType::Normal || CurrentFloor.FloorJoints[(int)Direction].Type == EWallCornerType::NormalWithEdge)
	{

		FProceduralFigureRectangle NewSegment1(CoordinateBaseSegment +
			EDirectionGetVector(Direction) * (LevelSettings.CellSize) / 2, SegmentNormal, FVector(1, 0, 0), SizeSegment, BaseSegmentMaterial);

		CurrentFloor.WallSegments[&BaseSegmentMaterial]->Merge(NewSegment1);

		if (Direction == EDirection::YP && CurrentFloor.FloorJoints[(int)EDirectionGetRight(Direction)].Type == EWallCornerType::PositiveOffset)
		{
			FProceduralFigureRectangle NewSegment2(CoordinateBaseSegment + EDirectionGetVector(EDirectionGetRight(Direction)) * (LevelSettings.CellSize) / 2 +
				EDirectionGetVector(Direction) * (LevelSettings.CellSize) / 2, SegmentNormal, FVector(1, 0, 0), FVector2D(LevelSettings.WallThickness, LevelSettings.WallThickness), BaseSegmentMaterial);


			CurrentFloor.WallSegments[&BaseSegmentMaterial]->Merge(NewSegment2);
		}

	}


	if (CurrentFloor.FloorJoints[(int)EDirectionInvert(Direction)].Type == EWallCornerType::Normal || CurrentFloor.FloorJoints[(int)EDirectionInvert(Direction)].Type == EWallCornerType::NormalWithEdge)
	{
		FProceduralFigureRectangle NewSegment1(CoordinateBaseSegment +
			EDirectionGetVector(EDirectionInvert(Direction)) * (LevelSettings.CellSize) / 2, SegmentNormal, FVector(1, 0, 0), SizeSegment, BaseSegmentMaterial);

		CurrentFloor.WallSegments[&BaseSegmentMaterial]->Merge(NewSegment1);
	}
}

void FLevelGenBildingZoneBase::CreateSkirting(FLevelCellData& CurrentCell, int Floor, EDirection Direction,
	FVector CoordinateBaseSegment, FVector2D SizeBaseSegment, const FLevelGeneratorMaterialSettings& BaseSegmentMaterial, EFloorName FloorName)
{
	const FLevelGeneratorMaterialSettings& SkirtingSegmentMaterial = FloorName == EFloorName::Roof && CurrentCell.Floors[Floor].FloorWalls[(int)FloorName]->FloorJoints[(int)Direction].Type == EWallCornerType::Normal &&
		static_cast<FCellWall*>(CurrentCell.Floors[Floor].FloorWalls[(int)FloorName]->FloorJoints[(int)Direction].OtherWall)->WallType != EWallType::InternalWall ?
		LevelSettings.BildingWallMaterial : LevelSettings.WallsMaterial;

	FVector SegmentCoordinateOffset = FloorName == EFloorName::Floore ?
		EDirectionGetVector(Direction) * (LevelSettings.CellSize + LevelSettings.WallThickness) / 2 + FVector(0, 0, -1) * LevelSettings.WallThickness / 2 :
		EDirectionGetVector(Direction) * (LevelSettings.CellSize + LevelSettings.WallThickness) / 2 + FVector(0, 0, 1) * LevelSettings.WallThickness / 2;


	if ((FloorName == EFloorName::Floore && CurrentCell.Floors[Floor].FloorWalls[(int)FloorName]->FloorJoints[(int)Direction].Type == EWallCornerType::NormalWithEdge) ||
		(FloorName == EFloorName::Roof && CurrentCell.Floors[Floor].FloorWalls[(int)FloorName]->FloorJoints[(int)Direction].Type == EWallCornerType::Normal))
	{
		if (Floor == 0 && FloorName == EFloorName::Floore && CurrentCell.CellInst->GetCellType() == ELevelCellType::Tower) return;

		std::shared_ptr<FProceduralFigureRectangle> NewSegment1(new FProceduralFigureRectangle(CoordinateBaseSegment + SegmentCoordinateOffset, EDirectionGetVector(Direction),
			EDirectionGetVector(EDirectionGetRight(Direction)), FVector2D(SizeBaseSegment.X, LevelSettings.WallThickness), SkirtingSegmentMaterial));


		if (CurrentCell.Floors[Floor].FloorWalls[(int)FloorName]->WallSegments.find(&SkirtingSegmentMaterial) !=
			CurrentCell.Floors[Floor].FloorWalls[(int)FloorName]->WallSegments.end())
		{
			auto MatIt = CurrentCell.Floors[Floor].FloorWalls[(int)FloorName]->WallSegments.find(&SkirtingSegmentMaterial);
			MatIt->second->Merge(*NewSegment1);

		}
		else
		{
			CurrentCell.Floors[Floor].FloorWalls[(int)FloorName]->WallSegments.insert(std::make_pair(&SkirtingSegmentMaterial, NewSegment1));
		}

		if (CurrentCell.Floors[Floor].FloorWalls[(int)FloorName]->FloorJoints[(int)EDirectionGetRight(Direction)].Type == EWallCornerType::PositiveOffset)
		{
			FProceduralFigureRectangle NewSegment2(CoordinateBaseSegment + EDirectionGetVector(EDirectionGetRight(Direction)) * (LevelSettings.CellSize) / 2 + SegmentCoordinateOffset,
				EDirectionGetVector(Direction), EDirectionGetVector(EDirectionGetRight(Direction)), FVector2D(LevelSettings.WallThickness, LevelSettings.WallThickness), SkirtingSegmentMaterial);


			CurrentCell.Floors[Floor].FloorWalls[(int)FloorName]->WallSegments[&SkirtingSegmentMaterial]->Merge(NewSegment2);

		}


		FProceduralFigureRectangle NewSegment2(CoordinateBaseSegment + EDirectionGetVector(EDirectionGetRight(Direction)) * (LevelSettings.CellSize) / 2 + SegmentCoordinateOffset - EDirectionGetVector(Direction) * LevelSettings.WallThickness / 2,
			(EDirectionGetVector(Direction).Rotation() + FRotator(0, 45, 0)).Vector(), (EDirectionGetVector(Direction).Rotation() + FRotator(0, -45, 0)).Vector(),
			FVector2D(LevelSettings.WallThickness * sqrt(2), LevelSettings.WallThickness), SkirtingSegmentMaterial);

		CurrentCell.Floors[Floor].FloorWalls[(int)FloorName]->WallSegments[&SkirtingSegmentMaterial]->Merge(NewSegment2);


	}

}


void FLevelGenBildingZoneBase::CreateProceduralFigure()
{
	
	BildingAreaFigure = std::shared_ptr<FProceduralFigurBuffer>(new FProceduralFigurBuffer(FVector(GetSenterCoordinate() * LevelSettings.CellSize, 0)));

	for (int i = 0; i < (int)EDirection::end; i++)
	{
		CreateFasadeWall(*BildingAreaFigure, (EDirection)i);
	}

	for (int i = GetBeginCoordinate().X; i < GetBeginCoordinate().X + GetSize().X; i++)
	{
		for (int j = GetBeginCoordinate().Y; j < GetBeginCoordinate().Y + GetSize().Y; j++)
		{
			CreateLevelCell(*BildingAreaFigure, FVector2D(i, j));
		}
	}

}

void FLevelGenBildingZoneBase::EdgeCut()
{
	for (int i = 0; i < (int)EDirection::end; i++)
	{
		std::vector<FVector2D> WallCellsCoordinate;

		GetFasedeWallCellsCoordinats(WallCellsCoordinate, (EDirection)i);

		for (auto CurrentCoordinate = WallCellsCoordinate.begin(); CurrentCoordinate != WallCellsCoordinate.end(); CurrentCoordinate++)
		{
			if (CheckCellFloors(*CurrentCoordinate))
			{
				FVector2D CutCellCoordinate = *CurrentCoordinate + EDirectionGetVector2D((EDirection)i);

				while (CheckCell(CutCellCoordinate))
				{
					ReplaceCell(CutCellCoordinate);
					CutCellCoordinate = CutCellCoordinate + EDirectionGetVector2D((EDirection)i);
				}
			}
			
			
		}
	}
}



bool FLevelGenBildingZoneBase::CheckCell(FVector2D Coordinate)
{
	
	if (DataStorage.LevelMap.IsCellExist(Coordinate) && DataStorage.LevelMap.GetCell(Coordinate)->CellInst && DataStorage.LevelMap.GetCell(Coordinate)->CellInst->CanEdgeCut())
	{
		for (auto CellFloorIT = DataStorage.LevelMap.GetCell(Coordinate)->Floors.begin(); CellFloorIT != DataStorage.LevelMap.GetCell(Coordinate)->Floors.end(); CellFloorIT++)
		{
			if (CellFloorIT->PlasedRoom)
			{
				return false;
			}
		}

		return true;

	}

	return false;
}

bool FLevelGenBildingZoneBase::CheckCellFloors(FVector2D Coordinate)
{

	if (DataStorage.LevelMap.IsCellExist(Coordinate))
	{
		for (auto CellFloorIT = DataStorage.LevelMap.GetCell(Coordinate)->Floors.begin(); CellFloorIT != DataStorage.LevelMap.GetCell(Coordinate)->Floors.end(); CellFloorIT++)
		{
			if (CellFloorIT->PlasedRoom)
			{
				return false;
			}
		}

		return true;

	}

	return false;
}

void FLevelGenBildingZoneBase::ReplaceCell(FVector2D Coordinate)
{
	std::shared_ptr<FLevelCellBase> LastCell = std::move(DataStorage.LevelMap.GetCell(Coordinate)->CellInst);
	FLevelCellBilding* Bilding = static_cast<FLevelCellBilding*>(LastCell.get());
	DataStorage.LevelMap.GetCell(Coordinate)->CellInst = std::shared_ptr<FLevelCellBase>(new FLevelCellThroughCell(*Bilding));
}


void FLevelGenBildingZoneBase::CreateFasadeWall(FProceduralFigurBuffer& WallBuffer, EDirection Direction)
{
	std::vector<FVector2D> WallCells;

	GetFasedeWallCellsCoordinats(WallCells, Direction);


	for (auto CurrentCellCoordinate : WallCells)
	{
		FLevelCellData& CurrentCell = *DataStorage.LevelMap.GetCell(CurrentCellCoordinate);
		for (int k = 0; k < CurrentCell.Floors.size(); k++)
		{
			if (CurrentCell.Floors[k].Walls.find(Direction) != CurrentCell.Floors[k].Walls.end())
			{
				auto WallIt = CurrentCell.Floors[k].Walls.find(Direction);
				WallBuffer.AddWallSegments(WallIt->second->WallSegments, FVector(CurrentCellCoordinate * LevelSettings.CellSize, 0));

			}
		}


	}


}

void FLevelGenBildingZoneBase::CreateLevelCell(FProceduralFigurBuffer& WallBuffer, FVector2D CellCoordinate)
{
	FLevelCellData& CurrentCell = *DataStorage.LevelMap.GetCell(CellCoordinate);

	FVector CellCenterCoordinate(CellCoordinate * LevelSettings.CellSize, 0);

	for (int k = 0; k < CurrentCell.Floors.size(); k++)
	{
		for (int l = 0; l < (int)EDirection::end; l++)
		{
			if (CurrentCell.Floors[k].Walls.find((EDirection)l) != CurrentCell.Floors[k].Walls.end())
			{
				auto WallIt = CurrentCell.Floors[k].Walls.find((EDirection)l);

				WallBuffer.AddWallSegments(WallIt->second->WallSegments, CellCenterCoordinate);


			}
		}

		if (CurrentCell.Floors[k].FloorWalls[(int)EFloorName::Floore])
		{
			WallBuffer.AddWallSegments(CurrentCell.Floors[k].FloorWalls[(int)EFloorName::Floore]->WallSegments, CellCenterCoordinate);
		}

		if (CurrentCell.Floors[k].FloorWalls[(int)EFloorName::Roof])
		{
			WallBuffer.AddWallSegments(CurrentCell.Floors[k].FloorWalls[(int)EFloorName::Roof]->WallSegments, CellCenterCoordinate);
		}


	}

	if (CurrentCell.ZeroFlooreDown) WallBuffer.AddWallSegments(CurrentCell.ZeroFlooreDown->WallSegments, CellCenterCoordinate);

	if (CurrentCell.LastRoofUp) WallBuffer.AddWallSegments(CurrentCell.LastRoofUp->WallSegments, CellCenterCoordinate);
}

void FLevelGenBildingZoneBase::GetPossibreLinksBetweenBildingZone(std::map<float, FPossibleLink>& PossibleLinks, FLevelGenBildingZoneBase* OtherBildingZone, int FloorNum)
{

	for (int i = 0; i < GraphLayers[FloorNum].Graph.size(); i++)
	{
		for (int j = 0; j < GraphLayers[FloorNum].Graph[i].size(); j++)
		{
			if (GraphLayers[FloorNum].Graph[i][j]->NodeRoom)
			{
				for (int k = 0; k < OtherBildingZone->GraphLayers[FloorNum].Graph.size(); k++)
				{
					for (int l = 0; l < OtherBildingZone->GraphLayers[FloorNum].Graph[k].size(); l++)
					{
						if (OtherBildingZone->GraphLayers[FloorNum].Graph[k][l]->NodeRoom)
						{
							std::pair<FJointPart*, FJointPart*> JointParts;
							if (GraphLayers[FloorNum].Graph[i][j]->NodeRoom->FindNearestJointPair(JointParts, *OtherBildingZone->GraphLayers[FloorNum].Graph[k][l]->NodeRoom))
							{
								float Dist = FVector::Dist(JointParts.first->GetCoordinate(), JointParts.second->GetCoordinate());
								PossibleLinks.insert(std::make_pair(Dist, FPossibleLink(std::make_pair(GraphLayers[FloorNum].Graph[i][j].get(), OtherBildingZone->GraphLayers[FloorNum].Graph[k][l].get()),
									std::make_pair(JointParts.first, JointParts.second))));
							}

						}

					}
				}
					
			}
			
		}
	}

	
}

//*********************************************
//FLevelGenBildingZone
//*********************************************

FLevelGenBildingZone::FLevelGenBildingZone(bool UnPlacebelBilding, FVector2D Start, FVector2D End, FDataStorage& DataStorage,
	const FLevelGeneratorSettings& _LevelSettings, const ALevelGenerator& _LevelGenerator) :
	FLevelGenBildingZoneBase(UnPlacebelBilding, Start, End, DataStorage, _LevelSettings, _LevelGenerator)
{
	if (UnPlacebelBilding) return;

	CreateTowers(LevelSettings, LevelGenerator, DataStorage);

	ConnectBildingToLevelCells(DataStorage);

	CreateGraphLayers();

	CreateNods();

	SetNodsRoom();

	CreateLinksRooms();
}

void FLevelGenBildingZone::CreateProceduralFigure()
{
	FLevelGenBildingZoneBase::CreateProceduralFigure();

	FProceduralFigureRectangle GroundRectangle(FVector(0, 0, -(LevelSettings.SecondCityFloor + LevelSettings.FirstCityFloor)), FVector(0, 0, 1), FVector(1, 0, 0),
		GetSize() * LevelSettings.CellSize, LevelSettings.RoadGroundMaterial);

	BildingAreaFigure->AddProceduralFigure(GroundRectangle, FVector(GetSenterCoordinate().X *  LevelSettings.CellSize,
		GetSenterCoordinate().Y * LevelSettings.CellSize, 0), &LevelSettings.RoadGroundMaterial);
}

void FLevelGenBildingZone::ConnectBildingToLevelCells(FDataStorage& DataStorage)
{
	for (int i = 0; i < (int)EDirection::end; i++)
	{
		std::vector<FVector2D> WallCellsCoordinate;

		GetFasedeWallCellsCoordinats(WallCellsCoordinate, (EDirection)i);

		for (auto CurrentCoordinate = WallCellsCoordinate.begin(); CurrentCoordinate != WallCellsCoordinate.end(); CurrentCoordinate++)
		{
			DataStorage.LevelMap.GetCell(*CurrentCoordinate)->LinkedBildingZone = this;
		}
	}

	for (int i = Coordinate.X; i < Coordinate.X + Size.X; i++)
	{
		for (int j = Coordinate.Y; j < Coordinate.Y + Size.Y; j++)
		{
			if (!DataStorage.LevelMap.GetCell(i, j)->CellInst)
			{
				DataStorage.LevelMap.GetCell(i, j)->CellInst = std::shared_ptr<FLevelCellBase>(ELevelCellTypeCreate(ELevelCellType::Bilding, *DataStorage.LevelMap.GetCell(i, j)));

			}
			else
			{
				if (DataStorage.LevelMap.GetCell(i, j)->CellInst->GetCellType() != ELevelCellType::Tower)
				{
					DataStorage.LevelMap.GetCell(i, j)->CellInst = std::shared_ptr<FLevelCellBase>(ELevelCellTypeCreate(ELevelCellType::Bilding, *DataStorage.LevelMap.GetCell(i, j)));
					UE_LOG(LogTemp, Warning, TEXT("ConnectBildingToLevelCells bad cell type %s"),
						*GetEnumValueAsString<ELevelCellType>("ELevelCellType", DataStorage.LevelMap.GetCell(i, j)->CellInst->GetCellType()));

				}
			}
			DataStorage.LevelMap.GetCell(i, j)->LinkedBildingZone = this;
		}
	}

}


//*********************************************
//FLevelGenBildingZone
//*********************************************

FLevelGenBildingZoneTower::FLevelGenBildingZoneTower(bool UnPlacebelBilding, FVector2D Start, FVector2D End, FDataStorage& DataStorage,
	const FLevelGeneratorSettings& _LevelSettings, const ALevelGenerator& _LevelGenerator) :
	FLevelGenBildingZoneBase(UnPlacebelBilding, Start, End, DataStorage, _LevelSettings, _LevelGenerator)

{
	if (UnPlacebelBilding) return;
	
	ConnectBildingToLevelCells(DataStorage);

	CreateGraphLayers();

	CreateNods();

	SetNodsRoom();

	CreateLinksRooms();
}

void FLevelGenBildingZoneTower::CreateProceduralFigure()
{
	FLevelGenBildingZoneBase::CreateProceduralFigure();

	float TowerHeight = 5000 + GetSize().X * GetSize().Y * 200;

	TowerHeight = TowerHeight > 14000 ? 14000 : TowerHeight;

	for (int l = 0; l < (int)EDirection::end; l++)
	{
		FVector WallNormalVector = EDirectionGetVector(EDirectionInvert((EDirection)l));
		FVector WallXVector = EDirectionGetVector(EDirectionGetRight(EDirectionInvert((EDirection)l)));

		float WallWidth = (EDirection)l == EDirection::YP || (EDirection)l == EDirection::YM ? LevelSettings.CellSize * GetSize().X : LevelSettings.CellSize * GetSize().Y;

		FVector CoordinatOffset = (EDirection)l == EDirection::XP || (EDirection)l == EDirection::XM ? WallNormalVector * (LevelSettings.CellSize * GetSize().X * 0.5 + LevelSettings.WallThickness / 2):
			WallNormalVector * (LevelSettings.CellSize * GetSize().Y * 0.5 + LevelSettings.WallThickness / 2);

		CreateTowerFasedeWallSegment(CoordinatOffset,
				WallNormalVector, WallXVector, WallWidth - LevelSettings.WallThickness, TowerHeight);


		FRotator WallRotate = WallNormalVector.Rotation();

		FVector CoordinatOffsetLeft = (EDirection)l == EDirection::XP || (EDirection)l == EDirection::XM ? EDirectionGetVector(EDirectionGetLeft((EDirection)l)) * LevelSettings.CellSize * GetSize().Y * 0.5 :
			EDirectionGetVector(EDirectionGetLeft((EDirection)l)) * LevelSettings.CellSize * GetSize().X * 0.5;

		float WallTicnessOffsetX = 0;

		if ((EDirection)l == EDirection::XM)  WallTicnessOffsetX = LevelSettings.WallThickness * 0.5;
		else if ((EDirection)l == EDirection::XP)  WallTicnessOffsetX = -LevelSettings.WallThickness * 0.5;

		float WallTicnessOffsetY = 0;

		if ((EDirection)l == EDirection::YM)  WallTicnessOffsetY = LevelSettings.WallThickness * 0.5;
		else if ((EDirection)l == EDirection::YP)  WallTicnessOffsetY = -LevelSettings.WallThickness * 0.5;

		CreateTowerFasedeWallSegment(CoordinatOffset + CoordinatOffsetLeft - FVector(WallTicnessOffsetX, WallTicnessOffsetY, 0),
			(WallRotate + FRotator(0, 45, 0)).Vector(), (WallRotate + FRotator(0, -45, 0)).Vector(), LevelSettings.WallThickness * sqrt(2), TowerHeight);

	}
}

void FLevelGenBildingZoneTower::CreateTowerFasedeWallSegment(FVector WallCoordinate, FVector WallNormalVector, FVector WallXVector, float SegmentWeight, float TowerHeight)
{
	FProceduralFigureRectangle DownRectangle(WallCoordinate + FVector(0, 0, -(LevelSettings.SecondCityFloor + LevelSettings.FirstCityFloor) / 2) , WallNormalVector, WallXVector,
		FVector2D(SegmentWeight, LevelSettings.SecondCityFloor + LevelSettings.FirstCityFloor), LevelSettings.BildingWallMaterial);

	BildingAreaFigure->AddProceduralFigure(DownRectangle, FVector(GetSenterCoordinate().X *  LevelSettings.CellSize,
		GetSenterCoordinate().Y * LevelSettings.CellSize, 0), &LevelSettings.BildingWallMaterial);

	FProceduralFigureRectangle UpRectangle(WallCoordinate + FVector(0, 0, TowerHeight / 2 + LevelSettings.FloorNum * LevelSettings.CellHeight), WallNormalVector, WallXVector,
		FVector2D(SegmentWeight, TowerHeight), LevelSettings.BildingWallMaterial);

	BildingAreaFigure->AddProceduralFigure(UpRectangle, FVector(GetSenterCoordinate().X *  LevelSettings.CellSize,
		GetSenterCoordinate().Y * LevelSettings.CellSize, 0), &LevelSettings.BildingWallMaterial);
}

void FLevelGenBildingZoneTower::ConnectBildingToLevelCells(FDataStorage& DataStorage)
{
	for (int i = 0; i < (int)EDirection::end; i++)
	{
		std::vector<FVector2D> WallCellsCoordinate;

		GetFasedeWallCellsCoordinats(WallCellsCoordinate, (EDirection)i);

		for (auto CurrentCoordinate = WallCellsCoordinate.begin(); CurrentCoordinate != WallCellsCoordinate.end(); CurrentCoordinate++)
		{
			DataStorage.LevelMap.GetCell(*CurrentCoordinate)->LinkedBildingZone = this;
		}
	}
	
	for (int i = Coordinate.X; i < Coordinate.X + Size.X; i++)
	{
		for (int j = Coordinate.Y; j < Coordinate.Y + Size.Y; j++)
		{
			DataStorage.LevelMap.GetCell(i, j)->CellInst = std::shared_ptr<FLevelCellBase>(ELevelCellTypeCreate(ELevelCellType::Tower, *DataStorage.LevelMap.GetCell(i, j)));
			DataStorage.LevelMap.GetCell(i, j)->LinkedBildingZone = this;
		}
	}

}