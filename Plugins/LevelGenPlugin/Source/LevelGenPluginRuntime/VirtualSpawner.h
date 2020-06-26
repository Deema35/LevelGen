// Copyright 2018 Pavlov Dmitriy
#pragma once
#include "GameFramework/Actor.h"
#include <memory>
#include <queue>
#include <mutex>
#include "ProceduralFigure.h"
#include "HoverCars/CarTrackGraph.h"
#include "VirtualSpawner.generated.h"

struct FLevelBilderCell;
class ALevelGenerator;
struct FTrackCoordinate;

enum class EActorTaskType
{
	DeleteActors,
	CreateSplineMesh,
	CreateStaticMesh,
	CreateProceduralActor,
	CreateBlueprintActor,
	CreateHoverCar
};

FString EActorTaskTypeToString(EActorTaskType TaskType);

struct FActorTaskBase
{
	FActorTaskBase(const ALevelGenerator* _ParentActor, const FLevelGeneratorSettings& _LevelSettings) :
		ParentActor(_ParentActor), LevelSettings(_LevelSettings) {}

	virtual ~FActorTaskBase() = default;

	virtual bool Execute() = 0;

	virtual int GetTaskcomplexity() const = 0;

	virtual EActorTaskType GetType() const = 0;

	const ALevelGenerator* ParentActor;

	const FLevelGeneratorSettings& LevelSettings;
};

struct FActorTaskFLevelBilderCellBase : public FActorTaskBase
{
	FActorTaskFLevelBilderCellBase(const ALevelGenerator* _ParentActor, const FLevelGeneratorSettings& _LevelSettings, FLevelBilderCell& _LevelBilderCell) :
		FActorTaskBase(_ParentActor, _LevelSettings), LevelBilderCell(_LevelBilderCell) {}

	FLevelBilderCell& LevelBilderCell;
};

struct FActorTaskDeleteActors : public FActorTaskFLevelBilderCellBase
{
	FActorTaskDeleteActors(FLevelBilderCell& _LevelBilderCell, const ALevelGenerator* _ParentActor, const FLevelGeneratorSettings& LevelSettings) :
		FActorTaskFLevelBilderCellBase( _ParentActor, LevelSettings, _LevelBilderCell)  {}

	virtual int GetTaskcomplexity() const override { return 1; }

	virtual bool Execute() override;

	virtual EActorTaskType GetType() const override { return EActorTaskType::DeleteActors; }

};

struct FActorTaskCreateStaticMeshActor : public FActorTaskFLevelBilderCellBase
{
	FActorTaskCreateStaticMeshActor(FLevelBilderCell& _LevelBilderCell, const ALevelGenerator* _ParentActor, UStaticMesh* _Mesh, FVector _Location, FRotator _Rotation, FVector _Scale, FName _ActorTag, bool _Collision,
		const FLevelGeneratorSettings& LevelSettings)
		: FActorTaskFLevelBilderCellBase(_ParentActor, LevelSettings, _LevelBilderCell), Mesh(_Mesh),
		Location(_Location), Rotation(_Rotation), Scale(_Scale), Collision(_Collision)
	{
		
	}

	virtual bool Execute() override;

	virtual int GetTaskcomplexity() const override { return 10; }

	virtual EActorTaskType GetType() const override { return EActorTaskType::CreateStaticMesh; }

	UStaticMesh* Mesh;

	FVector Location;

	FRotator Rotation;
	
	FVector Scale;
	
	bool Collision;
};

struct FActorTaskCreateSplineMeshActor : public FActorTaskFLevelBilderCellBase
{
	FActorTaskCreateSplineMeshActor(FLevelBilderCell& _LevelBilderCell, const ALevelGenerator* _ParentActor, UStaticMesh* _Mesh, FVector _ActorLoc, const std::vector<FVector> _Points, FVector2D _Scale, FName _ActorTag, bool _Collision,
		const FLevelGeneratorSettings& LevelSettings) :
		FActorTaskFLevelBilderCellBase(_ParentActor, LevelSettings, _LevelBilderCell), Mesh(_Mesh), Points(_Points), Scale(_Scale), ActorLoc(_ActorLoc), Collision(_Collision)
	{

	}

