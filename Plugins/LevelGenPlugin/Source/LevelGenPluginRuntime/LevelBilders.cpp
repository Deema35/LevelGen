// Copyright 2018 Pavlov Dmitriy
#include "LevelBilders.h"
#include "DataFillers.h"
#include "LevelGenCore.h"
#include "LevelCell.h"
#include "LevelGeneratorSettings.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SplineMeshComponent.h"
#include "LevelGenPluginRuntime_LogCategory.h"
#include "LevelGraphNode.h"
#include "LevelGenerator.h"
#include "ProceduralMeshActor.h"
#include "ProceduralFigure.h"
#include "LevelGenSplineMeshActor.h"
#include "LevelGenRoad.h"
#include "LevelFloor.h"
#include "Rooms/LevelRooms.h"
#include "Rooms/PlacedLevelRoom.h"
#include "Rooms/LevelRoomActor.h"
#include "Towers/LevelTowers.h"
#include "Towers/PlacedLevelTower.h"
#include "Towers/LevelTowerActor.h"
#include "LevelGenBildingZone.h"
#include "Kismet/GameplayStatics.h"
#include "HoverCars/CarTrackGraph.h"
#include "VirtualSpawner.h"
#include "HoverCars/HoverCarActor.h"
#include "HoverCars/HoverCarStorage.h"
#include "DataStorage.h"
#include "HoverCars/HoverCar.h"
#include "SearchGraph.h"

static_assert(ELevelBilder::End == (ELevelBilder)6, "You must update CreateComponent after add new enum");

FLevelBilderBase* ELevelBilderCreate(ELevelBilder Type, FDataStorage& DataStorage, const FLevelGeneratorSettings& LevelSettings, const ALevelGenerator& ParentActor)
{
	switch (Type)
	{

	case ELevelBilder::Rooms:

		return new FLevelBilderRooms(DataStorage, LevelSettings, ParentActor);


	case ELevelBilder::BildingsZone:

		return new FLevelBilderBildingsZone(DataStorage, LevelSettings, ParentActor);

	case ELevelBilder::Bildings:

		return new FLevelBilderBildings(DataStorage, LevelSettings, ParentActor);


	case ELevelBilder::Roads:

		return new FLevelBilderRoads(DataStorage, LevelSettings, ParentActor);

	case ELevelBilder::CarTrack:

		return new FLevelBilderCarTrack(DataStorage, LevelSettings, ParentActor);

	case ELevelBilder::Tower:

		return new FLevelBilderTower(DataStorage, LevelSettings, ParentActor);

	default : throw;

	}
	

	
}

//******************************************************
//FLevelBilderBase
//*******************************************************



void FLevelBilderBase::CreateMesh(AVirtualSpawner* VirtualSpawner, FLevelBilderCell& LevelBilderCell, UStaticMesh* Mesh, FName ActorTag, FVector Location, bool Collision, FRotator Rotation, FVector Scale)
{
	if (!Mesh) return;

	if (!VirtualSpawner)
	{
		AStaticMeshActor* MeshActor = ParentActor.GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Location, Rotation);

		if (MeshActor)
		{
			MeshActor->SetMobility(EComponentMobility::Stationary);
			MeshActor->GetStaticMeshComponent()->SetStaticMesh(Mesh);
			
			MeshActor->SetActorScale3D(Scale);
			if (Collision)
			{
				MeshActor->GetStaticMeshComponent()->SetCollisionProfileName(FName("BlockAllDynamic"));
			}
			else
			{
				MeshActor->GetStaticMeshComponent()->SetCollisionProfileName(FName("NoCollision"));
			}

#if WITH_EDITORONLY_DATA
			MeshActor->ReregisterAllComponents();
			MeshActor->Tags.Add(ActorTag);
			MeshActor->SetFolderPath(ActorTag);
#endif //WITH_EDITORONLY_DATA

			LevelBilderCell.CreatedMeshActors.push_back(MeshActor);
		}
	}
	else
	{
		VirtualSpawner->AddTaskToQueue(std::shared_ptr<FActorTaskBase>(new FActorTaskCreateStaticMeshActor(LevelBilderCell, &ParentActor, Mesh, Location, Rotation, Scale, ActorTag, Collision, LevelSettings)));
	}

	
}

void FLevelBilderBase::CreateMesh(AVirtualSpawner* VirtualSpawner, FLevelBilderCell& LevelBilderCell, UStaticMesh* Mesh, FName ActorTag, FVector ActorLoc,
	const std::vector<FVector>& Points, bool Collision, FVector2D Scale)
{
	if (!Mesh) return;

	if (Points.size() < 2) throw;

	if (!VirtualSpawner)
	{
		ALevelGenSplineMeshActor* SplineActor = ParentActor.GetWorld()->SpawnActor<ALevelGenSplineMeshActor>(ALevelGenSplineMeshActor::StaticClass(), ActorLoc, FRotator::ZeroRotator);

		if (SplineActor)
		{
			SplineActor->SetStaticMesh(Mesh);

			SplineActor->SetScale(Scale);

			
			SplineActor->AddPoints(Points);
			
			for (int i = 0; i < SplineActor->SplineMeshComponents.Num(); i++)
			{
				if (Collision)
				{
					SplineActor->SplineMeshComponents[i]->SetCollisionProfileName(FName("BlockAllDynamic"));
				}
				else
				{
					SplineActor->SplineMeshComponents[i]->SetCollisionProfileName(FName("NoCollision"));
				}
			}

#if WITH_EDITORONLY_DATA
			SplineActor->Tags.Add(ActorTag);
			SplineActor->SetFolderPath(ActorTag);
			SplineActor->ReregisterAllComponents();
#endif //WITH_EDITORONLY_DATA

			LevelBilderCell.CreatedMeshActors.push_back(SplineActor);

		}
	}
	else
	{
		VirtualSpawner->AddTaskToQueue(std::shared_ptr<FActorTaskBase>(new FActorTaskCreateSplineMeshActor(LevelBilderCell, &ParentActor, Mesh, ActorLoc, Points, Scale, ActorTag, Collision, LevelSettings)));
	}
	
	
	
}


