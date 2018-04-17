// Copyright 2018 Pavlov Dmitriy
#include "DataFillers.h"
#include "LevelCell.h"
#include "LevelGraphNode.h"
#include "LevelGenCore.h"
#include "LevelGeneratorSettings.h"
#include "LevelGenPluginRuntime_LogCategory.h"
#include <time.h>
#include <algorithm>
#include <random>   
#include <ctime>
#include "Rooms/LevelRooms.h"
#include "LevelGenerator.h"
#include "Towers/TowerStorage.h"
#include "LevelGenRoad.h"
#include "LevelFloor.h"
#include "Rooms/PlacedLevelRoom.h"
#include "Towers/PlacedLevelTower.h"
#include "LevelGenBildingZone.h"
#include "Towers/LevelTowers.h"
#include "HoverCars/CarTrackGraph.h"
#include "ProceduralFigure.h"
#include "DataStorage.h"
#include "LevelBilders.h"
#include "ProceduralMeshActor.h"

static_assert(EDataFiller::End == (EDataFiller)6, "You must update CreateComponent after add new enum");

FDataFillerBase* EDataFillerCreate(EDataFiller ComponentType, FDataStorage& DataStorage, const FLevelGeneratorSettings& LevelSettings, const ALevelGenerator& LevelGenerator)
{
	switch (ComponentType)
	{

	case EDataFiller::Graph:

		return new FDataFillerGraph(DataStorage, LevelSettings, LevelGenerator);

	case EDataFiller::RoadMap:

		return new FDataFillerRoadMap(DataStorage, LevelSettings, LevelGenerator);

	case EDataFiller::MainRoadMap:

		return new FDataFillerMainRoadMap(DataStorage, LevelSettings, LevelGenerator);


	case EDataFiller::BildingsZoneCreater:

		return new FDataFillerBildingsZoneCreater(DataStorage, LevelSettings, LevelGenerator);

	

	case EDataFiller::BildingDisposer:

		return new FDataFillerBildingDisposer(DataStorage, LevelSettings, LevelGenerator);

	case EDataFiller::CarTrack:

		return new FDataFillerCarTrack(DataStorage, LevelSettings, LevelGenerator);


	default: throw;

	}

	
}

FDataFillerMainRoadMap::FDataFillerMainRoadMap(FDataStorage& _DataStorage, const FLevelGeneratorSettings& _LevelSettings, const ALevelGenerator& _LevelGenerator) :
	FDataFillerBase(_DataStorage, _LevelSettings, _LevelGenerator) {}

FDataFillerMainRoadMap::~FDataFillerMainRoadMap() = default;


//******************************************************
//FDataFillerMainRoadMap
//*******************************************************

void FDataFillerMainRoadMap::FillData(FVector2D Start, FVector2D End)
{
	FVector2D StartRoadCells = FVector2D(RoundNumber(Start.X / LevelSettings.RoadFrequency, ERoundType::Up), RoundNumber(Start.Y / LevelSettings.RoadFrequency, ERoundType::Up));
	FVector2D EndRoadCells = FVector2D(RoundNumber(End.X / LevelSettings.RoadFrequency, ERoundType::Up), RoundNumber(End.Y / LevelSettings.RoadFrequency, ERoundType::Up));

	CreateMainRoads(StartRoadCells, EndRoadCells);
	
	SetMainRoads(StartRoadCells, EndRoadCells);
		
}



void FDataFillerMainRoadMap::CreateMainRoads(FVector2D StartRoadCells, FVector2D EndRoadCells)
{
	FVector2D StartMainRoadCells = FVector2D(RoundNumber(StartRoadCells.X / LevelSettings.MainRoadFrequency, ERoundType::Up), RoundNumber(StartRoadCells.Y / LevelSettings.MainRoadFrequency, ERoundType::Up));
	FVector2D EndMainRoadCells = FVector2D(RoundNumber(EndRoadCells.X / LevelSettings.MainRoadFrequency, ERoundType::Up), RoundNumber(EndRoadCells.Y / LevelSettings.MainRoadFrequency, ERoundType::Up));

	auto FillerX = [&](int X)
	{
		ERoadDirection Direction = ERoadDirection::Vertical;

		int	MinNumber = (X - 1) * LevelSettings.MainRoadFrequency;
		
		int MaxNumber = (X + 1) * LevelSettings.MainRoadFrequency;
		
		int RoadColumnNumber = LevelGenerator.GetRandomGenerator().GetRandomNumber(MinNumber, MaxNumber);

		while (MainRoadsXMap.find(RoadColumnNumber) != MainRoadsXMap.end())
		{
			RoadColumnNumber = LevelGenerator.GetRandomGenerator().GetRandomNumber(MinNumber, MaxNumber);
		}

		int Coordinate = RoadColumnNumber * LevelSettings.RoadFrequency;

		std::unique_ptr<FMainRoad> NewMainRoad(new FMainRoad(Direction, Coordinate, RoadColumnNumber, LevelSettings.MainRoadSize, LevelSettings));
		MainRoadsXMap.insert(std::make_pair(RoadColumnNumber, NewMainRoad.get()));

		return NewMainRoad;
	};

	MainRoadsX.FillRange(StartMainRoadCells.X, EndMainRoadCells.X, FillerX);

	auto FillerY = [&](int Y)
	{
		ERoadDirection Direction = ERoadDirection::Horizontal;

		int MinNumber = (Y - 1) * LevelSettings.MainRoadFrequency;
		
		int MaxNumber = (Y + 1) * LevelSettings.MainRoadFrequency;

		int RoadColumnNumber = LevelGenerator.GetRandomGenerator().GetRandomNumber(MinNumber, MaxNumber);

		while (MainRoadsXMap.find(RoadColumnNumber) != MainRoadsXMap.end())
		{
			RoadColumnNumber = LevelGenerator.GetRandomGenerator().GetRandomNumber(MinNumber, MaxNumber);
		}

		int Coordinate = RoadColumnNumber * LevelSettings.RoadFrequency;

		std::unique_ptr<FMainRoad> NewMainRoad(new FMainRoad(Direction, Coordinate, RoadColumnNumber, LevelSettings.MainRoadSize, LevelSettings));

		MainRoadsYMap.insert(std::make_pair(RoadColumnNumber, NewMainRoad.get()));

		return NewMainRoad;
	};

	MainRoadsY.FillRange(StartMainRoadCells.Y, EndMainRoadCells.Y, FillerY);
}

