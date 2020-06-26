// Copyright 2018 Pavlov Dmitriy
#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "LevelRooms.h"
#include "RoomBordersShowComponent.generated.h"

class FProceduralFigureBase;

UCLASS()
class LEVELGENPLUGINRUNTIME_API ULevelGenBordersShowComponentBace : public UProceduralMeshComponent
{
	GENERATED_BODY()

public:

	ULevelGenBordersShowComponentBace(const FObjectInitializer& ObjectInitializer);

	~ULevelGenBordersShowComponentBace() {}

protected:

	void DrawFigure(FProceduralFigureBase* Figure, int ElementNumber);


};

enum class EBorderWallType : uint8
{
	Non,
	Border,
	WrongSlot,
	Wall,
	InputSlot,
	OutPutSlot
};

UCLASS()
class LEVELGENPLUGINRUNTIME_API URoomBordersShowComponentBace : public ULevelGenBordersShowComponentBace
{
	GENERATED_BODY()
public:


	void DrawRoom();


	bool OnRoomPerimetr(EDirection WallDirection, FVector CellNumber, FVector RoomSize);

	virtual const FLevelRoomBase* GetRoom() const { throw; }

#if WITH_EDITORONLY_DATA
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif //WITH_EDITORONLY_DATA
	

	UPROPERTY(EditAnywhere, meta = (ClampMin = 0))
		int32 CellSize = 2000;


	UPROPERTY(EditAnywhere, meta = (ClampMin = 0))
		int32 CellHeight = 400;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 0))
		int32 WallTicness = 20;

	UPROPERTY(EditAnywhere)
		UMaterialInterface* FlooreMaterial = nullptr;

	UPROPERTY(EditAnywhere)
		UMaterialInterface* BorderMaterial = nullptr;

	UPROPERTY(EditAnywhere)
		UMaterialInterface* InputSlotsMaterial = nullptr;

	UPROPERTY(EditAnywhere)
		UMaterialInterface* OutputSlotsMaterial = nullptr;

	UPROPERTY(EditAnywhere)
		UMaterialInterface* WrongSlotsMaterial = nullptr;

	UPROPERTY(EditAnywhere)
		UMaterialInterface* WallsMaterial = nullptr;
	
private:

	std::shared_ptr<FProceduralFigureBase> CreateWall(EDirection WallDirection, FVector CellNumber, FVector RoomSize, UMaterialInterface* CurrentWallMaterial, bool TwoSide = false);

	std::shared_ptr<FProceduralFigureBase> BuildWall(EBorderWallType WallType, EDirection Direction, FVector Coordinate);

	//bool bIsDrawComponent;
	
};

UCLASS()
class LEVELGENPLUGINRUNTIME_API URoomBordersShowComponentNode : public URoomBordersShowComponentBace
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	FLevelRoomNode Room;

	virtual const FLevelRoomBase* GetRoom() const override { return &Room; }
};

UCLASS()
class LEVELGENPLUGINRUNTIME_API URoomBordersShowComponentLink : public URoomBordersShowComponentBace
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	FLevelRoomLink Room;

	virtual const FLevelRoomBase* GetRoom() const override { return &Room; }
};

UCLASS()
class LEVELGENPLUGINRUNTIME_API URoomBordersShowComponentRoadLink : public URoomBordersShowComponentBace
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		FLevelRoomRoadLink Room;

	virtual const FLevelRoomBase* GetRoom() const override { return &Room; }
};

UCLASS()
class LEVELGENPLUGINRUNTIME_API URoomBordersShowComponentTerraceLink : public URoomBordersShowComponentBace
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		FLevelRoomTerraceLink Room;

	virtual const FLevelRoomBase* GetRoom() const override { return &Room; }
};