void FLevelBilderBase::CreateProceduralActor(AVirtualSpawner* VirtualSpawner, FLevelBilderCell& LevelBilderCell, const FProceduralFigurBuffer& FigureBufer, FVector AbsLocation, FName ActorTag, bool Collision)
{
	if (!VirtualSpawner)
	{
		ALevelGenProceduralMeshActor* ProceduralMeshActor = ParentActor.GetWorld()->SpawnActor<ALevelGenProceduralMeshActor>(ALevelGenProceduralMeshActor::StaticClass(), AbsLocation, FRotator::ZeroRotator);
		if (ProceduralMeshActor)
		{
			
#if WITH_EDITORONLY_DATA
			ProceduralMeshActor->Tags.Add(ActorTag);
			ProceduralMeshActor->SetFolderPath(ActorTag);
			
#endif //WITH_EDITORONLY_DATA
			ProceduralMeshActor->ReregisterAllComponents();
			ProceduralMeshActor->SetCollision(Collision);
			ProceduralMeshActor->AddMesh(FigureBufer);
			LevelBilderCell.CreatedMeshActors.push_back(ProceduralMeshActor);

			
		}
	}
	else
	{

		VirtualSpawner->AddTaskToQueue(std::shared_ptr<FActorTaskBase>(new FActorTaskCreateProceduralActor(LevelBilderCell, &ParentActor, FigureBufer, AbsLocation,  ActorTag, Collision, LevelSettings)));
	}
	
	
}



void  FLevelBilderBase::BildActor(AVirtualSpawner* VirtualSpawner, FLevelBilderCell& LevelBilderCell, UClass* ActorClass, FVector ActorStartCoordinate, FVector2D ActorSize, int ActorYaw)
{
	if (!ActorClass) throw;

	FVector2D DeltaActorSize(
		ActorSize.X > 0 ? ActorSize.X - LevelSettings.CellSize : ActorSize.X + LevelSettings.CellSize,
		ActorSize.Y > 0 ? ActorSize.Y - LevelSettings.CellSize : ActorSize.Y + LevelSettings.CellSize);

	FVector ActorLocation(
		(ActorStartCoordinate.X + DeltaActorSize.X / 2),
		(ActorStartCoordinate.Y + DeltaActorSize.Y / 2),
		ActorStartCoordinate.Z);

	FRotator ActorRotation(0, ActorYaw, 0);

	FTransform SpawnTransform(ActorRotation, ActorLocation);

	if (!VirtualSpawner)
	{
		
		ALevelGenActorBace* NewActor = ParentActor.GetWorld()->SpawnActor<ALevelGenActorBace>(ActorClass, ActorLocation, ActorRotation);
		

		if (NewActor)
		{
			NewActor->LevelGenerator = &ParentActor;

			//NewActor->RerunConstructionScripts();

			ULevelGenBordersShowComponentBace* ShowComponent = NewActor->GetBordersShowComponent();

#if WITH_EDITORONLY_DATA
			NewActor->Tags.Add(LevelSettings.ActorTag);
			NewActor->SetFolderPath(LevelSettings.ActorTag);
#endif //WITH_EDITORONLY_DATA

			if (ShowComponent) ShowComponent->SetVisibility(false);

			LevelBilderCell.CreatedMeshActors.push_back(NewActor);
		}

		
	}
	else
	{
		VirtualSpawner->AddTaskToQueue(std::shared_ptr<FActorTaskBase>(new FActorTaskCreateBlueprintActor(LevelBilderCell, &ParentActor, ActorClass, ActorLocation, ActorRotation, LevelSettings)));
	}

	
}

void FLevelBilderBase::BildLevel(FVector2D CenterCoordinate, int Radius, AVirtualSpawner* VirtualSpawner)
{

	NewBlidArea = std::unique_ptr<FBildArea>(new FBildArea(CenterCoordinate, Radius));
	

	FVector2D Start = CenterCoordinate - FVector2D(Radius, Radius);
	FVector2D End = CenterCoordinate + FVector2D(Radius, Radius);

	auto Filler = [&](int X, int Y)
	{
		return std::unique_ptr<FLevelBilderCell>(new FLevelBilderCell());
	};

	BildArea.FillRange(Start, End, Filler);
	std::pair<int, int> YCircleCoord;
	

	for (int i = CenterCoordinate.X - Radius; i <= CenterCoordinate.X + Radius; i++)
	{
		YCircleCoord = GetCircleCoordinateFromX(i, CenterCoordinate, Radius);
		for (int j = YCircleCoord.second; j <= YCircleCoord.first; j++)
		{
			if (!BildArea.GetCell(i, j)->IsBildet)
			{
				if (BildLevelCell(i, j, *BildArea.GetCell(i, j), VirtualSpawner))
				{
					BildArea.GetCell(i, j)->IsBildet = true;
				}
				
			}
			
		}
	}
	RefreshLastArea(VirtualSpawner);
}

