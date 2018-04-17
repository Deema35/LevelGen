// Copyright 2018 Pavlov Dmitriy
#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "LevelTowers.h"
#include "Rooms/RoomBordersShowComponent.h"
#include "TowersBordersShowComponent.generated.h"

class FProceduralFigureBase;

UCLASS()
class LEVELGENPLUGINRUNTIME_API UTowersBordersShowComponentBace : public ULevelGenBordersShowComponentBace
{
	GENERATED_BODY()
public:

	void DrawTower();

	virtual FLevelTowerBase* GetTower() { return nullptr; }


#if WITH_EDITORONLY_DATA


	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif //WITH_EDITORONLY_DATA
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0))
		int32 CellSize = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0))
		int32 SecondCityFloor = 700;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0))
		int32 FirstCityFloor = 200;

	UPROPERTY(EditAnywhere)
		UMaterialInterface* FlooreMaterial = nullptr;

	UPROPERTY(EditAnywhere)
		UMaterialInterface* FirstCityFloorMaterial = nullptr;

	UPROPERTY(EditAnywhere)
		UMaterialInterface* SecondCityFloorMaterial = nullptr;

private:

	std::shared_ptr<FProceduralFigureBase> CreateWall(EDirection WallDirection, int WallHeight, int WallOffsetZ, FVector2D TowerSize, UMaterialInterface* CurrentWallMaterial);

};

UCLASS(BlueprintType)
class LEVELGENPLUGINRUNTIME_API UTowersBordersShowComponentTower : public UTowersBordersShowComponentBace
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FLevelTower Tower;

	virtual FLevelTowerBase* GetTower() override { return &Tower; }
};

UCLASS(BlueprintType)
class LEVELGENPLUGINRUNTIME_API UTowersBordersShowComponentBilding : public UTowersBordersShowComponentBace
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FLevelBilding Bilding;

	virtual FLevelTowerBase* GetTower() override { return &Bilding; }
};