void FDataFillerMainRoadMap::SetMainRoads(FVector2D StartRoadCells, FVector2D EndRoadCells)
{
	auto Filler = [&](int X, int Y)
	{
		int Coordinate;

		if ((IsNumberEven(X) == IsNumberEven(Y)))
		{
			if (MainRoadsYMap.find(Y) != MainRoadsYMap.end())
			{
				Coordinate = MainRoadsYMap[Y]->Coordinate;
			}
			else
			{
				return std::unique_ptr<FLevelGenRoadBase>();
			}
			if (DataStorage.RoadsMap.IsCellExist(X, Y))  UE_LOG(LogTemp, Error, TEXT("SetMainRoads error"));
			return std::unique_ptr<FLevelGenRoadBase>(new FLevelGenRoadHorizontal(Coordinate, LevelSettings.MainRoadSize, true, FVector2D(X, Y), DataStorage.RoadsMap, LevelSettings, LevelGenerator));

		}
		else
		{

			if (MainRoadsXMap.find(X) != MainRoadsXMap.end())
			{
				Coordinate = MainRoadsXMap[X]->Coordinate;
			}
			else
			{
				return std::unique_ptr<FLevelGenRoadBase>();
			}
			if (DataStorage.RoadsMap.IsCellExist(X, Y))  UE_LOG(LogTemp, Error, TEXT("SetMainRoads error"));
			return std::unique_ptr<FLevelGenRoadBase>(new FLevelGenRoadVertical(Coordinate, LevelSettings.MainRoadSize, true, FVector2D(X, Y), DataStorage.RoadsMap, LevelSettings, LevelGenerator));

		}

	};

	DataStorage.RoadsMap.FillRange(StartRoadCells, EndRoadCells, Filler);
}


int FDataFillerMainRoadMap::GetDeltaOffsetGenerateArea() const
{
	return  LevelSettings.RoadFrequency * LevelSettings.MainRoadFrequency;
}

//******************************************************
//FDataFillerRoadMap
//*******************************************************

void FDataFillerRoadMap::FillData(FVector2D Start, FVector2D End)
{
	FVector2D StartRoadCells = FVector2D(RoundNumber(Start.X / LevelSettings.RoadFrequency, ERoundType::Up), RoundNumber(Start.Y / LevelSettings.RoadFrequency, ERoundType::Up));
	FVector2D EndRoadCells = FVector2D(RoundNumber(End.X / LevelSettings.RoadFrequency, ERoundType::Up), RoundNumber(End.Y / LevelSettings.RoadFrequency, ERoundType::Up));
	
	CreateRoads(StartRoadCells, EndRoadCells);
	
	CheckRoadPlase(StartRoadCells, EndRoadCells);
	
	for (int i = StartRoadCells.X; i <= EndRoadCells.X; i++)
	{
		for (int j = StartRoadCells.Y; j <= EndRoadCells.Y; j++)
		{
			FLevelGenRoadBase& CurrentRoad = *DataStorage.RoadsMap.GetCell(i, j);
			if (CurrentRoad.GetRoadState() == ERoadState::CoordinateCheced)
			{
				CurrentRoad.CheckMainRoadAndRoadCrossings(DataStorage);
				
				CurrentRoad.ConnectRoadToLevelCells(DataStorage);

			}
			
		}
	}
	
	
}




void FDataFillerRoadMap::CreateRoads(FVector2D StartRoadCells, FVector2D EndRoadCells)
{
	auto Filler = [&](int X, int Y)
	{
		
		if ((IsNumberEven(X) == IsNumberEven(Y)))
		{
			return std::unique_ptr<FLevelGenRoadBase>(new FLevelGenRoadHorizontal(LevelSettings.RoadSize, false, FVector2D(X, Y), DataStorage.RoadsMap, LevelSettings, LevelGenerator));
		}
		else
		{
			
			return std::unique_ptr<FLevelGenRoadBase>(new FLevelGenRoadVertical(LevelSettings.RoadSize, false, FVector2D(X, Y), DataStorage.RoadsMap, LevelSettings, LevelGenerator));
		}


	};

	DataStorage.RoadsMap.FillRange(StartRoadCells, EndRoadCells, Filler);
}

void FDataFillerRoadMap::CheckRoadPlase(FVector2D StartRoadCells, FVector2D EndRoadCells)
{
	
	for (int i = StartRoadCells.X; i <= EndRoadCells.X; i++)
	{
		for (int j = StartRoadCells.Y; j <= EndRoadCells.Y; j++)
		{
			DataStorage.RoadsMap.GetCell(i, j)->CheckRoadPlase();
		}
	}
	
}

int FDataFillerRoadMap::GetDeltaOffsetGenerateArea() const
{ 
	return  LevelSettings.RoadFrequency;
}


//******************************************************
//FDataFillerBildingsZoneCreater
//*******************************************************


void FDataFillerBildingsZoneCreater::FillData(FVector2D Start, FVector2D End)
{
	FVector2D StartBilldingCells = FVector2D(RoundNumber(Start.X / LevelSettings.RoadFrequency, ERoundType::Up), RoundNumber(Start.Y / LevelSettings.RoadFrequency, ERoundType::Up));
	FVector2D EndBildingCells = FVector2D(RoundNumber(End.X / LevelSettings.RoadFrequency, ERoundType::Up), RoundNumber(End.Y / LevelSettings.RoadFrequency, ERoundType::Up));

	CreateBildings(StartBilldingCells, EndBildingCells);
	
	CreateBildingsZoneLinks(StartBilldingCells, EndBildingCells);
};


