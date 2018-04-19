// Copyright 2018 Pavlov Dmitriy
#include "RoomBordersShowComponent.h"
#include "ProceduralFigure.h"
#include "LevelGeneratorSettings.h"

//*************************************************
//ULevelGenBordersShowComponentBace
//***************************************************

ULevelGenBordersShowComponentBace::ULevelGenBordersShowComponentBace()
{
	bHiddenInGame = true;
	bUseEditorCompositing = true;
	bGenerateOverlapEvents = false;

}

void ULevelGenBordersShowComponentBace::DrawFigure(FProceduralFigureBase* Figure, int ElementNumber)
{


	FProcMeshSection Section;
	Figure->GetMeshSection(Section);
	Section.bEnableCollision = false;

	SetProcMeshSection(ElementNumber, Section);

	if (Figure->GetMaterial())
	{
		SetMaterial(ElementNumber, const_cast<UMaterialInterface*>(Figure->GetMaterial()));
	}

}


//*************************************************
//URoomBordersShowComponentBace
//***************************************************



void URoomBordersShowComponentBace::DrawRoom()
{
	ClearAllMeshSections();

	int ElementCounter = 0;

	FVector AbsRoomSize = GetRoom()->GetSize();

	AbsRoomSize.X = AbsRoomSize.X * CellSize - WallTicness;

	AbsRoomSize.Y = AbsRoomSize.Y * CellSize - WallTicness;

	AbsRoomSize.Z = AbsRoomSize.Z * CellHeight;

	

	FProceduralFigureRectangle RectangleFloore(FVector(0, 0, 0), FVector(0, 0, 1), FVector(1, 0, 0), FVector2D(AbsRoomSize), FLevelGeneratorMaterialSettings(FlooreMaterial));
	DrawFigure(&RectangleFloore, ElementCounter++);

	FProceduralFigureRectangle RectangleRoof(FVector(0, 0, AbsRoomSize.Z), FVector(0, 0, -1), FVector(1, 0, 0), FVector2D(AbsRoomSize), FLevelGeneratorMaterialSettings(FlooreMaterial));
	DrawFigure(&RectangleRoof, ElementCounter++);

	std::vector<const FJointSlot*> JoinSlots;

	GetRoom()->GetJointSlots(JoinSlots);

	std::vector<const FJointSlot*> RoomWalls;

	GetRoom()->GetWalls(RoomWalls);


	for (int i = 0; i < GetRoom()->GetSize().X; i++)
	{
		for (int j = 0; j < GetRoom()->GetSize().Y; j++)
		{
			for (int l = 0; l < GetRoom()->GetSize().Z; l++)
			{
				for (int k = 0; k < (int)EDirection::end; k++)
				{
					EBorderWallType WallType = EBorderWallType::Border;
					
					for (auto CurrentSlot = JoinSlots.begin(); CurrentSlot != JoinSlots.end(); CurrentSlot++)
					{
						if ((*CurrentSlot)->Direction != EDirection::end && (*CurrentSlot)->RelativeCoordinate == FVector(i, j, l) && EDirectionInvert((*CurrentSlot)->Direction) == (EDirection)k)
						{
							WallType = EBorderWallType::InputSlot;

							static_assert((int)ERoomType::end == 4, "Check draw room function");

							switch (GetRoom()->GetType())
							{
							case ERoomType::GroundLinkRoom:
							{
								URoomBordersShowComponentLink * LinkComponent = Cast<URoomBordersShowComponentLink>(this);

								if (LinkComponent->Room.GetOutPutSlot() == (*CurrentSlot)) WallType = EBorderWallType::OutPutSlot;

								break;
							}
							case ERoomType::RoadLinkRoom:
							{

								URoomBordersShowComponentRoadLink * RoadLinkComponent = Cast<URoomBordersShowComponentRoadLink>(this);

								if (RoadLinkComponent->Room.GetOutPutSlot() == (*CurrentSlot)) WallType = EBorderWallType::OutPutSlot;

								break;
							}
							case ERoomType::TerraceLinkRoom:

							{
								URoomBordersShowComponentTerraceLink * TerraceComponent = Cast<URoomBordersShowComponentTerraceLink>(this);

								if (TerraceComponent->Room.GetOutPutSlot() == (*CurrentSlot)) WallType = EBorderWallType::OutPutSlot;

								break;
							}
							case ERoomType::NodeRoom:

								break;

							default: throw;
							}
							
							break;
						}
						
					}
					for (auto CurrentRoomWall = RoomWalls.begin(); CurrentRoomWall != RoomWalls.end(); CurrentRoomWall++)
					{
						if ((*CurrentRoomWall)->Direction != EDirection::end && (*CurrentRoomWall)->RelativeCoordinate == FVector(i, j, l) &&
							EDirectionInvert((*CurrentRoomWall)->Direction) == (EDirection)k)
						{
							WallType = EBorderWallType::Wall;

							break;
						}

					}

					

					bool OnPerimetr = OnRoomPerimetr((EDirection)k, FVector(i, j, l), GetRoom()->GetSize());

					if (WallType == EBorderWallType::Border && !OnPerimetr)
					{
						WallType = EBorderWallType::Non;
					}
					else if ((WallType == EBorderWallType::InputSlot || WallType == EBorderWallType::OutPutSlot) && !OnPerimetr)
					{
						WallType = EBorderWallType::WrongSlot;
					}

					std::shared_ptr<FProceduralFigureBase> NewWall;

					NewWall = BuildWall(WallType, (EDirection)k, FVector(i, j, l));

					
					if (NewWall)
					{
						DrawFigure(NewWall.get(), ElementCounter++);
					}

				}
			}
		}
	}
	
	
}

