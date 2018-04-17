// Copyright 2018 Pavlov Dmitriy

#pragma once

#include "GameFramework/Actor.h"
#include <memory>
#include <vector>
#include "LevelGeneratorSettings.h"
#include "DataFillers.h"
#include "LevelBilders.h"
#include "LevelGenCore.h"
#include <random>   
#include <ctime>
#include "Towers/TowerStorage.h"
#include "LevelCell.h"
#include <future>
#include <queue>
#include "Future.h"
#include "LevelGenerator.generated.h"

class FLevelBilderBase;
class FDataFillerBase;
class UBillboardComponent;
struct FLevelCellBase;
struct FLevelRoomBase;
struct FLevelRoomNode;
struct FLevelRoomLink;
struct FLevelRoomRoadLink;
class URoomStorage;
class UHoverCarStorage;
struct FLevelTower;
enum class EDirection : uint8;
class FLevelGenRoadBase;
class AVirtualSpawner;
class FGenerateMashine;

USTRUCT()
struct FGenerateOrderModule
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, meta = (ClampMin = 0))
		int32 AddedGenerateRange = 0;


	UPROPERTY(VisibleAnywhere)
		TArray<EDataFiller> DataFillerObjects;

	UPROPERTY(VisibleAnywhere)
		TArray<ELevelBilder> LevelBilderObjects;


	void AddGenerateObject(EDataFiller GenerateObjectType);

	void AddGenerateObject(ELevelBilder GenerateObjectType);

	void GetGenerateObject(std::vector<EDataFiller>& GenerateObjects)
	{
		for (int i = 0; i < DataFillerObjects.Num(); i++)
		{
			GenerateObjects.push_back(DataFillerObjects[i]);
		}
	}

	void GetGenerateObject(std::vector<ELevelBilder>& GenerateObjects)
	{
		for (int i = 0; i < LevelBilderObjects.Num(); i++)
		{
			GenerateObjects.push_back(LevelBilderObjects[i]);
		}

	}
};

class URandomGenerator
{
public:
	URandomGenerator(int GeneratorSeed) : Generator(GeneratorSeed) {}

	int GetRandomNumber(int From, int To) const
	{
		std::uniform_int_distribution<> distr(From, To);
		return distr(Generator);
	}


	std::mt19937& GetGenerator() const { return Generator; }

private:

	mutable std::mt19937 Generator;
};





UCLASS()
class LEVELGENPLUGINRUNTIME_API ALevelGenerator : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
		FVector2D StartGenerateCoordinate;

	UPROPERTY(EditAnywhere)
		bool ShowModuleLeadTime = false;

	UPROPERTY(EditAnywhere)
		bool SetGenerateSeed = false;

	UPROPERTY(EditAnywhere)
		bool MainMenuLevel = false;

	UPROPERTY(EditAnywhere, meta =(EditCondition = "SetGenerateSeed"))
		int32 GenerateSeed = 0;

	UPROPERTY(EditAnywhere)
		TArray<FGenerateOrderModule> GenerateOrder;

	UPROPERTY(EditAnywhere)
		FLevelGeneratorSettings LevelSettings;

	UPROPERTY(EditAnywhere)
		UTowerStorage* TowerStorage;

	UPROPERTY(EditAnywhere)
		URoomStorage* RoomStorage;

	UPROPERTY(EditAnywhere)
		UHoverCarStorage* HoverCarStorage;

	std::unique_ptr<FDataStorage> DataStorage;

	std::mutex BildingTaskMutex;

	std::unique_ptr<FGenerateMashine> GenerateMashineOnlyDataFillers;
	
public:	

	ALevelGenerator();


	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
	
	void GenerateArea(FVector2D GenerateCenter);

	inline void GenerateSceneClick();

	void TestClick();

	inline void ReGenerateSceneClick();

	void DestroyAllMeshesClick();

	void SwitchShowModuleLeadTime();

	void DestroyAllMeshes();

	const UTowerStorage* GetTowerStorage() const { return TowerStorage; }

	const UHoverCarStorage* GetHoverCarStorage() const { return HoverCarStorage; }

	const URoomStorage* GeRoomStorage() const { return RoomStorage; }

	const std::vector<std::shared_ptr<FLevelRoomLink>>&  GetLevelLinks(FVector Coordinate, EDirection LinkDirection) const;

	URandomGenerator& GetRandomGenerator() { return *RandomGenerator; }

	const URandomGenerator& GetRandomGenerator() const {  return *RandomGenerator; }

	static bool GetPlayerStart(FVector& PlayerLocation, const ALevelGenerator& LevelGenerator, FVector2D ApproximateLocation);

	FVector2D GetLastBildCharacterLocation() const { return LastBildCharacterLocation; }

	void CreateBildingTask(FVector2D GenerateCenter);

	void AddBildingTask(std::shared_future<void> BildingFuture);

	static void AsyncGenerateArea(FGenerateMashine* GenerateMashine, FVector2D GenerateCenter, ALevelGenerator* LevelGenerator);

private:

	

	static void AsyncMovingHoverCar(float DeltaTime, FDataStorage* DataStorage, FLevelGeneratorSettings* LevelSettings);

	static void MoveCarOnTrack(FCarTrackGraphBase* CurrentTrack, float DeltaTime, int TrackLayerNum);

	
	void SetNewSettings();

	bool IsSettingsChange();

	void BildTimerMetod();

	

private:

	FTimerHandle BildTimer;

	std::future<void> HoverCarMovingFuture;

	FLevelGeneratorSettings CurrentLevelSettings;
	
	bool NeedRegenerateSettings = true;

	std::unique_ptr<FGenerateMashine> GenerateMashine;

	

	std::unique_ptr<URandomGenerator> RandomGenerator;

	FVector2D LastBildCharacterLocation;

	APawn* PlayerCharacter = nullptr;

	AVirtualSpawner* VirtualSpawner = nullptr;

	std::queue<std::shared_future<void>> BildingTaskQueue;

	TArray<std::unique_ptr<FGenerateOrderModule>> GenerateOrderOnlyDataFillers;



};