void FDataFillerBildingsZoneCreater::CreateBildings(FVector2D StartBilldingCells, FVector2D EndBildingCells)
{
	auto Filler = [&](int X, int Y)
	{
		
		int SecondCor = IsNumberEven(Y - X) ? Y + 1 : Y;
		int ThirdCor = IsNumberEven(X - Y) ? X : X + 1;
		int SixthCor = IsNumberEven(Y - X) ? Y : Y + 1;
		int SeventhCor = IsNumberEven(X - Y) ? X + 1 : X;
		FVector2D StartBilding;
		FVector2D EndBilding;


		if (DataStorage.RoadsMap.IsCellExist(X, SecondCor) && DataStorage.RoadsMap.GetCell(X, SecondCor)->GetRoadState() == ERoadState::ConectedToLevelMap)
		{
			StartBilding.X = DataStorage.RoadsMap.GetCell(X, SecondCor)->GetCoordinate() + DataStorage.RoadsMap.GetCell(X, SecondCor)->GetSize();
		}
		else if (DataStorage.RoadsMap.IsCellExist(X, SecondCor) && DataStorage.RoadsMap.GetCell(X, SecondCor)->GetRoadState() == ERoadState::UnPlaseble)
		{
			StartBilding.X = DataStorage.RoadsMap.GetCell(X, SecondCor)->GetCoordinate();
		}
		else
		{
			return std::unique_ptr<FLevelGenBildingZoneBase>();
		}

		
		if (DataStorage.RoadsMap.IsCellExist(ThirdCor, Y) && DataStorage.RoadsMap.GetCell(ThirdCor, Y)->GetRoadState() == ERoadState::ConectedToLevelMap)
		{
			StartBilding.Y = DataStorage.RoadsMap.GetCell(ThirdCor, Y)->GetCoordinate() + DataStorage.RoadsMap.GetCell(ThirdCor, Y)->GetSize();
		}
		else if (DataStorage.RoadsMap.IsCellExist(ThirdCor, Y) && DataStorage.RoadsMap.GetCell(ThirdCor, Y)->GetRoadState() == ERoadState::UnPlaseble)
		{

			StartBilding.Y = DataStorage.RoadsMap.GetCell(ThirdCor, Y)->GetCoordinate();
			
		}
		else
		{
			return std::unique_ptr<FLevelGenBildingZoneBase>();
		}

		if (DataStorage.RoadsMap.IsCellExist(X + 1, SixthCor) && DataStorage.RoadsMap.GetCell(X + 1, SixthCor)->GetRoadState() == ERoadState::ConectedToLevelMap ||
			DataStorage.RoadsMap.IsCellExist(X + 1, SixthCor) && DataStorage.RoadsMap.GetCell(X + 1, SixthCor)->GetRoadState() == ERoadState::UnPlaseble)
		{
			EndBilding.X = DataStorage.RoadsMap.GetCell(X + 1, SixthCor)->GetCoordinate();
		}
		else
		{
			
				
			return std::unique_ptr<FLevelGenBildingZoneBase>();
		}

		if (DataStorage.RoadsMap.IsCellExist(SeventhCor, Y + 1) && DataStorage.RoadsMap.GetCell(SeventhCor, Y + 1)->GetRoadState() == ERoadState::ConectedToLevelMap || 
			DataStorage.RoadsMap.IsCellExist(SeventhCor, Y + 1) && DataStorage.RoadsMap.GetCell(SeventhCor, Y + 1)->GetRoadState() == ERoadState::UnPlaseble)
		{

			EndBilding.Y = DataStorage.RoadsMap.GetCell(SeventhCor, Y + 1)->GetCoordinate();
		}
		else
		{
			
			return std::unique_ptr<FLevelGenBildingZoneBase>();
		}

		bool UnPlacebelBilding = (EndBilding.X <= StartBilding.X || EndBilding.Y <= StartBilding.Y) ? true : false;

		int RandNum = LevelGenerator.GetRandomGenerator().GetRandomNumber(0, LevelSettings.FullBildingTowersFrequency);
		
		if (RandNum == LevelSettings.FullBildingTowersFrequency)
		{
			return std::unique_ptr<FLevelGenBildingZoneBase>(new FLevelGenBildingZoneTower(UnPlacebelBilding, StartBilding, EndBilding, DataStorage, LevelSettings, LevelGenerator));
		}
		else
		{
			return std::unique_ptr<FLevelGenBildingZoneBase>(new FLevelGenBildingZone(UnPlacebelBilding, StartBilding, EndBilding, DataStorage, LevelSettings, LevelGenerator));
		}
		

	};

	DataStorage.BildingMap.FillRange(StartBilldingCells, EndBildingCells, Filler);
}

void FDataFillerBildingsZoneCreater::CreateBildingsZoneLinks(FVector2D StartBilldingCells, FVector2D EndBildingCells)
{
	for (int i = StartBilldingCells.X; i <= EndBildingCells.X; i++)
	{
		for (int j = StartBilldingCells.Y; j <= EndBildingCells.Y; j++)
		{
			if (DataStorage.BildingMap.IsCellExist(i, j))
			{
				FLevelGenBildingZoneBase& CurrentCell = *DataStorage.BildingMap.GetCell(i, j);

				CreateLinksForCurrentBildingZone(CurrentCell, FVector2D(i, j), FVector2D(1, 1));
				
				CreateLinksForCurrentBildingZone(CurrentCell, FVector2D(i, j), FVector2D(1, 0));

				CreateLinksForCurrentBildingZone(CurrentCell, FVector2D(i, j), FVector2D(1, -1));

				CreateLinksForCurrentBildingZone(CurrentCell, FVector2D(i, j), FVector2D(0, -1));

				CreateLinksForCurrentBildingZone(CurrentCell, FVector2D(i, j), FVector2D(-1, -1));

				CreateLinksForCurrentBildingZone(CurrentCell, FVector2D(i, j), FVector2D(-1, 0));

				CreateLinksForCurrentBildingZone(CurrentCell, FVector2D(i, j), FVector2D(-1, 1));

				CreateLinksForCurrentBildingZone(CurrentCell, FVector2D(i, j), FVector2D(0, 1));
				
			}
			
		}
	}
}