void FLevelBilderBase::RefreshLastArea(AVirtualSpawner* VirtualSpawner)
{
	int NewDeltaRad = NewBlidArea->Radius + 2;
	

	if (LastBlidArea)
	{
		int LastDeltaRad = LastBlidArea->Radius + 3;

		for (int i = LastBlidArea->CenterCoordinate.X - LastDeltaRad; i < LastBlidArea->CenterCoordinate.X + LastDeltaRad; i++)
		{
			
			for (int j = LastBlidArea->CenterCoordinate.Y - LastDeltaRad; j < LastBlidArea->CenterCoordinate.Y + LastDeltaRad; j++)
			{
				if (BildArea.IsCellExist(i,j) && BildArea.GetCell(i, j)->IsBildet)
				{
					if (!(NewBlidArea && IsPointInCircle(i, j, NewBlidArea->CenterCoordinate, NewDeltaRad)))
					{
						DeleteCell(i, j, *BildArea.GetCell(i, j), VirtualSpawner);
						BildArea.GetCell(i, j)->IsBildet = false;
					}
				}
			}
		}
	}
	
	LastBlidArea = std::move(NewBlidArea);
}








void FLevelBilderBase::DeleteCell(int X, int Y, FLevelBilderCell& LevelBilderCell, AVirtualSpawner* VirtualSpawner)
{
	if (!VirtualSpawner)
	{
		for (int i = 0; i < LevelBilderCell.CreatedMeshActors.size(); i++)
		{
			LevelBilderCell.CreatedMeshActors[i]->Destroy();
		}
		LevelBilderCell.CreatedMeshActors.clear();
	}
	else
	{
		VirtualSpawner->AddTaskToQueue(std::shared_ptr<FActorTaskBase>(new FActorTaskDeleteActors(LevelBilderCell, nullptr, LevelSettings)));
	}
	
}

void FLevelBilderBase::GetRadiusAndCentrCoordinateForGenerate(ELevelMapType LevelMapType, FVector2D& CenterCoordinate, int& Radius)
{
	switch (LevelMapType)
	{
	case ELevelMapType::LevelMap:

		return;

	case ELevelMapType::BildingMap:
	case ELevelMapType::RoadsMap:
	

		Radius = RoundNumber(Radius / LevelSettings.RoadFrequency, ERoundType::Up);

		CenterCoordinate = CenterCoordinate / LevelSettings.RoadFrequency;

		CenterCoordinate.X = RoundNumber(CenterCoordinate.X, ERoundType::Up) - 1;
		CenterCoordinate.Y = RoundNumber(CenterCoordinate.Y, ERoundType::Up) - 1;

		return;

	case ELevelMapType::CarTrackMap:

		Radius = RoundNumber(Radius / LevelSettings.RoadFrequency, ERoundType::Mathematical);

		CenterCoordinate = CenterCoordinate / LevelSettings.RoadFrequency;

		CenterCoordinate.X = RoundNumber(CenterCoordinate.X, ERoundType::Mathematical);
		CenterCoordinate.Y = RoundNumber(CenterCoordinate.Y, ERoundType::Mathematical);

		return;

	default: throw;
	}
}

//******************************************************
//FLevelBilderRoads
//*******************************************************



bool FLevelBilderRoads::BildLevelCell(int X, int Y, FLevelBilderCell& LevelBilderCell, AVirtualSpawner* VirtualSpawner)
{
	if (DataStorage.RoadsMap.IsCellExist(X, Y))
	{
		FLevelGenRoadBase& CurrentRoad = *DataStorage.RoadsMap.GetCell(X, Y);
		if (CurrentRoad.GetRoadState() == ERoadState::ConectedToLevelMap)
		{
			std::vector<FRoadCrossing> Crossings;

			if (CurrentRoad.GetRoadEdges(Crossings))
			{
				BildRoads(Crossings, CurrentRoad, LevelBilderCell, VirtualSpawner);

				BildRoadCrossings(Crossings, CurrentRoad, LevelBilderCell, VirtualSpawner);

				return true;
			}
		}
	}
	
	return false;
	
}