std::shared_ptr<FProceduralFigureBase> URoomBordersShowComponentBace::BuildWall(EBorderWallType WallType, EDirection Direction, FVector Coordinate)
{

	switch (WallType)
	{
	case EBorderWallType::Border:

		return CreateWall(Direction, Coordinate, GetRoom()->GetSize(), BorderMaterial);
		

	case EBorderWallType::OutPutSlot:

		return CreateWall(Direction, Coordinate, GetRoom()->GetSize(), OutputSlotsMaterial, true);
		

	case EBorderWallType::InputSlot:

		return CreateWall(Direction, Coordinate, GetRoom()->GetSize(), InputSlotsMaterial, true);
		

	case EBorderWallType::WrongSlot:

		return CreateWall(Direction, Coordinate, GetRoom()->GetSize(), WrongSlotsMaterial, true);

	case EBorderWallType::Wall:

		return CreateWall(Direction, Coordinate, GetRoom()->GetSize(), WallsMaterial, true);
		

	case EBorderWallType::Non:

		return std::shared_ptr<FProceduralFigureBase>();

	default: throw;

	}

	
}

bool URoomBordersShowComponentBace::OnRoomPerimetr(EDirection WallDirection, FVector CellNumber, FVector RoomSize)
{
	if (WallDirection == EDirection::XP && CellNumber.X != RoomSize.X - 1) return false;
	if (WallDirection == EDirection::XM && CellNumber.X != 0) return false;
	if (WallDirection == EDirection::YP && CellNumber.Y != RoomSize.Y - 1) return false;
	if (WallDirection == EDirection::YM && CellNumber.Y != 0) return false;
	
	return true;
}