void FDataFillerBildingsZoneCreater::CreateLinksForCurrentBildingZone(FLevelGenBildingZoneBase& CurrentBildingZone, FVector2D CurrentBildingCoordinate, FVector2D OtherBildingDelta)
{
	if (CurrentBildingZone.GetState() == EBildingZoneState::UnPlasebel) return;

	FVector2D OtherBildingCoordinate = CurrentBildingCoordinate + OtherBildingDelta;

	EDirection XDirection = EDirection::end;
	
	if (OtherBildingDelta.X == 1) XDirection = EDirection::XP;
	else if (OtherBildingDelta.X == -1) XDirection = EDirection::XM;

	EDirection YDirection = EDirection::end;

	if (OtherBildingDelta.Y == 1) YDirection = EDirection::YP;
	else if (OtherBildingDelta.Y == -1) YDirection = EDirection::YM;

	if (DataStorage.BildingMap.IsCellExist(OtherBildingCoordinate))
	{
		std::pair<int, int> IntersectRange;
		FLevelGenBildingZoneBase* OtherBildingZone = DataStorage.BildingMap.GetCell(OtherBildingCoordinate).get();

		

		if (CurrentBildingZone.GetLinks().find(OtherBildingZone) == CurrentBildingZone.GetLinks().end())
		{

			if (OtherBildingZone->GetState() == EBildingZoneState::UnPlasebel)
			{
				CurrentBildingZone.AddLink(OtherBildingZone, std::shared_ptr<FLevelGenBildingZoneLink>());
				return;
			}

			if (XDirection != EDirection::end && GetIntervalIntesect(IntersectRange, CurrentBildingZone.GetCoordinateRange(XDirection), OtherBildingZone->GetCoordinateRange(XDirection)))
			{
				int LinkBeginCoordinate = XDirection == EDirection::XP ? CurrentBildingZone.GetBeginCoordinate().X + CurrentBildingZone.GetSize().X :
					OtherBildingZone->GetBeginCoordinate().X + OtherBildingZone->GetSize().X;

				int LinkSize = XDirection == EDirection::XP ? OtherBildingZone->GetBeginCoordinate().X - LinkBeginCoordinate : CurrentBildingZone.GetBeginCoordinate().X - LinkBeginCoordinate;

				std::shared_ptr<FLevelGenBildingZoneLink> NewLink(new FLevelGenBildingZoneLink(XDirection, LinkBeginCoordinate, LinkSize, IntersectRange, CurrentBildingZone.GetGraphLayersNum()));

				CurrentBildingZone.AddLink(OtherBildingZone, NewLink);
				OtherBildingZone->AddLink(&CurrentBildingZone, NewLink);
			}
			else if (YDirection != EDirection::end && GetIntervalIntesect(IntersectRange, CurrentBildingZone.GetCoordinateRange(YDirection), OtherBildingZone->GetCoordinateRange(YDirection)))
			{
				int LinkBeginCoordinate = YDirection == EDirection::YP ? CurrentBildingZone.GetBeginCoordinate().Y + CurrentBildingZone.GetSize().Y :
					OtherBildingZone->GetBeginCoordinate().Y + OtherBildingZone->GetSize().Y;

				int LinkSize = YDirection == EDirection::YP ? OtherBildingZone->GetBeginCoordinate().Y - LinkBeginCoordinate : CurrentBildingZone.GetBeginCoordinate().Y - LinkBeginCoordinate;

				std::shared_ptr<FLevelGenBildingZoneLink> NewLink(new FLevelGenBildingZoneLink(YDirection, LinkBeginCoordinate, LinkSize, IntersectRange, CurrentBildingZone.GetGraphLayersNum()));

				CurrentBildingZone.AddLink(OtherBildingZone, NewLink);
				OtherBildingZone->AddLink(&CurrentBildingZone, NewLink);
			}
			else
			{
				CurrentBildingZone.AddLink(OtherBildingZone, std::shared_ptr<FLevelGenBildingZoneLink>());
				OtherBildingZone->AddLink(&CurrentBildingZone, std::shared_ptr<FLevelGenBildingZoneLink>());
			}
		}
	}
}



//******************************************************
//FDataFillerGraph
//*******************************************************



void FDataFillerGraph::FillData(FVector2D Start, FVector2D End)
{
	
	FVector2D StartBilldingCells = FVector2D(RoundNumber(Start.X / LevelSettings.RoadFrequency, ERoundType::Up), RoundNumber(Start.Y / LevelSettings.RoadFrequency, ERoundType::Up));
	FVector2D EndBildingCells = FVector2D(RoundNumber(End.X / LevelSettings.RoadFrequency, ERoundType::Up), RoundNumber(End.Y / LevelSettings.RoadFrequency, ERoundType::Up));

	

	for (int i = StartBilldingCells.X; i <= EndBildingCells.X; i++)
	{
		for (int j = StartBilldingCells.Y; j <= EndBildingCells.Y; j++)
		{
			if (DataStorage.BildingMap.IsCellExist(i, j))
			{
				FLevelGenBildingZoneBase& CurrentBildingZone = *DataStorage.BildingMap.GetCell(i, j);
				if (CurrentBildingZone.GetLinks().size() >= 8 && CurrentBildingZone.GetState() == EBildingZoneState::Created)
				{
					
					CurrentBildingZone.CreateLinksBetweenBildingZone();

					LinkJointPartsToRooms(CurrentBildingZone);

					CurrentBildingZone.EdgeCut();

					CurrentBildingZone.SetBaseLevelNum();

					CurrentBildingZone.CreateBaseWalls();

					CurrentBildingZone.CreateProceduralFigure();

					CurrentBildingZone.SetState(EBildingZoneState::FlooreAndWallCreated);
					
				}
				
			}
		}
	}
	
}