void FLevelBilderRoads::BildRoads(const std::vector<FRoadCrossing>& Crossings, const FLevelGenRoadBase& CurrentRoad, FLevelBilderCell& LevelBilderCell, AVirtualSpawner* VirtualSpawner)
{
	std::vector<FRoadCrossing> RoadEdges;
	std::vector<FVector> RoadCoordinats3D;

	for (auto CurrentCrossing : Crossings)
	{
		if (CurrentCrossing.Type == ERoadCrossingType::RoadEdge)
		{
			RoadEdges.push_back(CurrentCrossing);
		}
	}

	for (int j = 0; j < RoadEdges.size() / 2; j++)
	{
		RoadCoordinats3D.clear();
		
		if (CurrentRoad.IsMainRoad())
		{
			FVector ActorLoc(RoadEdges[j * 2].Coordinate.X * LevelSettings.CellSize, RoadEdges[j * 2].Coordinate.Y * LevelSettings.CellSize, -(LevelSettings.SecondCityFloor + LevelSettings.FirstCityFloor));

			RoadCoordinats3D.push_back(FVector(0, 0, 0));
			RoadCoordinats3D.push_back(FVector((RoadEdges[j * 2 + 1].Coordinate.X - RoadEdges[j * 2].Coordinate.X) * LevelSettings.CellSize,
				(RoadEdges[j * 2 + 1].Coordinate.Y - RoadEdges[j * 2].Coordinate.Y) * LevelSettings.CellSize, 0));
			
			
			float SizeCoeff = LevelSettings.MainRoadMesh ? ((float)LevelSettings.CellSize / LevelSettings.MainRoadMesh->GetBoundingBox().GetSize().Y) * CurrentRoad.GetSize() : 1;

			

			CreateMesh(VirtualSpawner, LevelBilderCell, LevelSettings.MainRoadMesh, LevelSettings.ActorTag, ActorLoc, RoadCoordinats3D, false, FVector2D(SizeCoeff, SizeCoeff));

		}
		else
		{
			for (int i = 0; i < 2; i++)
			{

				RoadCoordinats3D.push_back(FVector(RoadEdges[j * 2 + i].Coordinate.X * LevelSettings.CellSize, RoadEdges[j * 2 + i].Coordinate.Y * LevelSettings.CellSize, -(LevelSettings.SecondCityFloor + LevelSettings.FirstCityFloor)));
			}

			
			FillHoleUnderRoad(RoadCoordinats3D, CurrentRoad, VirtualSpawner, LevelBilderCell);
		}

		
	}
	
}

void FLevelBilderRoads::FillHoleUnderRoad(const std::vector<FVector>& RoadCoordinats3D, const FLevelGenRoadBase& CurrentRoad, AVirtualSpawner* VirtualSpawner, FLevelBilderCell& LevelBilderCell)
{
	FVector BaseCoordinate((RoadCoordinats3D[0].X + RoadCoordinats3D[1].X) / 2, (RoadCoordinats3D[0].Y + RoadCoordinats3D[1].Y) / 2, RoadCoordinats3D[0].Z);

	FVector2D BaseSize(RoadCoordinats3D[1] - RoadCoordinats3D[0]);

	BaseSize.X = abs(BaseSize.X);
	BaseSize.Y = abs(BaseSize.Y);

	if (BaseSize.X == 0)
	{
		BaseSize.X = CurrentRoad.GetSize() * LevelSettings.CellSize;
	}
	else if (BaseSize.Y == 0)
	{
		BaseSize.Y = CurrentRoad.GetSize() * LevelSettings.CellSize;
	}
	FProceduralFigurBuffer Buffer(FVector(0, 0, 0));
	
	FProceduralFigureRectangle Rectangle(FVector(0, 0, 0), FVector(0, 0, 1), FVector(1, 0, 0), BaseSize, LevelSettings.RoadGroundMaterial);

	Buffer.AddProceduralFigure(Rectangle, FVector(0, 0, 0), &LevelSettings.RoadGroundMaterial);

	CreateProceduralActor(VirtualSpawner, LevelBilderCell, Buffer, BaseCoordinate, LevelSettings.ActorTag);

}

void FLevelBilderRoads::FillHoleUnderRoad(FVector2D Coordinate, FVector2D Size, AVirtualSpawner* VirtualSpawner, FLevelBilderCell& LevelBilderCell)
{
	FProceduralFigurBuffer Buffer(FVector(0, 0, 0));

	FProceduralFigureRectangle Rectangle(FVector(0, 0, 0), FVector(0, 0, 1), FVector(1, 0, 0), Size, LevelSettings.RoadGroundMaterial);

	Buffer.AddProceduralFigure(Rectangle, FVector(0, 0, 0), &LevelSettings.RoadGroundMaterial);

	CreateProceduralActor(VirtualSpawner, LevelBilderCell, Buffer, FVector(Coordinate, -(LevelSettings.SecondCityFloor + LevelSettings.FirstCityFloor)),
		LevelSettings.ActorTag);
	
}

