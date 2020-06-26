// Copyright 2018 Pavlov Dmitriy
#include "LevelGenRoad.h"
#include <algorithm>
#include "LevelCell.h"
#include "LevelGenerator.h"
#include "DataStorage.h"
#include "DataFillers.h"



//......................................
//FLevelGenRoadBase
//.......................................


bool FLevelGenRoadBase::GetBoundaryForRoad(std::pair<int, int>& Coordinates) const
{
	
	std::pair<FLevelGenRoadBase*, FLevelGenRoadBase*> BoundaryRoads = GetNearestRoadsOppositDirection();


	if (BoundaryRoads.first && BoundaryRoads.second)
	{
		Coordinates.first = BoundaryRoads.first->RoadCoordinate + BoundaryRoads.first->GetSize();
		Coordinates.second = BoundaryRoads.second->RoadCoordinate - 1;
		return true;
	}
	else return false;

}


void FLevelGenRoadBase::CheckRoadPlase()
{
	if (GetRoadState() != ERoadState::Created)  return;

	if (IsMainRoad())
	{
		SetRoadState(ERoadState::CoordinateCheced);
		return;
	}
	

	std::pair<int, int> RoadCoordinatRange;
	
	int MinCoordinate = 0;
	int MaxCoordinate = 0;

	if (!GetRangeCoordinatForRoad(RoadCoordinatRange)) return;
	
	int MinBildingZoneSizeCheck = LevelSettings.MinBildingZoneSize + LevelSettings.RoadSize;
	
	MinCoordinate = RoadCoordinatRange.first + MinBildingZoneSizeCheck;
	MaxCoordinate = RoadCoordinatRange.second - MinBildingZoneSizeCheck;

	if (MinCoordinate < MaxCoordinate)
	{
		SetCoordinate(LevelGenerator.GetRandomGenerator().GetRandomNumber(MinCoordinate, MaxCoordinate));
		
	}
	else if (MinCoordinate == MaxCoordinate)
	{
		SetCoordinate(MinCoordinate);
	}
	else
	{
		MinCoordinate = -MinBildingZoneSizeCheck;
		SetCoordinate(MinCoordinate);
		SetRoadState(ERoadState::UnPlaseble);
		return;
	}
	

	SetRoadState(ERoadState::CoordinateCheced);

}

bool FLevelGenRoadBase::GetRangeCoordinatForRoad(std::pair<int, int>& CoordinateRange) const
{
	FVector2D RoadNumber_loc = GetRoadNumber();

	FVector2D RoadCoordinate1;
	FVector2D RoadCoordinate2;

	switch (GetDirection())
	{
	case ERoadDirection::Horizontal:

		RoadCoordinate1 = FVector2D(RoadNumber_loc.X - 1, RoadNumber_loc.Y);
		RoadCoordinate2 = FVector2D(RoadNumber_loc.X + 1, RoadNumber_loc.Y);

		break;

	case ERoadDirection::Vertical:

		RoadCoordinate1 = FVector2D(RoadNumber_loc.X, RoadNumber_loc.Y - 1);
		RoadCoordinate2 = FVector2D(RoadNumber_loc.X, RoadNumber_loc.Y + 1);

		break;
	}


	return GetRangeCoordinatForRoadFromRoadsCoordinats(CoordinateRange, RoadCoordinate1, RoadCoordinate2);

}

bool FLevelGenRoadBase::GetRangeCoordinatForRoadFromRoadsCoordinats(std::pair<int, int>& CoordinateRange, FVector2D RoadCoordinate1, FVector2D RoadCoordinate2) const
{
	std::pair<int, int> MinusOneRange(0, 0);
	std::pair<int, int> PlusOneRange(0, 0);

	std::pair<FLevelGenRoadBase*, FLevelGenRoadBase*> OppositRods;



	if (RoadsMap.IsCellExist(RoadCoordinate1))
	{
		OppositRods = RoadsMap.GetCell(RoadCoordinate1)->GetNearestRoadsOppositDirection();

		if (OppositRods.first)
		{
			MinusOneRange.first = OppositRods.first->GetCoordinate() + OppositRods.first->GetSize();

		}
		else return false;

		if (OppositRods.second)
		{
			MinusOneRange.second = OppositRods.second->GetCoordinate() - 1;

		}
		else return false;
	}
	else return false;

	if (RoadsMap.IsCellExist(RoadCoordinate2))
	{
		OppositRods = RoadsMap.GetCell(RoadCoordinate2)->GetNearestRoadsOppositDirection();

		if (OppositRods.first)
		{
			PlusOneRange.first = OppositRods.first->GetCoordinate() + OppositRods.first->GetSize();

		}
		else return false;

		if (OppositRods.second)
		{
			PlusOneRange.second = OppositRods.second->GetCoordinate() - 1;
		}
		else return false;

	}
	else return false;



	if (GetIntervalIntesect(CoordinateRange, MinusOneRange, PlusOneRange)) return true;
	else return false;
}

