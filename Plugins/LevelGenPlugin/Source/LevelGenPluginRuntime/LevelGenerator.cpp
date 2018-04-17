// Copyright 2018 Pavlov Dmitriy

#include "LevelGenerator.h"
#include "LevelBilders.h"
#include "EngineUtils.h"
#include "DataFillers.h"
#include "LevelCell.h"
#include <time.h>
#include "LevelGraphNode.h"
#include "Rooms/LevelRooms.h"
#include "Rooms/RoomStorage.h"
#include "Rooms/PlacedLevelRoom.h"
#include "Towers/TowerStorage.h"
#include "LevelGenPluginRuntime_LogCategory.h"
#include "LevelGenSplineMeshActor.h"
#include"VirtualSpawner.h"
#include "HoverCars/HoverCarActor.h"
#include "HoverCars/HoverCar.h"
#include "DataStorage.h"
#include "LevelGenRoad.h"
#include "LevelFloor.h"


//.....................................................
//ALevelGenerator
//.....................................................
ALevelGenerator::ALevelGenerator()
{
	
	PrimaryActorTick.bCanEverTick = true;

	static_assert(EDataFiller::End == (EDataFiller)6, "Fix list of data filler");
	static_assert(ELevelBilder::End == (ELevelBilder)6, "Fix list of level bilder");

	GenerateOrder.Add(FGenerateOrderModule());
	GenerateOrder.Last().AddGenerateObject(EDataFiller::MainRoadMap);
	GenerateOrder.Last().AddGenerateObject(EDataFiller::RoadMap);
	GenerateOrder.Last().AddGenerateObject(EDataFiller::BildingsZoneCreater);
	GenerateOrder.Last().AddGenerateObject(ELevelBilder::Roads);
	GenerateOrder.Last().AddGenerateObject(ELevelBilder::Tower);
	
	GenerateOrder.Add(FGenerateOrderModule());
	GenerateOrder.Last().AddedGenerateRange = 10;
	
	GenerateOrder.Last().AddGenerateObject(EDataFiller::BildingDisposer);
	GenerateOrder.Last().AddGenerateObject(EDataFiller::Graph);
	GenerateOrder.Last().AddGenerateObject(ELevelBilder::BildingsZone);
	GenerateOrder.Last().AddGenerateObject(ELevelBilder::Bildings);

	GenerateOrder.Add(FGenerateOrderModule());
	GenerateOrder.Last().AddedGenerateRange = 5;
	GenerateOrder.Last().AddGenerateObject(EDataFiller::CarTrack);
	GenerateOrder.Last().AddGenerateObject(ELevelBilder::CarTrack);
	

	GenerateOrder.Add(FGenerateOrderModule());
	GenerateOrder.Last().AddedGenerateRange = 10;
	GenerateOrder.Last().AddGenerateObject(ELevelBilder::Rooms);


	GenerateOrderOnlyDataFillers.Add(std::unique_ptr<FGenerateOrderModule>(new FGenerateOrderModule()));
	GenerateOrderOnlyDataFillers.Last()->AddedGenerateRange = 40;
	GenerateOrderOnlyDataFillers.Last()->AddGenerateObject(EDataFiller::MainRoadMap);
	GenerateOrderOnlyDataFillers.Last()->AddGenerateObject(EDataFiller::RoadMap);
	GenerateOrderOnlyDataFillers.Last()->AddGenerateObject(EDataFiller::BildingsZoneCreater);
	GenerateOrderOnlyDataFillers.Last()->AddGenerateObject(EDataFiller::BildingDisposer);
	GenerateOrderOnlyDataFillers.Last()->AddGenerateObject(EDataFiller::Graph);
	
}


void ALevelGenerator::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	DestroyAllMeshes();

	if (!MainMenuLevel)
	{
		VirtualSpawner = GetWorld()->SpawnActor<AVirtualSpawner>(AVirtualSpawner::StaticClass());

		VirtualSpawner->SetComplicitySpawn(LevelSettings.ComplicitySpawnForTick);

		LastBildCharacterLocation = StartGenerateCoordinate;
	}


	GenerateArea(StartGenerateCoordinate);


}


void ALevelGenerator::BeginPlay()
{
	Super::BeginPlay();

	if (!MainMenuLevel)
	{
		GetWorldTimerManager().SetTimer(BildTimer, this, &ALevelGenerator::BildTimerMetod, 1.0f, true, 1.0f);
	}

}