void FLevelBilderRoads::BildRoadCrossings(std::vector<FRoadCrossing>& Crossings, const FLevelGenRoadBase& CurrentRoad, FLevelBilderCell& LevelBilderCell, AVirtualSpawner* VirtualSpawner)
{
	for (auto CurrentCrossing : Crossings)
	{
		switch (CurrentCrossing.Type)
		{

		case ERoadCrossingType::XCrossing:

			if (CurrentCrossing.CrossingRoad->IsMainRoad())
			{
				if (CurrentRoad.IsMainRoad())
				{
					float SizeCoeffX = LevelSettings.MainRoadCrossingMesh ? ((float)LevelSettings.CellSize / LevelSettings.MainRoadCrossingMesh->GetBoundingBox().GetSize().X) * CurrentRoad.GetSize() : 1;
					float SizeCoeffY = LevelSettings.MainRoadCrossingMesh ? ((float)LevelSettings.CellSize / LevelSettings.MainRoadCrossingMesh->GetBoundingBox().GetSize().Y) * CurrentRoad.GetSize() : 1;

					CreateMesh(VirtualSpawner, LevelBilderCell, LevelSettings.MainRoadCrossingMesh, LevelSettings.ActorTag,
						FVector(CurrentCrossing.Coordinate * LevelSettings.CellSize, -(LevelSettings.SecondCityFloor + LevelSettings.FirstCityFloor)), false,
						FRotator::ZeroRotator, FVector(SizeCoeffX, SizeCoeffY, SizeCoeffX));
				}
				else
				{
					BildMainRoadAndNotMaindRoadCrossing(CurrentCrossing.CrossingRoad, &CurrentRoad, CurrentCrossing.Coordinate * LevelSettings.CellSize, LevelBilderCell, VirtualSpawner);
				}
			}
			else
			{
				if (CurrentRoad.IsMainRoad())
				{
					BildMainRoadAndNotMaindRoadCrossing(&CurrentRoad, CurrentCrossing.CrossingRoad, CurrentCrossing.Coordinate * LevelSettings.CellSize, LevelBilderCell, VirtualSpawner);
				}
				else
				{

				FillHoleUnderRoad(CurrentCrossing.Coordinate * LevelSettings.CellSize,
						FVector2D(CurrentRoad.GetSize(), CurrentRoad.GetSize()) * LevelSettings.CellSize, VirtualSpawner, LevelBilderCell);
				}
			}
			break;

		case ERoadCrossingType::TCrossing:

			if (CurrentRoad.IsMainRoad())
			{
				
				std::vector<FVector> MainCoordinats;
				FVector CrossingLoc(CurrentCrossing.Coordinate * LevelSettings.CellSize, -LevelSettings.FirstCityFloor);
				

				switch (CurrentRoad.GetDirection())
				{
				case ERoadDirection::Horizontal:

					MainCoordinats.push_back(FVector(CrossingLoc.X - CurrentCrossing.CrossingRoad->GetSize() * LevelSettings.CellSize / 2, CrossingLoc.Y, 0));
					MainCoordinats.push_back(FVector(CrossingLoc.X + CurrentCrossing.CrossingRoad->GetSize() * LevelSettings.CellSize / 2, CrossingLoc.Y, 0));

					break;

				case ERoadDirection::Vertical:

					MainCoordinats.push_back(FVector(CrossingLoc.X, CrossingLoc.Y - CurrentCrossing.CrossingRoad->GetSize() * LevelSettings.CellSize / 2, 0));
					MainCoordinats.push_back(FVector(CrossingLoc.X, CrossingLoc.Y + CurrentCrossing.CrossingRoad->GetSize() * LevelSettings.CellSize / 2, 0));

					break;

				default: throw;

				}

				float MainRoadSegmentSizeCoeff = LevelSettings.MainRoadMesh ? ((float)LevelSettings.CellSize / LevelSettings.MainRoadMesh->GetBoundingBox().GetSize().Y) * CurrentRoad.GetSize() : 1;

				FVector ActorLoc(0, 0, -(LevelSettings.SecondCityFloor + LevelSettings.FirstCityFloor));
				CreateMesh(VirtualSpawner, LevelBilderCell, LevelSettings.MainRoadMesh, LevelSettings.ActorTag, ActorLoc, MainCoordinats, false, FVector2D(MainRoadSegmentSizeCoeff, MainRoadSegmentSizeCoeff));

				
			}
			else
			{
				
				if (!CurrentCrossing.CrossingRoad->IsMainRoad())
				{

					FillHoleUnderRoad(CurrentCrossing.Coordinate * LevelSettings.CellSize, FVector2D(CurrentRoad.GetSize(),
						CurrentCrossing.CrossingRoad->GetSize()) * LevelSettings.CellSize, VirtualSpawner, LevelBilderCell);

				}

				
			}

			break;
		}
	}
	
}

void FLevelBilderRoads::BildMainRoadAndNotMaindRoadCrossing(const FLevelGenRoadBase* MainRoad, const FLevelGenRoadBase* NotMainRoad, FVector2D CrossingLocation,
	FLevelBilderCell& LevelBilderCell, AVirtualSpawner* VirtualSpawner)
{
	std::vector<FVector> MainCoordinats;
	std::vector<FVector> NotMainCoordinats;

	FVector CrossingLoc(CrossingLocation, -LevelSettings.FirstCityFloor);

	switch (NotMainRoad->GetDirection())
	{

	case ERoadDirection::Horizontal:

		MainCoordinats.push_back(FVector(CrossingLoc.X - MainRoad->GetSize() * LevelSettings.CellSize / 2, CrossingLoc.Y, CrossingLoc.Z));
		MainCoordinats.push_back(FVector(CrossingLoc.X + MainRoad->GetSize() * LevelSettings.CellSize / 2, CrossingLoc.Y, CrossingLoc.Z));

		

		NotMainCoordinats.push_back(FVector(CrossingLoc.X, CrossingLoc.Y - NotMainRoad->GetSize() * LevelSettings.CellSize / 2, 0));
		NotMainCoordinats.push_back(FVector(CrossingLoc.X, CrossingLoc.Y + NotMainRoad->GetSize() * LevelSettings.CellSize / 2, 0));

		

		break;

	case ERoadDirection::Vertical:

		MainCoordinats.push_back(FVector(CrossingLoc.X, CrossingLoc.Y - MainRoad->GetSize() * LevelSettings.CellSize / 2, CrossingLoc.Z));
		MainCoordinats.push_back(FVector(CrossingLoc.X, CrossingLoc.Y + MainRoad->GetSize() * LevelSettings.CellSize / 2, CrossingLoc.Z));

		NotMainCoordinats.push_back(FVector(CrossingLoc.X - NotMainRoad->GetSize() * LevelSettings.CellSize / 2, CrossingLoc.Y, 0));
		NotMainCoordinats.push_back(FVector(CrossingLoc.X + NotMainRoad->GetSize() * LevelSettings.CellSize / 2, CrossingLoc.Y,  0));

		break;

	default: throw;

	}

	float SizeCoeff = LevelSettings.MainRoadMesh ? ((float)LevelSettings.CellSize / LevelSettings.MainRoadMesh->GetBoundingBox().GetSize().Y) * MainRoad->GetSize() : 1;

	FVector ActorLoc(0, 0, -(LevelSettings.SecondCityFloor + LevelSettings.FirstCityFloor));

	CreateMesh(VirtualSpawner, LevelBilderCell, LevelSettings.MainRoadMesh, LevelSettings.ActorTag, ActorLoc, NotMainCoordinats, false, FVector2D(SizeCoeff, SizeCoeff));

}