//......................................
//FLevelGenRoadHorizontal
//.......................................

FLevelGenRoadHorizontal::FLevelGenRoadHorizontal(int Coordinate, int Size, bool IsMainRoad, FVector2D RoadNumber, const TLevelMap2D<FLevelGenRoadBase>& RoadsMap,
	const FLevelGeneratorSettings& LevelSettings, const ALevelGenerator& LevelGenerator) : FLevelGenRoadBase(Size, IsMainRoad, RoadNumber, RoadsMap, LevelSettings, LevelGenerator)
{
	SetCoordinate(Coordinate);
}

FLevelGenRoadHorizontal::FLevelGenRoadHorizontal(int Size, bool IsMainRoad, FVector2D RoadNumber, const TLevelMap2D<FLevelGenRoadBase>& RoadsMap,
	const FLevelGeneratorSettings& LevelSettings, const ALevelGenerator& LevelGenerator) : FLevelGenRoadBase(Size, IsMainRoad, RoadNumber, RoadsMap, LevelSettings, LevelGenerator)
{
	SetCoordinate(RoadNumber.Y * LevelSettings.RoadFrequency);
}



void FLevelGenRoadHorizontal::ConnectRoadToLevelCells(FDataStorage& DataStorage)
{
	if (GetRoadState() != ERoadState::CoordinateCheced) return;

	std::pair<int, int> BoundaryCoordinate;
	
	if (!GetBoundaryForRoad(BoundaryCoordinate)) return;
	
	bool RoadConnect = true;
	for (int i = BoundaryCoordinate.first; i <= BoundaryCoordinate.second; i++)
	{
		for (int j = GetCoordinate(); j < GetCoordinate() + GetSize(); j++)
		{
			if (DataStorage.LevelMap.IsCellExist(i, j))
			{
				if (!DataStorage.LevelMap.GetCell(i, j)->CellInst)
				{
					if (IsMainRoad())
					{
						DataStorage.LevelMap.GetCell(i, j)->CellInst =
							std::shared_ptr<FLevelCellBase>(ELevelCellTypeCreate(ELevelCellType::MainRoad, *DataStorage.LevelMap.GetCell(i, GetCoordinate() + j)));
					}
					else
					{
						DataStorage.LevelMap.GetCell(i, j)->CellInst =
							std::shared_ptr<FLevelCellBase>(ELevelCellTypeCreate(ELevelCellType::Road, *DataStorage.LevelMap.GetCell(i, GetCoordinate() + j)));
					}

				}
				else
				{
					if (DataStorage.LevelMap.GetCell(i, j)->CellInst->GetCellType() != ELevelCellType::MainRoad &&
						DataStorage.LevelMap.GetCell(i, j)->CellInst->GetCellType() != ELevelCellType::Road)
					{
						UE_LOG(LogTemp, Error, TEXT("FLevelGenRoadHorizontal::ConnectRoadToLevelCells bad cell type %s"),
							*GetEnumValueAsString<ELevelCellType>("ELevelCellType", DataStorage.LevelMap.GetCell(i, j)->CellInst->GetCellType()));
					}
				}
			}
			else
			{
				RoadConnect = false;
			}

		}

	}

	if (RoadConnect) SetRoadState(ERoadState::ConectedToLevelMap);
		
	
	
}