void FDataFillerGraph::LinkJointPartsToRooms(FLevelGenBildingZoneBase& CurrentBildingZone)
{
	
	for (int i = CurrentBildingZone.GetBeginCoordinate().X; i < CurrentBildingZone.GetBeginCoordinate().X + CurrentBildingZone.GetSize().X; i++)
	{
		for (int j = CurrentBildingZone.GetBeginCoordinate().Y; j < CurrentBildingZone.GetBeginCoordinate().Y + CurrentBildingZone.GetSize().Y; j++)
		{
			for (int k = 0; k < DataStorage.LevelMap.GetCell(i, j)->Floors.size(); k++)
			{
				
				for (int l = 0; l < DataStorage.LevelMap.GetCell(i, j)->Floors[k].JointPart.size(); l++)
				{

					if (DataStorage.LevelMap.GetCell(i, j)->Floors[k].JointPart[l])
					{
						DataStorage.LevelMap.GetCell(i, j)->Floors[k].JointPart[l]->SetJointRoom(DataStorage.LevelMap.GetCell(i, j)->Floors[k].PlasedRoom);
					}
				}
				
			}
		}
	}

	for (int m = 0; m < (int)EDirection::end; m++)
	{
		std::vector<FVector2D> WallCellsCoordinate;

		CurrentBildingZone.GetFasedeWallCellsCoordinats(WallCellsCoordinate, (EDirection)m);

		for (auto CurrentCoordinate : WallCellsCoordinate)
		{
			for (int k = 0; k < DataStorage.LevelMap.GetCell(CurrentCoordinate)->Floors.size(); k++)
			{
				for (int l = 0; l < DataStorage.LevelMap.GetCell(CurrentCoordinate)->Floors[k].JointPart.size(); l++)
				{

					if (DataStorage.LevelMap.GetCell(CurrentCoordinate)->Floors[k].JointPart[l])
					{
						DataStorage.LevelMap.GetCell(CurrentCoordinate)->Floors[k].JointPart[l]->SetJointRoom(DataStorage.LevelMap.GetCell(CurrentCoordinate)->Floors[k].PlasedRoom);
					}
				}
			}
		}
	}
	
	
}



//******************************************************
//FDataFillerBildingDisposer
//*******************************************************


void FDataFillerBildingDisposer::FillData(FVector2D Start, FVector2D End)
{
	std::vector<ELevelCellType> IgnoredType;

	std::vector<const FLevelBilding*> Bildings;
	LevelGenerator.GetTowerStorage()->GetBildings(Bildings);

	for (int i = Start.X; i <= End.X; i++)
	{
		for (int j = Start.Y; j <= End.Y; j++)
		{
			if (DataStorage.LevelMap.IsCellExist(i, j))
			{
				FLevelCellData& CurrentCell = *DataStorage.LevelMap.GetCell(i, j);
				if (!CurrentCell.LevelBilding)
				{
					std::shuffle(Bildings.begin(), Bildings.end(), LevelGenerator.GetRandomGenerator().GetGenerator());
					for (auto CurrentBilding : Bildings)
					{
						bool On90Deg = LevelGenerator.GetRandomGenerator().GetRandomNumber(0, 1);


						if (CurrentBilding->IsPlaceFit(FVector2D(i, j), On90Deg ? EYawTurn::HalfPi : EYawTurn::Zero, DataStorage))
						{
							CreateBilding(*CurrentBilding, FVector2D(i, j), On90Deg);
						}
						else
						{
							On90Deg = !On90Deg;

							if (CurrentBilding->IsPlaceFit(FVector2D(i, j), On90Deg ? EYawTurn::HalfPi : EYawTurn::Zero, DataStorage))
							{
								CreateBilding(*CurrentBilding, FVector2D(i, j), On90Deg);
							}

						}
					}
				}
			}
			
		}
	}
}


void FDataFillerBildingDisposer::CreateBilding(const FLevelBilding& Bilding, FVector2D Coordinate, bool On90Deg)
{
	EYawTurn CurrentYaw;
	
	if (On90Deg)
	{
		CurrentYaw = LevelGenerator.GetRandomGenerator().GetRandomNumber(0, 1) ? EYawTurn::HalfPi : EYawTurn::PiAndHalf;
	}
	else
	{
		CurrentYaw = LevelGenerator.GetRandomGenerator().GetRandomNumber(0, 1) ? EYawTurn::Zero : EYawTurn::Pi;
	}

	FPlacedLevelBilding* NewBilding = new FPlacedLevelBilding(&Bilding, Coordinate, CurrentYaw, DataStorage, LevelSettings);

}


//******************************************************
//FDataFillerCarTrack
//*******************************************************

void FDataFillerCarTrack::Generate(FVector2D CenterCoordinate, int Radius, AVirtualSpawner* VirtualSpawner)
{
	std::unique_lock<std::mutex> DataStorageLock(DataStorage.DataLock);

	int BildRadius = RoundNumber(Radius / LevelSettings.RoadFrequency, ERoundType::Mathematical);

	FVector2D BildCenterCoordinate = CenterCoordinate / LevelSettings.RoadFrequency;

	BildCenterCoordinate.X = RoundNumber(BildCenterCoordinate.X, ERoundType::Mathematical);
	BildCenterCoordinate.Y = RoundNumber(BildCenterCoordinate.Y, ERoundType::Mathematical);

	FillData(CenterCoordinate - FVector2D(Radius, Radius), CenterCoordinate + FVector2D(Radius, Radius));

	SetSpawnCarTracks(BildCenterCoordinate, BildRadius);
}