//******************************************************
//FLevelBilderRooms
//*******************************************************




bool FLevelBilderRooms::BildLevelCell(int X, int Y, FLevelBilderCell& LevelBilderCell, AVirtualSpawner* VirtualSpawner)
{
	if (DataStorage.BildingMap.IsCellExist(X, Y))
	{
		
		FLevelGenBildingZoneBase& CurrentBilding = *DataStorage.BildingMap.GetCell(X, Y);

		if (CurrentBilding.GetState() == EBildingZoneState::FlooreAndWallCreated)
		{

			const std::vector<FLvevelLayer>& GraphLayers = CurrentBilding.GetGraphLayers();

			for (int k = 0; k < GraphLayers.size(); k++)
			{
				for (int i = 0; i < GraphLayers[k].Graph.size(); i++)
				{
					for (int j = 0; j < GraphLayers[k].Graph[i].size(); j++)
					{
						FLevelGraphNodeBase& CurrentNode = *GraphLayers[k].Graph[i][j];

						BildPlacedRoom(CurrentNode.NodeRoom.get(), LevelBilderCell, VirtualSpawner);

						for (auto CurentLink = CurrentNode.Links.begin(); CurentLink != CurrentNode.Links.end(); CurentLink++)
						{
							if (CurentLink->second)
							{

								for (auto CurrentRoom = CurentLink->second->Rooms.begin(); CurrentRoom != CurentLink->second->Rooms.end(); CurrentRoom++)
								{
									BildPlacedRoom((*CurrentRoom).get(), LevelBilderCell, VirtualSpawner);
								}
							}
						}
						
					}
				}
			}

			for (auto CurrentLink = CurrentBilding.GetLinks().begin(); CurrentLink != CurrentBilding.GetLinks().end(); CurrentLink++)
			{
				
				if (CurrentLink->second)
				{
					for (int i = 0; i < CurrentLink->second->Links.size(); i++)
					{
							
						for (int j = 0; j < CurrentLink->second->Links[i]->Rooms.size(); j++)
						{
									
							BildPlacedRoom(CurrentLink->second->Links[i]->Rooms[j].get(), LevelBilderCell, VirtualSpawner);
									
						}
						

					}
					
					
				}

			}
			return true;
		}
		
	}

	return false;
	
}

void FLevelBilderRooms::BildPlacedRoom(FPlacedLevelRoomLinkedToLevel* PlacedRoom, FLevelBilderCell& LevelBilderCell, AVirtualSpawner* VirtualSpawner)
{
	if (PlacedRoom && BildetRooms.find(PlacedRoom) == BildetRooms.end())
	{
		BildetRooms.insert(PlacedRoom);

		FVector ActorStartCoordinate = PlacedRoom->GetStartCoordinate();

		ActorStartCoordinate.X *= LevelSettings.CellSize;
		ActorStartCoordinate.Y *= LevelSettings.CellSize;
		ActorStartCoordinate.Z *= LevelSettings.CellHeight;
		
		BildActor(VirtualSpawner, LevelBilderCell, PlacedRoom->GetRoom()->GetRoomActorClass(), ActorStartCoordinate,
			FVector2D(PlacedRoom->GetRoom()->GetSize()) * LevelSettings.CellSize, PlacedRoom->GetRoom()->GetRoomRotation());
	}
}

void FLevelBilderRooms::DeleteCell(int X, int Y, FLevelBilderCell& LevelBilderCell, AVirtualSpawner* VirtualSpawner)
{
	if (DataStorage.BildingMap.IsCellExist(X, Y))
	{
		FLevelGenBildingZoneBase& CurrentBilding = *DataStorage.BildingMap.GetCell(X, Y);
		
		const std::vector<FLvevelLayer>& GraphLayers = CurrentBilding.GetGraphLayers();

		for (int k = 0; k < GraphLayers.size(); k++)
		{
			for (int i = 0; i < GraphLayers[k].Graph.size(); i++)
			{
				for (int j = 0; j < GraphLayers[k].Graph[i].size(); j++)
				{
					FLevelGraphNodeBase& CurrentNode = *GraphLayers[k].Graph[i][j];
					BildetRooms.erase(CurrentNode.NodeRoom.get());

					for (auto CurentLink = CurrentNode.Links.begin(); CurentLink != CurrentNode.Links.end(); CurentLink++)
					{
						if (CurentLink->second)
						{

							for (auto CurrentRoom = CurentLink->second->Rooms.begin(); CurrentRoom != CurentLink->second->Rooms.end(); CurrentRoom++)
							{
								BildetRooms.erase((*CurrentRoom).get());
							}
						}
					}
				}

				
			}
		}

		for (auto CurrentLink = CurrentBilding.GetLinks().begin(); CurrentLink != CurrentBilding.GetLinks().end(); CurrentLink++)
		{

			if (CurrentLink->second)
			{
				for (int i = 0; i < CurrentLink->second->Links.size(); i++)
				{

					for (int j = 0; j < CurrentLink->second->Links[i]->Rooms.size(); j++)
					{
						BildetRooms.erase(CurrentLink->second->Links[i]->Rooms[j].get());

					}


				}


			}

		}
		
	}
	
	
	FLevelBilderBase::DeleteCell(X, Y, LevelBilderCell, VirtualSpawner);
}




