// Copyright 2018 Pavlov Dmitriy
#include "RoomBordersShowComponentDetailPanelCustomization.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "IDetailsView.h"
#include "DetailCategoryBuilder.h"
#include "Rooms/RoomBordersShowComponent.h"
#include "Rooms/LevelRoomActor.h"
#include "Widgets/Input/SButton.h"

//******************************************************
//FRoomBordersShowComponentBase_DetailPanelCustomization
//*******************************************************

void FRoomBordersShowComponentBase_DetailPanelCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	IDetailCategoryBuilder& MyCategory = DetailBuilder.EditCategory("RoomProperty", FText::GetEmpty(), ECategoryPriority::Important);

	AddPropertysToRoomCategory(MyCategory);

	IDetailCategoryBuilder& MaterialCategory = DetailBuilder.EditCategory("RoomMaterials", FText::GetEmpty(), ECategoryPriority::Important);

	MaterialCategory.AddProperty(GET_MEMBER_NAME_CHECKED(URoomBordersShowComponentBace, FlooreMaterial), URoomBordersShowComponentBace::StaticClass());

	MaterialCategory.AddProperty(GET_MEMBER_NAME_CHECKED(URoomBordersShowComponentBace, BorderMaterial), URoomBordersShowComponentBace::StaticClass());

	MaterialCategory.AddProperty(GET_MEMBER_NAME_CHECKED(URoomBordersShowComponentBace, WallsMaterial), URoomBordersShowComponentBace::StaticClass());

	MaterialCategory.AddProperty(GET_MEMBER_NAME_CHECKED(URoomBordersShowComponentBace, InputSlotsMaterial), URoomBordersShowComponentBace::StaticClass());

	MaterialCategory.AddProperty(GET_MEMBER_NAME_CHECKED(URoomBordersShowComponentBace, OutputSlotsMaterial), URoomBordersShowComponentBace::StaticClass());

	MaterialCategory.AddProperty(GET_MEMBER_NAME_CHECKED(URoomBordersShowComponentBace, WrongSlotsMaterial), URoomBordersShowComponentBace::StaticClass());

}

void FRoomBordersShowComponentBase_DetailPanelCustomization::AddPropertysToRoomCategory(IDetailCategoryBuilder& RoomCategory)
{
	RoomCategory.AddProperty(GET_MEMBER_NAME_CHECKED(URoomBordersShowComponentBace, CellSize), URoomBordersShowComponentBace::StaticClass());

	RoomCategory.AddProperty(GET_MEMBER_NAME_CHECKED(URoomBordersShowComponentBace, CellHeight), URoomBordersShowComponentBace::StaticClass());

	RoomCategory.AddProperty(GET_MEMBER_NAME_CHECKED(URoomBordersShowComponentBace, WallTicness), URoomBordersShowComponentBace::StaticClass());

}




//******************************************************
//FRoomBordersShowComponentNode_DetailPanelCustomization
//*******************************************************


void FRoomBordersShowComponentNode_DetailPanelCustomization::AddPropertysToRoomCategory(IDetailCategoryBuilder& RoomCategory)
{
		
	RoomCategory.AddProperty(GET_MEMBER_NAME_CHECKED(URoomBordersShowComponentNode, Room), URoomBordersShowComponentNode::StaticClass());


	FRoomBordersShowComponentBase_DetailPanelCustomization::AddPropertysToRoomCategory(RoomCategory);
}





//******************************************************
//FRoomBordersShowComponentRoomLink_DetailPanelCustomization
//*******************************************************


void FRoomBordersShowComponentRoomLink_DetailPanelCustomization::AddPropertysToRoomCategory(IDetailCategoryBuilder& RoomCategory)
{

	RoomCategory.AddProperty(GET_MEMBER_NAME_CHECKED(URoomBordersShowComponentLink, Room), URoomBordersShowComponentLink::StaticClass());

	FRoomBordersShowComponentBase_DetailPanelCustomization::AddPropertysToRoomCategory(RoomCategory);

}

//******************************************************
//FRoomBordersShowComponentRoadLink_DetailPanelCustomization
//*******************************************************


void FRoomBordersShowComponentRoadLink_DetailPanelCustomization::AddPropertysToRoomCategory(IDetailCategoryBuilder& RoomCategory)
{

	RoomCategory.AddProperty(GET_MEMBER_NAME_CHECKED(URoomBordersShowComponentRoadLink, Room), URoomBordersShowComponentRoadLink::StaticClass());

	FRoomBordersShowComponentBase_DetailPanelCustomization::AddPropertysToRoomCategory(RoomCategory);

}