void  ALevelGenerator::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	

	
	if (!HoverCarMovingFuture.valid())
	{
		HoverCarMovingFuture = std::async(std::launch::async, AsyncMovingHoverCar, DeltaSeconds, DataStorage.get(), &LevelSettings);

	}
	else
	{
		if (HoverCarMovingFuture._Is_ready())
		{
			HoverCarMovingFuture = std::async(std::launch::async, AsyncMovingHoverCar, DeltaSeconds, DataStorage.get(), &LevelSettings);

		}
	}
}

void ALevelGenerator::CreateBildingTask(FVector2D GenerateCenter)
{
	AddBildingTask(std::async(std::launch::async, AsyncGenerateArea, GenerateMashine.get(), GenerateCenter, this));

}

void ALevelGenerator::AddBildingTask(std::shared_future<void> BildingFuture)
{
	for (int i = 0; i < BildingTaskQueue.size(); i++)
	{
		if (BildingTaskQueue.front()._Is_ready())
		{
			BildingTaskQueue.pop();
		}
		else
		{
			break;
		}
	}

	BildingTaskQueue.push(BildingFuture);

}



void ALevelGenerator::AsyncMovingHoverCar(float DeltaTime, FDataStorage* DataStorage, FLevelGeneratorSettings* LevelSettings)
{
	std::unique_lock<std::mutex> DataStorageLock(DataStorage->DataLock);

	FVector2D CenterCoordinate = DataStorage->ActiveHoverCarTrackCenterCoordinate;
	int Radius = DataStorage->ActiveHoverCarTrackRadius;


	for (int i = CenterCoordinate.X - Radius; i <= CenterCoordinate.X + Radius; i++)
	{
		
		for (int j = CenterCoordinate.Y - Radius; j <= CenterCoordinate.Y + Radius; j++)
		{
			for (int k = 0; k < LevelSettings->HoverCarTrackLayers.Num(); k++)
			{
				FCartTracsContainer& CurrentTrack = (*DataStorage->CarTrackMap.GetCell(i, j))[k];
				for (auto CurrentCarNod : CurrentTrack.CarTrackNodes)
				{
					MoveCarOnTrack(CurrentCarNod.get(), DeltaTime, k);

					const std::map<const FCarTrackNode*, std::shared_ptr<FCarTrackLink>>& CurrentCarNodLinks = CurrentCarNod->GetLinks();

					for (auto CurrentLink : CurrentCarNodLinks)
					{
						MoveCarOnTrack(CurrentLink.second.get(), DeltaTime, k);
					}
				}
			}
		}
	}
}

void ALevelGenerator::MoveCarOnTrack(FCarTrackGraphBase* CurrentTrack, float DeltaTime, int TrackLayerNum)
{
	std::vector<std::shared_ptr<FHoverCar>> CarsForRemove;
	for (auto CurrentHoverCar = CurrentTrack->GetHoverCars().begin(); CurrentHoverCar != CurrentTrack->GetHoverCars().end(); CurrentHoverCar++)
	{
		(*CurrentHoverCar)->CurrentTrackCoordinate += DeltaTime * (*CurrentHoverCar)->Speed;

		if ((*CurrentHoverCar)->CurrentTrackLenght < (*CurrentHoverCar)->CurrentTrackCoordinate)
		{
			CarsForRemove.push_back(*CurrentHoverCar);

			FCarTrackGraphBase* NewTrack = CurrentTrack->GetNextTrack(TrackLayerNum);

			NewTrack->AddHoverCar(*CurrentHoverCar);

			(*CurrentHoverCar)->CurrentTrackCoordinate = 0;

			(*CurrentHoverCar)->CurrentTrackLenght = NewTrack->GetTreckLenght();

			FVector Coordinate;
			FVector Tangent;

			NewTrack->GetCoordinateAndTangent((*CurrentHoverCar)->CurrentTrackCoordinate, Coordinate, Tangent);

			(*CurrentHoverCar)->SetCoordinate(Coordinate, Tangent);
		}
		else
		{
			FVector Coordinate;
			FVector Tangent;

			CurrentTrack->GetCoordinateAndTangent((*CurrentHoverCar)->CurrentTrackCoordinate, Coordinate, Tangent);

			(*CurrentHoverCar)->SetCoordinate(Coordinate, Tangent);
			
		}
	}

	for (int i = 0; i < CarsForRemove.size(); i++)
	{
		CurrentTrack->RemoveHoverCar(CarsForRemove[i]);
	}
}