void FLevelGenRoadHorizontal::CheckMainRoadAndRoadCrossings(FDataStorage& DataStorage)
{
	if (IsMainRoad()) return;

	std::pair<FLevelGenRoadBase*, FLevelGenRoadBase*> CrossingRoads = GetCrossingRoads();

	if (!CrossingRoads.first || !CrossingRoads.second)  return;
	if (CrossingRoads.first->GetCoordinate() == CrossingRoads.second->GetCoordinate() && CrossingRoads.first->IsMainRoad())
	{
		for (int i = CrossingRoads.first->GetCoordinate(); i < CrossingRoads.first->GetCoordinate() + CrossingRoads.first->GetSize(); i++)
		{
			for (int j = GetCoordinate(); j < GetCoordinate() + GetSize(); j++)
			{
				if (DataStorage.LevelMap.IsCellExist(i, j))
				{
					DataStorage.LevelMap.GetCell(i, j)->CellInst =
						std::shared_ptr<FLevelCellBase>(ELevelCellTypeCreate(ELevelCellType::MainRoad, *DataStorage.LevelMap.GetCell(i, j)));
				}
				
			}
		}
	}
	

}



bool FLevelGenRoadHorizontal::GetRoadEdges(std::vector<FRoadCrossing>& Crossings) const
{
	std::pair<FLevelGenRoadBase*, FLevelGenRoadBase*> OppositDirectionRoads = GetNearestRoadsOppositDirection();

	if (!OppositDirectionRoads.first || !OppositDirectionRoads.second) return false;

	std::pair<int, int> BoundaryCoordinate;

	if (!GetBoundaryForRoad(BoundaryCoordinate)) return false;

	std::pair<FLevelGenRoadBase*, FLevelGenRoadBase*> CrossingRoads = GetCrossingRoads();

	if (!CrossingRoads.first || !CrossingRoads.second)  return false;

	Crossings.clear();

	float Y = GetCoordinate() + (GetSize() - 1) * 0.5;

	
	Crossings.push_back(FRoadCrossing(ERoadCrossingType::EndRoad,
			FVector2D(OppositDirectionRoads.first->GetCoordinate() + (OppositDirectionRoads.first->GetSize() - 1) * 0.5, Y), OppositDirectionRoads.first));
		
	

	if (CrossingRoads.first->GetCoordinate() < CrossingRoads.second->GetCoordinate())
	{
		Crossings.push_back(FRoadCrossing(ERoadCrossingType::RoadEdge, FVector2D(BoundaryCoordinate.first - 0.5, Y), nullptr));
		Crossings.push_back(FRoadCrossing(ERoadCrossingType::RoadEdge, FVector2D(CrossingRoads.first->GetCoordinate() - 0.5, Y), nullptr));


		Crossings.push_back(FRoadCrossing(ERoadCrossingType::TCrossing, FVector2D(CrossingRoads.first->GetCoordinate() + (CrossingRoads.first->GetSize() - 1) * 0.5, Y), CrossingRoads.first));


		Crossings.push_back(FRoadCrossing(ERoadCrossingType::RoadEdge, FVector2D(CrossingRoads.first->GetCoordinate() + CrossingRoads.first->GetSize() - 0.5, Y), nullptr));
		Crossings.push_back(FRoadCrossing(ERoadCrossingType::RoadEdge, FVector2D(CrossingRoads.second->GetCoordinate() - 0.5, Y), nullptr));

		Crossings.push_back(FRoadCrossing(ERoadCrossingType::TCrossing, FVector2D(CrossingRoads.second->GetCoordinate() + (CrossingRoads.second->GetSize() - 1) * 0.5, Y), CrossingRoads.second));


		Crossings.push_back(FRoadCrossing(ERoadCrossingType::RoadEdge, FVector2D(CrossingRoads.second->GetCoordinate() + CrossingRoads.second->GetSize() - 0.5, Y), nullptr));
		Crossings.push_back(FRoadCrossing(ERoadCrossingType::RoadEdge, FVector2D(BoundaryCoordinate.second + 0.5, Y), nullptr));


	}
	else if (CrossingRoads.first->GetCoordinate() > CrossingRoads.second->GetCoordinate())
	{
		
		Crossings.push_back(FRoadCrossing(ERoadCrossingType::RoadEdge, FVector2D(BoundaryCoordinate.first - 0.5, Y), nullptr));
		Crossings.push_back(FRoadCrossing(ERoadCrossingType::RoadEdge, FVector2D(CrossingRoads.second->GetCoordinate() - 0.5, Y), nullptr));


		Crossings.push_back(FRoadCrossing(ERoadCrossingType::TCrossing, FVector2D(CrossingRoads.second->GetCoordinate() + (CrossingRoads.second->GetSize() - 1) * 0.5, Y), CrossingRoads.second));

		Crossings.push_back(FRoadCrossing(ERoadCrossingType::RoadEdge, FVector2D(CrossingRoads.second->GetCoordinate() + CrossingRoads.second->GetSize() - 0.5, Y), nullptr));
		Crossings.push_back(FRoadCrossing(ERoadCrossingType::RoadEdge, FVector2D(CrossingRoads.first->GetCoordinate() - 0.5, Y), nullptr));

		Crossings.push_back(FRoadCrossing(ERoadCrossingType::TCrossing, FVector2D(CrossingRoads.first->GetCoordinate() + (CrossingRoads.first->GetSize() - 1) * 0.5, Y), CrossingRoads.first));


		Crossings.push_back(FRoadCrossing(ERoadCrossingType::RoadEdge, FVector2D(CrossingRoads.first->GetCoordinate() + CrossingRoads.first->GetSize() - 0.5, Y), nullptr));
		Crossings.push_back(FRoadCrossing(ERoadCrossingType::RoadEdge, FVector2D(BoundaryCoordinate.second + 0.5, Y), nullptr));

		
	}
	else
	{
		Crossings.push_back(FRoadCrossing(ERoadCrossingType::RoadEdge, FVector2D(BoundaryCoordinate.first - 0.5, Y), nullptr));
		Crossings.push_back(FRoadCrossing(ERoadCrossingType::RoadEdge, FVector2D(CrossingRoads.first->GetCoordinate() - 0.5, Y), nullptr));

		Crossings.push_back(FRoadCrossing(ERoadCrossingType::XCrossing, FVector2D(CrossingRoads.first->GetCoordinate() + (CrossingRoads.first->GetSize() - 1) * 0.5, Y), CrossingRoads.first));

		Crossings.push_back(FRoadCrossing(ERoadCrossingType::RoadEdge, FVector2D(CrossingRoads.first->GetCoordinate() + CrossingRoads.first->GetSize() - 0.5, Y), nullptr));
		Crossings.push_back(FRoadCrossing(ERoadCrossingType::RoadEdge, FVector2D(BoundaryCoordinate.second + 0.5, Y), nullptr));

	}

	
	Crossings.push_back(FRoadCrossing(ERoadCrossingType::EndRoad,
			FVector2D(OppositDirectionRoads.second->GetCoordinate() + (OppositDirectionRoads.second->GetSize() - 1) * 0.5, Y), OppositDirectionRoads.second));

	return true;

}


