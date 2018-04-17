// Copyright 2018 Pavlov Dmitriy
#include"VirtualSpawner.h"
#include "LevelBilders.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "LevelGenSplineMeshActor.h"
#include "Components/SplineMeshComponent.h"
#include "ProceduralMeshActor.h"
#include "Rooms/LevelRoomActor.h"
#include "Rooms/RoomBordersShowComponent.h"
#include "LevelGenerator.h"
#include "HoverCars/HoverCarActor.h"
#include "DataStorage.h"
#include "HoverCars/HoverCar.h"

FString EActorTaskTypeToString(EActorTaskType TaskType)
{
	switch (TaskType)
		{
		case EActorTaskType::CreateBlueprintActor: return FString("CreateBlueprintActor");
		case EActorTaskType::CreateHoverCar: return FString("CreateHoverCar");
		case EActorTaskType::CreateProceduralActor: return FString("CreateProceduralActor");
		case EActorTaskType::CreateSplineMesh: return FString("CreateSplineMesh");
		case EActorTaskType::CreateStaticMesh: return FString("CreateStaticMesh");
		case EActorTaskType::DeleteActors: return FString("DeleteActors");
		default: throw;
		}
		
}

//************************************************
//FActorTaskDeleteActors
//***********************************************

bool FActorTaskDeleteActors::Execute()
{
	for (int i = 0; i < LevelBilderCell.CreatedMeshActors.size(); i++)
	{
		LevelBilderCell.CreatedMeshActors[i]->Destroy();
	}
	LevelBilderCell.CreatedMeshActors.clear();

	return true;
}

//************************************************
//FActorTaskCreateStaticMeshActor
//***********************************************

bool FActorTaskCreateStaticMeshActor::Execute()
{
	AStaticMeshActor* MeshActor = ParentActor->GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Location, Rotation);

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

		LevelBilderCell.CreatedMeshActors.push_back(MeshActor);

		return true;
	}

	return false;
}

//************************************************
//FActorTaskCreateSplineMeshActor
//***********************************************

bool FActorTaskCreateSplineMeshActor::Execute()
{
	ALevelGenSplineMeshActor* SplineActor = ParentActor->GetWorld()->SpawnActor<ALevelGenSplineMeshActor>(ALevelGenSplineMeshActor::StaticClass(), ActorLoc, FRotator::ZeroRotator);

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

		SplineActor->ReregisterAllComponents();

		LevelBilderCell.CreatedMeshActors.push_back(SplineActor);
		return true;
	}

	return false;
}

//************************************************
//FActorTaskCreateProceduralActor
//***********************************************

bool FActorTaskCreateProceduralActor::Execute()
{
	if (MeshCount == 0)
	{
		ProceduralMeshActor = ParentActor->GetWorld()->SpawnActor<ALevelGenProceduralMeshActor>(ALevelGenProceduralMeshActor::StaticClass(), Location, FRotator::ZeroRotator);
		if (ProceduralMeshActor)
		{
			ProceduralMeshActor->SetCollision(Collision);
			LevelBilderCell.CreatedMeshActors.push_back(ProceduralMeshActor);
			MeshIT = FigureBufer.GetBuffer().begin();
		}
	}
	
	if (ProceduralMeshActor)
	{
		ProceduralMeshActor->AddMesh(MeshIT->second);
		
		MeshCount++;
		MeshIT++;

		if (MeshCount == FigureBufer.GetBuffer().size())
		{
			return true;
		}
	}

	return false;
}


//************************************************
//FActorTaskCreateBlueprintActor
//***********************************************

bool FActorTaskCreateBlueprintActor::Execute()
{
	ALevelGenActorBace* NewActor = ParentActor->GetWorld()->SpawnActor<ALevelGenActorBace>(ActorClass, ActorLocation, ActorRotation);

	if (NewActor)
	{
		NewActor->LevelGenerator = ParentActor;

		//NewActor->RerunConstructionScripts();
		
		LevelBilderCell.CreatedMeshActors.push_back(NewActor);

		return true;
	}

	return false;
}

//************************************************
//FActorTaskCreateHoverCar
//***********************************************

bool FActorTaskCreateHoverCar::Execute()
{
	std::shared_ptr<FHoverCar>pHoverCar = HoverCar.lock();

	if (pHoverCar)
	{
		AHoverCarActor* NewHoverCarActor = ParentActor->GetWorld()->SpawnActor<AHoverCarActor>(HoverCarClass, Coordinate, Tangent.Rotation());

		if (NewHoverCarActor)
		{
			NewHoverCarActor->SetHoverCar(HoverCar);
			pHoverCar->SetSpeed(NewHoverCarActor->Speed);
		}
		return true;
	}
	return false;
}




//************************************************
//AVirtualSpawner
//***********************************************

AVirtualSpawner::AVirtualSpawner()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AVirtualSpawner::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	
	std::unique_lock<std::mutex> Lock(DataLock, std::defer_lock);

	if (Lock.try_lock())
	{
		if (TaskQueue.size() > 0)
		{
			int ComplicityCount = 0;

			if (!TaskQueue.front())
			{
				TaskQueue.pop();
				return;
			}

			while (ComplicityCount + TaskQueue.front()->GetTaskcomplexity() <= ComplicitySpawnForTick)
			{
				
				if (TaskQueue.front()->Execute())
				{
					ComplicityCount += TaskQueue.front()->GetTaskcomplexity();
					TaskQueue.pop();
				}
				else
				{
					ComplicityCount += TaskQueue.front()->GetTaskcomplexity();
				}
				
				if (TaskQueue.size() == 0) break;

			}
		}

	}
	
	

}

void AVirtualSpawner::AddTaskToQueue(std::shared_ptr<FActorTaskBase> ActorTask)
{
	std::unique_lock<std::mutex> Lock(DataLock);

	TaskQueue.push(ActorTask);
}