void ALevelGenerator::BildTimerMetod()
{
	PlayerCharacter = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (!PlayerCharacter) return;
	FVector2D PlayerLocation = FVector2D(PlayerCharacter->GetActorLocation()) / LevelSettings.CellSize;

	PlayerLocation.X = RoundNumber(PlayerLocation.X, ERoundType::Mathematical);

	PlayerLocation.Y = RoundNumber(PlayerLocation.Y, ERoundType::Mathematical);


	FVector2D DeltaLocation = PlayerLocation - LastBildCharacterLocation;

	if ((abs(DeltaLocation.X) > LevelSettings.DeltaCellForGeneration || abs(DeltaLocation.Y) > LevelSettings.DeltaCellForGeneration))
	{
		LastBildCharacterLocation = PlayerLocation;
		
		CreateBildingTask(PlayerLocation);
	}

}



void ALevelGenerator::AsyncGenerateArea(FGenerateMashine* GenerateMashine, FVector2D GenerateCenter, ALevelGenerator* LevelGenerator)
{
	std::unique_lock<std::mutex> BildingTaskLock(LevelGenerator->BildingTaskMutex);

	int RadiusForGenerate = GenerateMashine->GetRadiusNecessaryForGenerate();

	auto CellFiller = [&](int X, int Y)
	{
		return std::unique_ptr<FLevelCellData>(new FLevelCellData(LevelGenerator->LevelSettings, FVector2D(X, Y), *LevelGenerator->DataStorage));
	};

	LevelGenerator->DataStorage->LevelMap.FillRange(GenerateCenter - FVector2D(RadiusForGenerate, RadiusForGenerate), GenerateCenter + FVector2D(RadiusForGenerate, RadiusForGenerate), CellFiller);


	GenerateMashine->Generate(GenerateCenter, LevelGenerator->VirtualSpawner);
	
}


void ALevelGenerator::SetNewSettings()
{
	if (RoomStorage)
	{
		RoomStorage->CreateLinksForAllDirections();
	}

	DataStorage = std::unique_ptr<FDataStorage>(new FDataStorage(LevelSettings));

	DataStorage->SpawnCarTrack.resize(LevelSettings.HoverCarTrackLayers.Num());

	DestroyAllMeshes();

	TArray<std::unique_ptr<FGenerateOrderModule>> GenerateOrderTemp;

	for (int i = 0; i < GenerateOrder.Num(); i++)
	{
		GenerateOrderTemp.Add(std::unique_ptr<FGenerateOrderModule>(new FGenerateOrderModule(GenerateOrder[i])));
	}

	GenerateMashine = std::unique_ptr<FGenerateMashine>(new FGenerateMashine(GenerateOrderTemp, *DataStorage, LevelSettings, *this));

	GenerateMashineOnlyDataFillers = std::unique_ptr<FGenerateMashine>(new FGenerateMashine(GenerateOrderOnlyDataFillers, *DataStorage, LevelSettings, *this));
}



void ALevelGenerator::GenerateArea(FVector2D GenerateCenter)
{
	UE_LOG(LogLevelGen, Display, TEXT("Generate Start"));
	
	if (!RoomStorage)
	{
		
		UE_LOG(LogLevelGen, Error, TEXT("Has not RoomStorage"));
		return;
	}

	if (!TowerStorage)
	{

		UE_LOG(LogLevelGen, Error, TEXT("Has not TowerStorage"));
		return;
	}

	if (!HoverCarStorage)
	{

		UE_LOG(LogLevelGen, Error, TEXT("Has not HoverCarStorage"));
		return;
	}
	
	
	if (IsSettingsChange() || NeedRegenerateSettings)
	{

		RandomGenerator = std::unique_ptr<URandomGenerator>(new URandomGenerator(SetGenerateSeed ? GenerateSeed : time(0)));
		
		SetNewSettings();
		CurrentLevelSettings = LevelSettings;
		NeedRegenerateSettings = false;
		
	}

	int RadiusForGenerate = GenerateMashine->GetRadiusNecessaryForGenerate();

	auto CellFiller = [&](int X, int Y)
	{
		return std::unique_ptr<FLevelCellData> (new FLevelCellData(LevelSettings, FVector2D(X,Y), *DataStorage));
	};

	DataStorage->LevelMap.FillRange(GenerateCenter - FVector2D(RadiusForGenerate, RadiusForGenerate), GenerateCenter + FVector2D(RadiusForGenerate, RadiusForGenerate), CellFiller);


	GenerateMashine->Generate(GenerateCenter, nullptr);

}