std::pair<FLevelGenRoadBase*, FLevelGenRoadBase*> FLevelGenRoadHorizontal::GetNearestRoadsOppositDirection() const
{
	std::pair<FLevelGenRoadBase*, FLevelGenRoadBase*> Roads(nullptr, nullptr);

	FVector2D RoadNumber_loc = GetRoadNumber();
	
	if (RoadsMap.IsCellExist(RoadNumber_loc.X - 1, RoadNumber_loc.Y))
	{
		Roads.first = RoadsMap.GetCell(RoadNumber_loc.X - 1, RoadNumber_loc.Y).get();
	}
	if (RoadsMap.IsCellExist(RoadNumber_loc.X + 1, RoadNumber_loc.Y))
	{
		Roads.second = RoadsMap.GetCell(RoadNumber_loc.X + 1, RoadNumber_loc.Y).get();
	}
	
	return Roads;
}

std::pair<FLevelGenRoadBase*, FLevelGenRoadBase*> FLevelGenRoadHorizontal::GetNearestRoadsSameDirection() const
{
	std::pair<FLevelGenRoadBase*, FLevelGenRoadBase*> Roads(nullptr, nullptr);

	FVector2D RoadNumber_loc = GetRoadNumber();

	if (RoadsMap.IsCellExist(RoadNumber_loc.X - 2, RoadNumber_loc.Y))
	{
		Roads.first = RoadsMap.GetCell(RoadNumber_loc.X - 2, RoadNumber_loc.Y).get();
	}
	if (RoadsMap.IsCellExist(RoadNumber_loc.X + 2, RoadNumber_loc.Y))
	{
		Roads.second = RoadsMap.GetCell(RoadNumber_loc.X + 2, RoadNumber_loc.Y).get();
	}

	return Roads;
}