void FDataFillerCarTrack::FillData(FVector2D Start, FVector2D End)
{
	FVector2D StartRoadCells = FVector2D(RoundNumber(Start.X / LevelSettings.RoadFrequency), RoundNumber(Start.Y / LevelSettings.RoadFrequency));
	FVector2D EndRoadCells = FVector2D(RoundNumber(End.X / LevelSettings.RoadFrequency), RoundNumber(End.Y / LevelSettings.RoadFrequency));

	auto Filler = [&](int X, int Y)
	{
		std::unique_ptr<std::vector<FCartTracsContainer>> NewCantainer(new std::vector<FCartTracsContainer>());

		NewCantainer->resize(LevelSettings.HoverCarTrackLayers.Num());

		return NewCantainer;
		
	};

	DataStorage.CarTrackMap.FillRange(StartRoadCells - FVector2D(1, 1), EndRoadCells + FVector2D(1, 1), Filler);

	CreateTrackNods(StartRoadCells, EndRoadCells);
	CreateTrackLinks(StartRoadCells, EndRoadCells);
}

void FDataFillerCarTrack::CreateTrackNods(FVector2D StartRoadCells, FVector2D EndRoadCells)
{
	for (int i = StartRoadCells.X; i <= EndRoadCells.X; i++)
	{
		for (int j = StartRoadCells.Y; j <= EndRoadCells.Y; j++)
		{
			if (DataStorage.RoadsMap.IsCellExist(i, j) && DataStorage.RoadsMap.GetCell(i, j)->GetRoadState() == ERoadState::ConectedToLevelMap)
			{
				for (int k = 0; k < LevelSettings.HoverCarTrackLayers.Num(); k++)
				{
					CreateTrackLayerNods(i, j, k, LevelSettings.HoverCarTrackLayers[k].TrackHeight, LevelSettings.HoverCarTrackLayers[k].TrackDirection);
				}
			}
			
			
		}
	}
}

void FDataFillerCarTrack::CreateTrackLayerNods(int X, int Y, int LayerNum, int TrackHeight, EHoverCarTrackDirection TrackDirection)
{
	FLevelGenRoadBase& CurrentRoad = *DataStorage.RoadsMap.GetCell(X, Y);
	FCartTracsContainer& TracksContainer = (*DataStorage.CarTrackMap.GetCell(X, Y))[LayerNum];


	if (TracksContainer.CarTrackNodes.size() == 0)
	{
		std::vector<FRoadCrossing> Crossings;

		if (CurrentRoad.GetRoadEdges(Crossings))
		{
			std::vector<FVector> AbsoluteRoadCoordinate3D;
			std::vector<FVector> AbsoluteCrossingCoordinate3D;

			for (auto CurentCrossing : Crossings)
			{
				if (CurentCrossing.Type == ERoadCrossingType::RoadEdge)
				{
					AbsoluteRoadCoordinate3D.push_back(FVector(CurentCrossing.Coordinate * LevelSettings.CellSize, TrackHeight));
				}
				else
				{
					AbsoluteCrossingCoordinate3D.push_back(FVector(CurentCrossing.Coordinate * LevelSettings.CellSize, TrackHeight));
				}

			}

			for (int i = 0; i < AbsoluteRoadCoordinate3D.size() / 2; i++)
			{
				if (AbsoluteRoadCoordinate3D[i * 2] == AbsoluteRoadCoordinate3D[i * 2 + 1])
				{
					if (CurrentRoad.GetDirection() == ERoadDirection::Horizontal)
					{
						AbsoluteRoadCoordinate3D[i * 2].X -= 1;
					}
					else
					{
						AbsoluteRoadCoordinate3D[i * 2].Y -= 1;
					}
				}
				std::shared_ptr<FCarTrackNode> NewTrackNode(new FCarTrackNode(TrackDirection,
					std::make_pair(AbsoluteRoadCoordinate3D[i * 2], AbsoluteCrossingCoordinate3D[i]),
					std::make_pair(AbsoluteRoadCoordinate3D[i * 2 + 1], AbsoluteCrossingCoordinate3D[i + 1]), &CurrentRoad, LevelSettings, DataStorage));


				for (int j = 0; j < NewTrackNode->NodeJointCoordinats.size(); j++)
				{
					TracksContainer.CarTrackNodesJointCoordinats.insert(std::make_pair(NewTrackNode->NodeJointCoordinats[j], NewTrackNode.get()));
				}

				TracksContainer.CarTrackNodes.push_back(NewTrackNode);
			}
		}
	}

}

void FDataFillerCarTrack::CreateTrackLinks(FVector2D StartRoadCells, FVector2D EndRoadCells)
{
	for (int i = StartRoadCells.X; i <= EndRoadCells.X; i++)
	{
		for (int j = StartRoadCells.Y; j <= EndRoadCells.Y; j++)
		{
			if (DataStorage.RoadsMap.IsCellExist(i, j) && DataStorage.RoadsMap.GetCell(i, j)->GetRoadState() == ERoadState::ConectedToLevelMap)
			{
				for (int k = 0; k < LevelSettings.HoverCarTrackLayers.Num(); k++)
				{
					CreateTrackLayerLinks(i, j, k);
				}
			}
			
		}
	}
}