bool ALevelGenerator::IsSettingsChange()
{
	if (LevelSettings == CurrentLevelSettings)
	{
		return false;
	}
	else
	{
		return true;
	}
	
}

void ALevelGenerator::ReGenerateSceneClick()
{
	DestroyAllMeshes();
	NeedRegenerateSettings = true;
	GenerateArea(StartGenerateCoordinate);
}



void ALevelGenerator::TestClick()
{
	/*std::shared_ptr<FDataStorage> MyDataStorage(new FDataStorage(LevelSettings));
	std::shared_ptr<FDataFillerCarTrack>Filler(new FDataFillerCarTrack(*MyDataStorage, LevelSettings, *this));
	Filler->CreateTrackLayerLinks(0, 0, 0);*/
}

void ALevelGenerator::GenerateSceneClick()
{
	GenerateArea(StartGenerateCoordinate);
}

void ALevelGenerator::DestroyAllMeshesClick()
{
	DestroyAllMeshes();
	NeedRegenerateSettings = true;
}

void ALevelGenerator::SwitchShowModuleLeadTime()
{
	GenerateMashine->SetShowModuleLeadTime(ShowModuleLeadTime);
}

void ALevelGenerator::DestroyAllMeshes()
{
	// Destroy all meshes that have a  tag
	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if (ActorItr->ActorHasTag(LevelSettings.ActorTag))
		{
			ActorItr->Destroy();
		}
	}

}

const std::vector<std::shared_ptr<FLevelRoomLink>>&  ALevelGenerator::GetLevelLinks(FVector Coordinate, EDirection LinkDirection) const
{
	ELevelCellType CellType = DataStorage->LevelMap.GetCell(FVector2D(Coordinate))->CellInst->GetCellType();
	if (CellType == ELevelCellType::Road || CellType == ELevelCellType::MainRoad)
	{
		return RoomStorage->GetLevelLinks(true, LinkDirection);
	}
	else
	{
		return RoomStorage->GetLevelLinks(false, LinkDirection);
	}

}



bool ALevelGenerator::GetPlayerStart(FVector& PlayerLocation, const ALevelGenerator& LevelGenerator, FVector2D ApproximateLocation)
{

	UObjectsDisposer PlayerCoordinateDisposer(std::make_pair(ApproximateLocation - FVector2D(10, 10), ApproximateLocation + FVector2D(10, 10)),
		LevelGenerator.GetRandomGenerator().GetGenerator(), LevelGenerator.LevelSettings);

	auto Predicate = [&](FVector Coordinate)
	{
		FVector2D CellCoordinate(Coordinate);


		if (LevelGenerator.DataStorage->LevelMap.IsCellExist(CellCoordinate) && LevelGenerator.DataStorage->LevelMap.GetCell(CellCoordinate)->CellInst && 
			LevelGenerator.DataStorage->LevelMap.GetCell(CellCoordinate)->CellInst->CanPlayerSpawn())
		{
			
			if (LevelGenerator.DataStorage->LevelMap.GetCell(FVector2D(Coordinate))->Floors[Coordinate.Z].FloorInst &&
				LevelGenerator.DataStorage->LevelMap.GetCell(FVector2D(Coordinate))->Floors[Coordinate.Z].FloorInst->CanPlayerSpawn())
			{
				if (LevelGenerator.DataStorage->LevelMap.GetCell(FVector2D(Coordinate))->Floors[Coordinate.Z].PlasedRoom)
				{
					if (LevelGenerator.DataStorage->LevelMap.GetCell(FVector2D(Coordinate))->Floors[Coordinate.Z].PlasedRoom->GetRoom()->GetCanPlayerSpawn())
					{
						return true;
					}
					else
					{
							
						return false;
					}
				}
				else
				{
					return true;
				}
					
			}
			
			
		}
		return false;
	};


	if (!PlayerCoordinateDisposer.GetLocation(PlayerLocation, ApproximateLocation, FVector2D(1, 1), Predicate)) return false;
		
	PlayerLocation.X *= LevelGenerator.LevelSettings.CellSize;
	PlayerLocation.Y *= LevelGenerator.LevelSettings.CellSize;
	PlayerLocation.Z = PlayerLocation.Z * LevelGenerator.LevelSettings.CellHeight + 100;

	return true;

}