std::pair<FLevelGenRoadBase*, FLevelGenRoadBase*> FLevelGenRoadHorizontal::GetCrossingRoads() const
{
	std::pair<FLevelGenRoadBase*, FLevelGenRoadBase*> Result(std::make_pair(nullptr, nullptr));

	FVector2D RoadNumber_loc = GetRoadNumber();
	
	if (RoadsMap.IsCellExist(RoadNumber_loc.X, RoadNumber_loc.Y - 1))
	{
		Result.first = RoadsMap.GetCell(RoadNumber_loc.X, RoadNumber_loc.Y - 1).get();
	}
	if (RoadsMap.IsCellExist(RoadNumber_loc.X, RoadNumber_loc.Y + 1))
	{
		Result.second = RoadsMap.GetCell(RoadNumber_loc.X, RoadNumber_loc.Y + 1).get();
	}
	
	return Result;
}



//......................................
//FLevelGenRoadVertical
//.......................................

FLevelGenRoadVertical::FLevelGenRoadVertical(int Size, bool IsMainRoad, FVector2D RoadNumber, const TLevelMap2D<FLevelGenRoadBase>& RoadsMap,
	const FLevelGeneratorSettings& LevelSettings, const ALevelGenerator& LevelGenerator) : FLevelGenRoadBase(Size, IsMainRoad, RoadNumber, RoadsMap, LevelSettings, LevelGenerator)
{
	SetCoordinate(RoadNumber.X * LevelSettings.RoadFrequency);
	
}



void FLevelGenRoadVertical::ConnectRoadToLevelCells(FDataStorage& DataStorage)
{
	if (GetRoadState() != ERoadState::CoordinateCheced) return;

	std::pair<int, int> BoundaryCoordinate;

	if (!GetBoundaryForRoad(BoundaryCoordinate)) return;
	
	bool RoadConnect = true;
	for (int i = BoundaryCoordinate.first; i <= BoundaryCoordinate.second; i++)
	{
		for (int j = GetCoordinate(); j < GetCoordinate() + GetSize(); j++)
		{
			if (DataStorage.LevelMap.IsCellExist(j, i))
			{
				if (!DataStorage.LevelMap.GetCell(j, i)->CellInst)
				{
					if (IsMainRoad())
					{
						DataStorage.LevelMap.GetCell(j, i)->CellInst =
							std::shared_ptr<FLevelCellBase>(ELevelCellTypeCreate(ELevelCellType::MainRoad, *DataStorage.LevelMap.GetCell(j, i)));
					}
					else
					{
						DataStorage.LevelMap.GetCell(j, i)->CellInst =
							std::shared_ptr<FLevelCellBase>(ELevelCellTypeCreate(ELevelCellType::Road, *DataStorage.LevelMap.GetCell(j, i)));
					}


				}
				else
				{
					if (DataStorage.LevelMap.GetCell(j, i)->CellInst->GetCellType() != ELevelCellType::MainRoad &&
						DataStorage.LevelMap.GetCell(j, i)->CellInst->GetCellType() != ELevelCellType::Road)
					{
						UE_LOG(LogTemp, Warning, TEXT("FLevelGenRoadHorizontal::ConnectRoadToLevelCells bad cell type %s"),
							*GetEnumValueAsString<ELevelCellType>("ELevelCellType", DataStorage.LevelMap.GetCell(j, i)->CellInst->GetCellType()));
					}
				}
					
			}
			else
			{
				RoadConnect = false;
			}
				
		}
	
			
	}

	if (RoadConnect) SetRoadState(ERoadState::ConectedToLevelMap);
		
	
}

void FLevelGenRoadVertical::CheckMainRoadAndRoadCrossings(FDataStorage& DataStorage)
{
	if (IsMainRoad()) return;

	std::pair<FLevelGenRoadBase*, FLevelGenRoadBase*> CrossingRoads = GetCrossingRoads();

	if (!CrossingRoads.first || !CrossingRoads.second)  return;
	if (CrossingRoads.first->GetCoordinate() == CrossingRoads.second->GetCoordinate() && CrossingRoads.first->IsMainRoad())
	{
		for (int i = GetCoordinate(); i < GetCoordinate() + GetSize(); i++)
		{
			for (int j = CrossingRoads.first->GetCoordinate(); j < CrossingRoads.first->GetCoordinate() + CrossingRoads.first->GetSize(); j++)
			{
				if (DataStorage.LevelMap.IsCellExist(i, j))
				{
					DataStorage.LevelMap.GetCell(i, j)->CellInst =
						std::shared_ptr<FLevelCellBase>(ELevelCellTypeCreate(ELevelCellType::MainRoad, *DataStorage.LevelMap.GetCell(i, j)));
				}
				
			}
		}
	}


}