void FDataFillerCarTrack::CreateTrackLayerLinks(int X, int Y, int LayerNum)
{
	if (DataStorage.RoadsMap.IsCellExist(X, Y) && DataStorage.CarTrackMap.IsCellExist(X, Y))
	{
		
		FLevelGenRoadBase& CurrentRoad = *DataStorage.RoadsMap.GetCell(X, Y);
		FCartTracsContainer& TracksContainer = (*DataStorage.CarTrackMap.GetCell(X, Y))[LayerNum];

		std::pair<FCartTracsContainer*, FCartTracsContainer*> NearestTracksOppositDirection = GetNearesTracksOppositDirection(X, Y, LayerNum, CurrentRoad);

		if (NearestTracksOppositDirection.first && NearestTracksOppositDirection.second)
		{
			FindPossibleTrackLinksForRoads(TracksContainer, *NearestTracksOppositDirection.first);
			FindPossibleTrackLinksForRoads(TracksContainer, *NearestTracksOppositDirection.second);
		}
		else
		{
			return;
		}

		int A = TracksContainer.CarTrackNodes.size() - 1; //becose error!!!

		for (int i = 0; i < A; i++) //Create links inaside road
		{
			CreateCarTrackLink(TracksContainer.CarTrackNodes[i].get(), TracksContainer.CarTrackNodes[i + 1].get(), TracksContainer.CarTrackNodes[i]->NodeJointCoordinats[(int)ETrackCoordinatAppointment::End]);
			
		}

		std::pair<FCartTracsContainer*, FCartTracsContainer*> NearestTracksSameDirection = GetNearesTracksSameDirection(X, Y, LayerNum, CurrentRoad);

		
		if (NearestTracksSameDirection.first && NearestTracksSameDirection.first->CarTrackNodes.size() > 0 && TracksContainer.CarTrackNodes.size() > 0)
		{
			std::multimap<FVector, FCarTrackNode*, FVectorLess>::iterator It = NearestTracksSameDirection.first->CarTrackNodesJointCoordinats.find(TracksContainer.CarTrackNodes[0]->NodeJointCoordinats[(int)ETrackCoordinatAppointment::Start]);

			if (It != NearestTracksSameDirection.first->CarTrackNodesJointCoordinats.end())
			{
				CreateCarTrackLink(It->second, TracksContainer.CarTrackNodes[0].get(), TracksContainer.CarTrackNodes[0]->NodeJointCoordinats[(int)ETrackCoordinatAppointment::Start]);
			}
		}
	}
	
	
}


std::pair<FCartTracsContainer*, FCartTracsContainer*> FDataFillerCarTrack::GetNearesTracksOppositDirection(int X, int Y, int LayerNum, FLevelGenRoadBase& CurrentRoad)
{
	std::pair<FCartTracsContainer*, FCartTracsContainer*> NearestTracksSameDirection(nullptr, nullptr);

	if (CurrentRoad.GetDirection() == ERoadDirection::Horizontal)
	{
		if (DataStorage.CarTrackMap.IsCellExist(X - 1, Y))
		{
			NearestTracksSameDirection.first = &(*DataStorage.CarTrackMap.GetCell(X - 1, Y))[LayerNum];
		}
		if (DataStorage.CarTrackMap.IsCellExist(X + 1, Y))
		{
			NearestTracksSameDirection.second = &(*DataStorage.CarTrackMap.GetCell(X + 1, Y))[LayerNum];
		}
	}
	else if (CurrentRoad.GetDirection() == ERoadDirection::Vertical)
	{
		if (DataStorage.CarTrackMap.IsCellExist(X, Y - 1))
		{
			NearestTracksSameDirection.first = &(*DataStorage.CarTrackMap.GetCell(X, Y - 1))[LayerNum];
		}
		if (DataStorage.CarTrackMap.IsCellExist(X, Y + 1))
		{
			NearestTracksSameDirection.second = &(*DataStorage.CarTrackMap.GetCell(X, Y + 1))[LayerNum];
		}
	}
	else throw;

	return NearestTracksSameDirection;
}

std::pair<FCartTracsContainer*, FCartTracsContainer*> FDataFillerCarTrack::GetNearesTracksSameDirection(int X, int Y, int LayerNum, FLevelGenRoadBase& CurrentRoad)
{
	std::pair<FCartTracsContainer*, FCartTracsContainer*> NearestTracksSameDirection(nullptr, nullptr);

	if (CurrentRoad.GetDirection() == ERoadDirection::Horizontal)
	{
		if (DataStorage.CarTrackMap.IsCellExist(X - 2, Y))
		{
			NearestTracksSameDirection.first = &(*DataStorage.CarTrackMap.GetCell(X - 2, Y))[LayerNum];
		}
		if (DataStorage.CarTrackMap.IsCellExist(X + 2, Y))
		{
			NearestTracksSameDirection.second = &(*DataStorage.CarTrackMap.GetCell(X + 2, Y))[LayerNum];
		}
	}
	else if (CurrentRoad.GetDirection() == ERoadDirection::Vertical)
	{
		if (DataStorage.CarTrackMap.IsCellExist(X, Y - 2))
		{
			NearestTracksSameDirection.first = &(*DataStorage.CarTrackMap.GetCell(X, Y - 2))[LayerNum];
		}
		if (DataStorage.CarTrackMap.IsCellExist(X, Y + 2))
		{
			NearestTracksSameDirection.second = &(*DataStorage.CarTrackMap.GetCell(X, Y + 2))[LayerNum];
		}
	}
	else throw;

	return NearestTracksSameDirection;
}

void FDataFillerCarTrack::FindPossibleTrackLinksForRoads(FCartTracsContainer& TracksContainer, FCartTracsContainer& NeareRoad)
{
	
	for (auto CurrentNearsRoadNode : NeareRoad.CarTrackNodes)
	{
		
		for (auto CurrentNearsRoadJointCoordinate : CurrentNearsRoadNode->NodeJointCoordinats)
		{
			
			std::pair<std::multimap<FVector, FCarTrackNode*, FVectorLess>::iterator, std::multimap<FVector, FCarTrackNode*, FVectorLess>::iterator> CurrentRoadJointCoordinats_ItRange =
				TracksContainer.CarTrackNodesJointCoordinats.equal_range(CurrentNearsRoadJointCoordinate);

			for (auto CurrentRoadJointCoordinats_It = CurrentRoadJointCoordinats_ItRange.first; CurrentRoadJointCoordinats_It != CurrentRoadJointCoordinats_ItRange.second;CurrentRoadJointCoordinats_It++)
			{
				
				CreateCarTrackLink(CurrentRoadJointCoordinats_It->second, CurrentNearsRoadNode.get(), CurrentNearsRoadJointCoordinate);
				
			}

		}

	}

	
}