#if WITH_EDITORONLY_DATA
void URoomBordersShowComponentBace::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	FName PropertyOwner = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.MemberProperty->GetFName() : NAME_None;
	
	if (PropertyName == GET_MEMBER_NAME_CHECKED(URoomBordersShowComponentBace, CellSize) || PropertyName == GET_MEMBER_NAME_CHECKED(URoomBordersShowComponentBace, CellHeight) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(URoomBordersShowComponentBace, FlooreMaterial) || PropertyName == GET_MEMBER_NAME_CHECKED(URoomBordersShowComponentBace, BorderMaterial) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(URoomBordersShowComponentBace, InputSlotsMaterial) || PropertyName == GET_MEMBER_NAME_CHECKED(URoomBordersShowComponentBace, OutputSlotsMaterial)||
		PropertyName == GET_MEMBER_NAME_CHECKED(URoomBordersShowComponentBace, WallsMaterial) || PropertyName == GET_MEMBER_NAME_CHECKED(URoomBordersShowComponentBace, WrongSlotsMaterial))
	{
		DrawRoom();
	}

	else if (PropertyOwner == GET_MEMBER_NAME_CHECKED(URoomBordersShowComponentNode, Room))
	{
		DrawRoom();
	}
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif //WITH_EDITORONLY_DATA
std::shared_ptr<FProceduralFigureBase> URoomBordersShowComponentBace::CreateWall(EDirection WallDirection, FVector CellNumber, FVector RoomSize, UMaterialInterface* CurrentWallMaterial, bool TwoSide)
{
	FRotator RotateAngle;
	int DeltaWallOffsetX;
	int DeltaWallOffsetY;
	int DeltaWallOffsetZ = (CellNumber.Z + 0.5) * CellHeight;



	switch (WallDirection)
	{
	case EDirection::XP:

		DeltaWallOffsetX = (CellNumber.X - 0.5 * (RoomSize.X - 1) + 0.5) * CellSize - WallTicness/2;
		DeltaWallOffsetY = (CellNumber.Y - 0.5 * (RoomSize.Y - 1)) * CellSize;

		RotateAngle = FRotator(0, 180, 0);

		break;

	case EDirection::XM:

		DeltaWallOffsetX = (CellNumber.X - 0.5 * (RoomSize.X - 1) - 0.5) * CellSize + WallTicness / 2;
		DeltaWallOffsetY = (CellNumber.Y - 0.5 * (RoomSize.Y - 1)) * CellSize;

		RotateAngle = FRotator(0, 0, 0);
		break;

	case EDirection::YP:

		DeltaWallOffsetX = (CellNumber.X - 0.5 * (RoomSize.X - 1)) * CellSize;
		DeltaWallOffsetY = (CellNumber.Y - 0.5 * (RoomSize.Y - 1) + 0.5) * CellSize - WallTicness / 2;

		RotateAngle = FRotator(0, 270, 0);
		break;

	case EDirection::YM:

		DeltaWallOffsetX = (CellNumber.X - 0.5 * (RoomSize.X - 1)) * CellSize;
		DeltaWallOffsetY = (CellNumber.Y - 0.5 * (RoomSize.Y - 1) - 0.5) * CellSize + WallTicness / 2;

		RotateAngle = FRotator(0, 90, 0);
		break;

	default: throw;

		
	}

	std::shared_ptr<FProceduralFigureBase> NewWall = std::shared_ptr<FProceduralFigureRectangle>(new FProceduralFigureRectangle(FVector(DeltaWallOffsetX, DeltaWallOffsetY, DeltaWallOffsetZ),
		RotateAngle.Vector(), FVector(0, 0, 1), FVector2D(CellHeight, CellSize - WallTicness), FLevelGeneratorMaterialSettings(CurrentWallMaterial)));

	if (TwoSide)
	{
		FProceduralFigureRectangle SecondSide(FVector(DeltaWallOffsetX, DeltaWallOffsetY, DeltaWallOffsetZ),
			-(RotateAngle).Vector(), FVector(0, 0, 1), FVector2D(CellHeight, CellSize - WallTicness), FLevelGeneratorMaterialSettings(CurrentWallMaterial));

		NewWall->Merge(SecondSide, FVector(0, 0, 0));


	}


	return NewWall;

}