bool FLevelGenRoadVertical::GetRoadEdges(std::vector<FRoadCrossing>& Crossings) const
{
	std::pair<FLevelGenRoadBase*, FLevelGenRoadBase*> OppositDirectionRoads = GetNearestRoadsOppositDirection();

	if (!OppositDirectionRoads.first || !OppositDirectionRoads.second) return false;

	std::pair<int, int> BoundaryCoordinate;

	if (!GetBoundaryForRoad(BoundaryCoordinate)) return false;

	std::pair<FLevelGenRoadBase*, FLevelGenRoadBase*> CrossingRoads = GetCrossingRoads();

	if (!CrossingRoads.first || !CrossingRoads.second)  return false;

	float X = GetCoordinate() + (GetSize() - 1) * 0.5;

	Crossings.clear();

	
	Crossings.push_back(FRoadCrossing(ERoadCrossingType::EndRoad,
			FVector2D(X, OppositDirectionRoads.first->GetCoordinate() + (OppositDirectionRoads.first->GetSize() - 1) * 0.5), OppositDirectionRoads.first));
	

	if (CrossingRoads.first->GetCoordinate() < CrossingRoads.second->GetCoordinate())
	{
		Crossings.push_back(FRoadCrossing(ERoadCrossingType::RoadEdge, FVector2D(X, BoundaryCoordinate.first - 0.5), nullptr));
		Crossings.push_back(FRoadCrossing(ERoadCrossingType::RoadEdge, FVector2D(X, CrossingRoads.first->GetCoordinate() - 0.5), nullptr));

		
		Crossings.push_back(FRoadCrossing(ERoadCrossingType::TCrossing, FVector2D(X, CrossingRoads.first->GetCoordinate() + (CrossingRoads.first->GetSize() - 1) * 0.5), CrossingRoads.first));


		Crossings.push_back(FRoadCrossing(ERoadCrossingType::RoadEdge, FVector2D(X, CrossingRoads.first->GetCoordinate() + CrossingRoads.first->GetSize() - 0.5), nullptr));
		Crossings.push_back(FRoadCrossing(ERoadCrossingType::RoadEdge, FVector2D(X, CrossingRoads.second->GetCoordinate() - 0.5), nullptr));

		Crossings.push_back(FRoadCrossing(ERoadCrossingType::TCrossing, FVector2D(X, CrossingRoads.second->GetCoordinate() + (CrossingRoads.second->GetSize() - 1) * 0.5), CrossingRoads.second));


		Crossings.push_back(FRoadCrossing(ERoadCrossingType::RoadEdge, FVector2D(X, CrossingRoads.second->GetCoordinate() + CrossingRoads.second->GetSize() - 0.5), nullptr));
		Crossings.push_back(FRoadCrossing(ERoadCrossingType::RoadEdge, FVector2D(X, BoundaryCoordinate.second + 0.5), nullptr));
		
	}
	else if (CrossingRoads.first->GetCoordinate() > CrossingRoads.second->GetCoordinate())
	{
		Crossings.push_back(FRoadCrossing(ERoadCrossingType::RoadEdge, FVector2D(X, BoundaryCoordinate.first - 0.5), nullptr));
		Crossings.push_back(FRoadCrossing(ERoadCrossingType::RoadEdge, FVector2D(X, CrossingRoads.second->GetCoordinate() - 0.5), nullptr));


		Crossings.push_back(FRoadCrossing(ERoadCrossingType::TCrossing, FVector2D(X, CrossingRoads.second->GetCoordinate() + (CrossingRoads.second->GetSize() - 1) * 0.5), CrossingRoads.second));

		Crossings.push_back(FRoadCrossing(ERoadCrossingType::RoadEdge, FVector2D(X, CrossingRoads.second->GetCoordinate() + CrossingRoads.second->GetSize() - 0.5), nullptr));
		Crossings.push_back(FRoadCrossing(ERoadCrossingType::RoadEdge, FVector2D(X, CrossingRoads.first->GetCoordinate() - 0.5), nullptr));

		Crossings.push_back(FRoadCrossing(ERoadCrossingType::TCrossing, FVector2D(X, CrossingRoads.first->GetCoordinate() + (CrossingRoads.first->GetSize() - 1) * 0.5), CrossingRoads.first));


		Crossings.push_back(FRoadCrossing(ERoadCrossingType::RoadEdge, FVector2D(X, CrossingRoads.first->GetCoordinate() + CrossingRoads.first->GetSize() - 0.5), nullptr));
		Crossings.push_back(FRoadCrossing(ERoadCrossingType::RoadEdge, FVector2D(X, BoundaryCoordinate.second + 0.5), nullptr));

	}
	else
	{
		Crossings.push_back(FRoadCrossing(ERoadCrossingType::RoadEdge, FVector2D(X, BoundaryCoordinate.first - 0.5), nullptr));
		Crossings.push_back(FRoadCrossing(ERoadCrossingType::RoadEdge, FVector2D(X, CrossingRoads.first->GetCoordinate() - 0.5), nullptr));

		Crossings.push_back(FRoadCrossing(ERoadCrossingType::XCrossing, FVector2D(X, CrossingRoads.first->GetCoordinate() + (CrossingRoads.first->GetSize() - 1) * 0.5), CrossingRoads.first));

		Crossings.push_back(FRoadCrossing(ERoadCrossingType::RoadEdge, FVector2D(X, CrossingRoads.first->GetCoordinate() + CrossingRoads.first->GetSize() - 0.5), nullptr));
		Crossings.push_back(FRoadCrossing(ERoadCrossingType::RoadEdge, FVector2D(X, BoundaryCoordinate.second + 0.5), nullptr));

	}

		
	
	Crossings.push_back(FRoadCrossing(ERoadCrossingType::EndRoad,
			FVector2D(X, OppositDirectionRoads.second->GetCoordinate() + (OppositDirectionRoads.second->GetSize() - 1) * 0.5), OppositDirectionRoads.second));
	
	return true;
}