	virtual bool Execute() override;

	virtual int GetTaskcomplexity() const override { return 10; }

	virtual EActorTaskType GetType() const override { return EActorTaskType::CreateSplineMesh; }

	UStaticMesh* Mesh;

	const std::vector<FVector> Points;

	FVector2D Scale;

	FVector ActorLoc;

	bool Collision;
};

struct FActorTaskCreateProceduralActor : public FActorTaskFLevelBilderCellBase
{
public:

	FActorTaskCreateProceduralActor(FLevelBilderCell& _LevelBilderCell, const ALevelGenerator* _ParentActor, const FProceduralFigurBuffer _FigureBufer, FVector _Location, FName _ActorTag, bool _Collision,
		const FLevelGeneratorSettings& LevelSettings) :
		FActorTaskFLevelBilderCellBase( _ParentActor, LevelSettings, _LevelBilderCell), FigureBufer(_FigureBufer), Location(_Location), Collision(_Collision) {}

	virtual bool Execute() override;

	virtual int GetTaskcomplexity() const override { return 100; };

	virtual EActorTaskType GetType() const override { return EActorTaskType::CreateProceduralActor; }

public:

	const FProceduralFigurBuffer FigureBufer;

	FVector Location;

	bool Collision;

private:

	ALevelGenProceduralMeshActor* ProceduralMeshActor = nullptr;

};

struct FActorTaskCreateBlueprintActor : public FActorTaskFLevelBilderCellBase
{
	FActorTaskCreateBlueprintActor(FLevelBilderCell& _LevelBilderCell, const ALevelGenerator* _ParentActor, UClass* _ActorClass, FVector _ActorLocation, FRotator _ActorRotation, const FLevelGeneratorSettings& LevelSettings) :
		FActorTaskFLevelBilderCellBase(_ParentActor, LevelSettings, _LevelBilderCell), ActorClass(_ActorClass), ActorLocation(_ActorLocation), ActorRotation(_ActorRotation) {}

	virtual bool Execute() override;

	virtual int GetTaskcomplexity() const override { return 20; }

	virtual EActorTaskType GetType() const override { return EActorTaskType::CreateBlueprintActor; }

	UClass* ActorClass;

	FVector ActorLocation;

	FRotator ActorRotation;
};

struct FActorTaskCreateHoverCar : public FActorTaskBase
{
	FActorTaskCreateHoverCar(const ALevelGenerator* _ParentActor, UClass* _HoverCarClass, FVector _Coordinate, FVector _Tangent, std::weak_ptr<FHoverCar> _HoverCar, const FLevelGeneratorSettings& _LevelSettings) :
		FActorTaskBase(_ParentActor, _LevelSettings),HoverCarClass(_HoverCarClass), Coordinate(_Coordinate), Tangent(_Tangent), HoverCar(_HoverCar) {}

	virtual bool Execute() override;

	virtual int GetTaskcomplexity() const override { return 10; }

	virtual EActorTaskType GetType() const override { return EActorTaskType::CreateHoverCar; }

	UClass* HoverCarClass;

	FVector Coordinate;

	FVector Tangent;

	std::weak_ptr<FHoverCar> HoverCar;

};



UCLASS()

class AVirtualSpawner : public AActor
{
	GENERATED_BODY()
public:

	AVirtualSpawner();


	void AddTaskToQueue(std::unique_ptr<FActorTaskBase> ActorTask);

	virtual void Tick(float DeltaSeconds) override;

	void SetComplicitySpawn(int ComplicitySpawn) { ComplicitySpawnForTick = ComplicitySpawn; }

private:

	int ComplicitySpawnForTick;

	std::queue<std::unique_ptr<FActorTaskBase>> TaskQueue;

	std::mutex DataLock;

	
};