void FDataFillerCarTrack::CreateCarTrackLink(FCarTrackNode* FirstCarTrackNode, FCarTrackNode* SecondCarTrackNode, FVector JointCoordinate)
{
	const std::map<const FCarTrackNode*, std::shared_ptr<FCarTrackLink>>& FirstTrackLinks = FirstCarTrackNode->GetLinks();
	const std::map<const FCarTrackNode*, std::shared_ptr<FCarTrackLink>>& SecondTrackLinks = SecondCarTrackNode->GetLinks();

	if (!(FirstTrackLinks.find(SecondCarTrackNode) != FirstTrackLinks.end() || SecondTrackLinks.find(FirstCarTrackNode) != SecondTrackLinks.end()))
	{
		try
		{
			std::shared_ptr<FCarTrackLink> NewLink(new FCarTrackLink(FirstCarTrackNode, SecondCarTrackNode, JointCoordinate, LevelSettings, DataStorage));
			NewLink->GetStartNode()->AddLink(std::make_pair(NewLink->GetEndNode(), NewLink));
		}
		catch (FString error)
		{
			return;
		}

	}
}






void FDataFillerCarTrack::SetSpawnCarTracks(FVector2D CenterCoordinate, int Radius)
{
	DataStorage.ActiveHoverCarTrackCenterCoordinate = CenterCoordinate;
	DataStorage.ActiveHoverCarTrackRadius = Radius;

	for (int k = 0; k < LevelSettings.HoverCarTrackLayers.Num(); k++)
	{
		SetSpawnCarTrackForLayer(CenterCoordinate, Radius, k);
	}
}

void FDataFillerCarTrack::SetSpawnCarTrackForLayer(FVector2D CenterCoordinate, int Radius, int LayerNum)
{
	DataStorage.SpawnCarTrack[LayerNum].clear();

	//std::pair<int, int> YCircleCoord;

	if (LevelSettings.HoverCarTrackLayers[LayerNum].TrackDirection == EHoverCarTrackDirection::Pozitive)
	{
		for (int i = CenterCoordinate.X - Radius; i <= CenterCoordinate.X; i++)
		{
			
			//YCircleCoord = FLevelBilderBase::GetCircleCoordinateFromX(i, CenterCoordinate, Radius);

			FCartTracsContainer& CurrentTrack1 = (*DataStorage.CarTrackMap.GetCell(i, CenterCoordinate.Y - Radius))[LayerNum];

			for (auto CurrentCarNod = CurrentTrack1.CarTrackNodes.begin(); CurrentCarNod != CurrentTrack1.CarTrackNodes.end(); CurrentCarNod++)
			{
				DataStorage.SpawnCarTrack[LayerNum].push_back((*CurrentCarNod).get());
			}

			FCartTracsContainer& CurrentTrack2 = (*DataStorage.CarTrackMap.GetCell(i, CenterCoordinate.Y + Radius))[LayerNum];
			for (auto CurrentCarNod = CurrentTrack2.CarTrackNodes.begin(); CurrentCarNod != CurrentTrack2.CarTrackNodes.end(); CurrentCarNod++)
			{
				DataStorage.SpawnCarTrack[LayerNum].push_back((*CurrentCarNod).get());
			}

		}
		for (int i = CenterCoordinate.X; i <= CenterCoordinate.X + Radius; i++)
		{
			//YCircleCoord = FLevelBilderBase::GetCircleCoordinateFromX(i, CenterCoordinate, Radius);

			FCartTracsContainer& CurrentTrack = (*DataStorage.CarTrackMap.GetCell(i, CenterCoordinate.Y - Radius))[LayerNum];
			for (auto CurrentCarNod = CurrentTrack.CarTrackNodes.begin(); CurrentCarNod != CurrentTrack.CarTrackNodes.end(); CurrentCarNod++)
			{
				DataStorage.SpawnCarTrack[LayerNum].push_back((*CurrentCarNod).get());
			}
		}
	}
	else
	{
		for (int i = CenterCoordinate.X; i <= CenterCoordinate.X + Radius; i++)
		{
			//YCircleCoord = FLevelBilderBase::GetCircleCoordinateFromX(i, CenterCoordinate, Radius);

			FCartTracsContainer& CurrentTrack1 = (*DataStorage.CarTrackMap.GetCell(i, CenterCoordinate.Y + Radius))[LayerNum];
			for (auto CurrentCarNod = CurrentTrack1.CarTrackNodes.begin(); CurrentCarNod != CurrentTrack1.CarTrackNodes.end(); CurrentCarNod++)
			{
				DataStorage.SpawnCarTrack[LayerNum].push_back((*CurrentCarNod).get());
			}

			FCartTracsContainer& CurrentTrack2 = (*DataStorage.CarTrackMap.GetCell(i, CenterCoordinate.Y - Radius))[LayerNum];
			for (auto CurrentCarNod = CurrentTrack2.CarTrackNodes.begin(); CurrentCarNod != CurrentTrack2.CarTrackNodes.end(); CurrentCarNod++)
			{
				DataStorage.SpawnCarTrack[LayerNum].push_back((*CurrentCarNod).get());
			}

		}
		for (int i = CenterCoordinate.X - Radius; i <= CenterCoordinate.X; i++)
		{
			//YCircleCoord = FLevelBilderBase::GetCircleCoordinateFromX(i, CenterCoordinate, Radius);

			FCartTracsContainer& CurrentTrack = (*DataStorage.CarTrackMap.GetCell(i, CenterCoordinate.Y + Radius))[LayerNum];
			for (auto CurrentCarNod = CurrentTrack.CarTrackNodes.begin(); CurrentCarNod != CurrentTrack.CarTrackNodes.end(); CurrentCarNod++)
			{
				DataStorage.SpawnCarTrack[LayerNum].push_back((*CurrentCarNod).get());
			}
		}
	}

	
}