std::pair<FLevelGenRoadBase*, FLevelGenRoadBase*> FLevelGenRoadVertical::GetNearestRoadsOppositDirection() const
{
	std::pair<FLevelGenRoadBase*, FLevelGenRoadBase*> Roads(nullptr, nullptr);

	FVector2D RoadNumber_loc = GetRoadNumber();
	
	if (RoadsMap.IsCellExist(RoadNumber_loc.X, RoadNumber_loc.Y - 1))
	{
		Roads.first = RoadsMap.GetCell(RoadNumber_loc.X, RoadNumber_loc.Y - 1).get();
	}
	if (RoadsMap.IsCellExist(RoadNumber_loc.X, RoadNumber_loc.Y + 1))
	{
		Roads.second = RoadsMap.GetCell(RoadNumber_loc.X, RoadNumber_loc.Y + 1).get();
	}
	
	return Roads;
}

std::pair<FLevelGenRoadBase*, FLevelGenRoadBase*> FLevelGenRoadVertical::GetNearestRoadsSameDirection() const
{
	std::pair<FLevelGenRoadBase*, FLevelGenRoadBase*> Roads(nullptr, nullptr);

	FVector2D RoadNumber_loc = GetRoadNumber();

	if (RoadsMap.IsCellExist(RoadNumber_loc.X, RoadNumber_loc.Y - 2))
	{
		Roads.first = RoadsMap.GetCell(RoadNumber_loc.X, RoadNumber_loc.Y - 2).get();
	}
	if (RoadsMap.IsCellExist(RoadNumber_loc.X, RoadNumber_loc.Y + 2))
	{
		Roads.second = RoadsMap.GetCell(RoadNumber_loc.X, RoadNumber_loc.Y + 2).get();
	}

	return Roads;
}

std::pair<FLevelGenRoadBase*, FLevelGenRoadBase*> FLevelGenRoadVertical::GetCrossingRoads() const
{
	std::pair<FLevelGenRoadBase*, FLevelGenRoadBase*> Result(std::make_pair(nullptr, nullptr));

	FVector2D RoadNumber_loc = GetRoadNumber();
	
	if (RoadsMap.IsCellExist(RoadNumber_loc.X - 1, RoadNumber_loc.Y))
	{
		Result.first = RoadsMap.GetCell(RoadNumber_loc.X - 1, RoadNumber_loc.Y).get();
	}
	if (RoadsMap.IsCellExist(RoadNumber_loc.X + 1, RoadNumber_loc.Y))
	{
		Result.second = RoadsMap.GetCell(RoadNumber_loc.X + 1, RoadNumber_loc.Y).get();
	}
	
	return Result;
}