//******************************************************
//FLevelBilderBildingsZone
//*******************************************************



bool FLevelBilderBildingsZone::BildLevelCell(int X, int Y, FLevelBilderCell& LevelBilderCell, AVirtualSpawner* VirtualSpawner)
{
	if (DataStorage.BildingMap.GetCell(X, Y))
	{
		FLevelGenBildingZoneBase& CurrentBilding = *DataStorage.BildingMap.GetCell(X, Y);

		if (CurrentBilding.BildingAreaFigure)
		{

			CreateProceduralActor(VirtualSpawner, LevelBilderCell, *CurrentBilding.BildingAreaFigure, FVector(CurrentBilding.GetSenterCoordinate().X *  LevelSettings.CellSize,
				CurrentBilding.GetSenterCoordinate().Y * LevelSettings.CellSize, 0), LevelSettings.ActorTag, true);

			return true;
		}
	}

	return false;
}



//******************************************************
//FLevelBilderTower
//*******************************************************



bool FLevelBilderTower::BildLevelCell(int X, int Y, FLevelBilderCell& LevelBilderCell, AVirtualSpawner* VirtualSpawner)
{
	if (DataStorage.BildingMap.IsCellExist(X, Y))
	{
		BildTowers(*DataStorage.BildingMap.GetCell(X, Y), LevelBilderCell, VirtualSpawner);

		return true;
		

	}

	return false;
}



void FLevelBilderTower::BildTowers(FLevelGenBildingZoneBase& CurrentBilding, FLevelBilderCell& LevelBilderCell, AVirtualSpawner* VirtualSpawner)
{
	std::vector<std::vector<std::shared_ptr<FPlacedLevelTower>>>& CurrentTowers = CurrentBilding.GetTowers();

	for (int i = 0; i < CurrentTowers.size(); i++)
	{
		for (int j = 0; j < CurrentTowers[i].size(); j++)
		{
			if (CurrentTowers[i][j])
			{
				FPlacedLevelTowerBase* PlacedTower = CurrentTowers[i][j].get();

				if (PlacedTower && PlacedTower->GetTower())
				{
					BildActor(VirtualSpawner, LevelBilderCell, PlacedTower->GetTower()->GetTowerActorClass(),
						FVector(PlacedTower->GetStartCoordinate() * LevelSettings.CellSize, -(LevelSettings.SecondCityFloor + LevelSettings.FirstCityFloor)),
						PlacedTower->GetTower()->GetSize() * LevelSettings.CellSize, EYawTurnGetAngle(PlacedTower->Yaw));
				}

			}

		}
	}
}




//******************************************************
//FLevelBilderBildings
//*******************************************************


 bool FLevelBilderBildings::BildLevelCell(int X, int Y, FLevelBilderCell& LevelBilderCell, AVirtualSpawner* VirtualSpawner)
{
	FLevelCellData& CurrentCell = *DataStorage.LevelMap.GetCell(X, Y);

	if (CurrentCell.LevelBilding && BildetBildings.find(CurrentCell.LevelBilding.get()) == BildetBildings.end())
	{
		FPlacedLevelBilding& PlacedBilding = *CurrentCell.LevelBilding;

		BildActor(VirtualSpawner, LevelBilderCell, PlacedBilding.GetTower()->GetTowerActorClass(),
			FVector(PlacedBilding.GetStartCoordinate() * LevelSettings.CellSize, -(LevelSettings.SecondCityFloor + LevelSettings.FirstCityFloor)),
			PlacedBilding.GetTower()->GetSize() * LevelSettings.CellSize, EYawTurnGetAngle(PlacedBilding.Yaw));

		BildetBildings.insert(CurrentCell.LevelBilding.get());

		return true;
	}
	
	return false;
}



void FLevelBilderBildings::DeleteCell(int X, int Y, FLevelBilderCell& LevelBilderCell, AVirtualSpawner* VirtualSpawner)
{
	FLevelCellData& CurrentCell = *DataStorage.LevelMap.GetCell(X, Y);

	auto FindBilding = BildetBildings.find(CurrentCell.LevelBilding.get());

	if (CurrentCell.LevelBilding && FindBilding != BildetBildings.end())
	{
		BildetBildings.erase(FindBilding);
	}

	FLevelBilderBase::DeleteCell(X, Y, LevelBilderCell, VirtualSpawner);
}



//******************************************************
//FLevelBilderCarTrack
//*******************************************************

