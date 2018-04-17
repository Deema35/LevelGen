// Copyright 2018 Pavlov Dmitriy
#include"TowersBordersShowComponent.h"
#include "Rooms/RoomBordersShowComponent.h"
#include "ProceduralFigure.h"




void UTowersBordersShowComponentBace::DrawTower()
{
	ClearAllMeshSections();

	int ElementCounter = 0;

	FVector2D AbsRoomSize = GetTower()->GetSize();

	AbsRoomSize.X = AbsRoomSize.X * CellSize;

	AbsRoomSize.Y = AbsRoomSize.Y * CellSize;


	FProceduralFigureRectangle RectangleFloore(FVector(0, 0, 0), FVector(0, 0, 1), FVector(1, 0, 0), FVector2D(AbsRoomSize), FLevelGeneratorMaterialSettings(FlooreMaterial));
	DrawFigure(&RectangleFloore, ElementCounter++);

	FProceduralFigureRectangle RectangleRoof(FVector(0, 0, SecondCityFloor + FirstCityFloor), FVector(0, 0, -1), FVector(1, 0, 0), FVector2D(AbsRoomSize),
		FLevelGeneratorMaterialSettings(FlooreMaterial));
	DrawFigure(&RectangleRoof, ElementCounter++);

	for (int k = 0; k < (int)EDirection::end; k++)
	{
		std::shared_ptr<FProceduralFigureBase> FirstWall = CreateWall((EDirection)k, SecondCityFloor, 0.5 * SecondCityFloor, GetTower()->GetSize(), SecondCityFloorMaterial);
		DrawFigure(FirstWall.get(), ElementCounter++);

		std::shared_ptr<FProceduralFigureBase> SecondWall = CreateWall((EDirection)k, FirstCityFloor, 0.5 * FirstCityFloor + SecondCityFloor, GetTower()->GetSize(), FirstCityFloorMaterial);
		DrawFigure(SecondWall.get(), ElementCounter++);
	}

}


#if WITH_EDITORONLY_DATA


void UTowersBordersShowComponentBace::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	FName PropertyOwner = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.MemberProperty->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UTowersBordersShowComponentBace, CellSize) || PropertyName == GET_MEMBER_NAME_CHECKED(UTowersBordersShowComponentBace, FirstCityFloor) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(UTowersBordersShowComponentBace, SecondCityFloor) || PropertyName == GET_MEMBER_NAME_CHECKED(UTowersBordersShowComponentBace, FlooreMaterial)||
		PropertyName == GET_MEMBER_NAME_CHECKED(UTowersBordersShowComponentBace, SecondCityFloorMaterial) || PropertyName == GET_MEMBER_NAME_CHECKED(UTowersBordersShowComponentBace, FirstCityFloorMaterial))
	{
		DrawTower();
	}

	if (PropertyOwner == GET_MEMBER_NAME_CHECKED(UTowersBordersShowComponentTower, Tower) || PropertyOwner == GET_MEMBER_NAME_CHECKED(UTowersBordersShowComponentBilding, Bilding))
	{
		DrawTower();
	}
	
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif //WITH_EDITORONLY_DATA
std::shared_ptr<FProceduralFigureBase> UTowersBordersShowComponentBace::CreateWall(EDirection WallDirection, int WallHeight, int WallOffsetZ, FVector2D TowerSize, 
	UMaterialInterface* CurrentWallMaterial)
{
	FRotator RotateAngle;
	int DeltaWallOffsetX;
	int DeltaWallOffsetY;
	int DeltaWallOffsetZ = WallOffsetZ;
	int WallSize;


	switch (WallDirection)
	{
	case EDirection::XP:

		DeltaWallOffsetX = 0.5 * TowerSize.X  * CellSize;
		DeltaWallOffsetY = 0;
		WallSize = TowerSize.X * CellSize;

		RotateAngle = FRotator(0, 180, 0);

		break;

	case EDirection::XM:

		DeltaWallOffsetX = -0.5 * TowerSize.X  * CellSize;
		DeltaWallOffsetY = 0;
		WallSize = TowerSize.X * CellSize;

		RotateAngle = FRotator(0, 0, 0);
		break;

	case EDirection::YP:

		DeltaWallOffsetX = 0;
		DeltaWallOffsetY = 0.5 * TowerSize.Y  * CellSize;
		WallSize = TowerSize.Y * CellSize;

		RotateAngle = FRotator(0, 270, 0);
		break;

	case EDirection::YM:

		DeltaWallOffsetX = 0;
		DeltaWallOffsetY = -0.5 * TowerSize.Y  * CellSize;
		WallSize = TowerSize.Y * CellSize;

		RotateAngle = FRotator(0, 90, 0);
		break;

	default:

		throw ("Bad road direction");
	}

	std::shared_ptr<FProceduralFigureBase> NewWall = std::shared_ptr<FProceduralFigureRectangle>(new FProceduralFigureRectangle(FVector(DeltaWallOffsetX, DeltaWallOffsetY, DeltaWallOffsetZ),
		RotateAngle.Vector(), FVector(0, 0, 1), FVector2D(WallHeight, WallSize), FLevelGeneratorMaterialSettings(CurrentWallMaterial)));


	return NewWall;
}