void FLevelBilderCarTrack::Generate(FVector2D CenterCoordinate, int Radius, AVirtualSpawner* VirtualSpawner)
{
	GetRadiusAndCentrCoordinateForGenerate(GetLevelMapType(), CenterCoordinate, Radius);

	std::unique_lock<std::mutex> DataStorageLock(DataStorage.DataLock);

	BildLevel(CenterCoordinate, Radius, VirtualSpawner);
	
}

bool FLevelBilderCarTrack::BildLevelCell(int X, int Y, FLevelBilderCell& LevelBilderCell, AVirtualSpawner* VirtualSpawner)
{
	std::vector<UClass*> HoverCars;

	ParentActor.GetHoverCarStorage()->GetHoverCars(HoverCars);

	for (int k = 0; k < LevelSettings.HoverCarTrackLayers.Num(); k++)
	{

		FCartTracsContainer& CurrentTrack = (*DataStorage.CarTrackMap.GetCell(X, Y))[k];

		for (auto CurrentCarNod : CurrentTrack.CarTrackNodes)
		{
			if (!CurrentCarNod->IsTrackActive())
			{

				CreateHoverCars(*CurrentCarNod, HoverCars, LevelBilderCell, VirtualSpawner, LevelSettings.HoverCarTrackLayers[k].OneHowerCarsOnRoadUnits);

				CurrentCarNod->SetActive();

				LevelBilderCell.ActiveTracs.push_back(CurrentCarNod.get());
			}

			const std::map<const FCarTrackNode*, std::shared_ptr<FCarTrackLink>>& CurrentCarNodLinks = CurrentCarNod->GetLinks();

			for (auto CurrentLink : CurrentCarNodLinks)
			{
				if (!CurrentLink.second->IsTrackActive())
				{
					CurrentLink.second->SetActive();

					LevelBilderCell.ActiveTracs.push_back(CurrentLink.second.get());
				}
			}

		}
	}

	return true;
}





void FLevelBilderCarTrack::CreateHoverCars(FCarTrackGraphBase& CarTrack, std::vector<UClass*>& HoverCars, FLevelBilderCell& LevelBilderCell, AVirtualSpawner* VirtualSpawner,int OneHowerCarsOnRoadUnits)
{
	if (HoverCars.size() == 0) return;

	
	int HoverCarsNum = RoundNumber((CarTrack.GetTreckLenght() / OneHowerCarsOnRoadUnits), ERoundType::Down);

	float HoverCarsNumRemainder = (CarTrack.GetTreckLenght() / OneHowerCarsOnRoadUnits) - HoverCarsNum;

	int RandomNum = ParentActor.GetRandomGenerator().GetRandomNumber(0, 100);

	if ((float)RandomNum / 100 < HoverCarsNumRemainder) HoverCarsNum++;

	float CarDiatanceSegment = CarTrack.GetTreckLenght() / HoverCarsNum;

	float CarDiatance = CarDiatanceSegment / 2;

	for (int i = 0; i < HoverCarsNum; i++) 
	{
		int HoverCarNumber = ParentActor.GetRandomGenerator().GetRandomNumber(0, HoverCars.size() - 1);
		
		CreateHoverCar(VirtualSpawner, LevelBilderCell, CarTrack, HoverCars[HoverCarNumber], CarDiatance);

		CarDiatance += CarDiatanceSegment;
		

	}
}

void FLevelBilderCarTrack::DeleteCell(int X, int Y, FLevelBilderCell& LevelBilderCell, AVirtualSpawner* VirtualSpawner)
{
	
	for (auto CurrentActivateTrack : LevelBilderCell.ActiveTracs)
	{
		CurrentActivateTrack->SetUnActive();
	}
	LevelBilderCell.ActiveTracs.clear();
	

	FLevelBilderBase::DeleteCell(X, Y, LevelBilderCell, VirtualSpawner);
}

void FLevelBilderCarTrack::CreateHoverCar(AVirtualSpawner* VirtualSpawner, FLevelBilderCell& LevelBilderCell, FCarTrackGraphBase& CarTrack, UClass* HoverCarClass, float CarDistance)
{
	FVector Coordinate;
	FVector Tangent;

	if (CarTrack.GetCoordinateAndTangent(CarDistance, Coordinate, Tangent))
	{
		std::shared_ptr<FHoverCar>NewHoverCar(new FHoverCar(CarDistance, CarTrack.GetTreckLenght()));
		CarTrack.AddHoverCar(NewHoverCar);

		if (!VirtualSpawner)
		{
			AHoverCarActor* NewHoverCarActor = ParentActor.GetWorld()->SpawnActor<AHoverCarActor>(HoverCarClass, Coordinate, Tangent.Rotation());

			if (NewHoverCarActor)
			{
				NewHoverCarActor->SetHoverCar(std::weak_ptr<FHoverCar>(NewHoverCar));
				NewHoverCar->SetSpeed(NewHoverCarActor->Speed);
				
#if WITH_EDITORONLY_DATA
				NewHoverCarActor->Tags.Add(LevelSettings.ActorTag);
				NewHoverCarActor->SetFolderPath(LevelSettings.ActorTag);
#endif //WITH_EDITORONLY_DATA

			}

		}
		else
		{
			VirtualSpawner->AddTaskToQueue(std::shared_ptr<FActorTaskBase>(new FActorTaskCreateHoverCar(&ParentActor, HoverCarClass, Coordinate, Tangent, std::weak_ptr<FHoverCar>(NewHoverCar), LevelSettings)));
		}
	}

	
}
