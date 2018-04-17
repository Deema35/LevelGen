// Copyright 2018 Pavlov Dmitriy
#pragma once
#include "IDetailCustomization.h"
#include "SNotificationList.h"

class URoomBordersShowComponentNode;
class IDetailCategoryBuilder;
class URoomBordersShowComponentBace;

class FRoomBordersShowComponentBase_DetailPanelCustomization : public IDetailCustomization
{
public:
	
	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

	virtual void AddPropertysToRoomCategory(IDetailCategoryBuilder& RoomCategory);

	

};

class FRoomBordersShowComponentNode_DetailPanelCustomization : public FRoomBordersShowComponentBase_DetailPanelCustomization
{
public:

	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance() { return MakeShareable(new FRoomBordersShowComponentNode_DetailPanelCustomization); }

	virtual void AddPropertysToRoomCategory(IDetailCategoryBuilder& RoomCategory) override;

};


class FRoomBordersShowComponentRoomLink_DetailPanelCustomization : public FRoomBordersShowComponentBase_DetailPanelCustomization
{
public:

	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance() { return MakeShareable(new FRoomBordersShowComponentRoomLink_DetailPanelCustomization); }

	virtual void AddPropertysToRoomCategory(IDetailCategoryBuilder& RoomCategory) override;

};

class FRoomBordersShowComponentRoadLink_DetailPanelCustomization : public FRoomBordersShowComponentBase_DetailPanelCustomization
{
public:

	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance() { return MakeShareable(new FRoomBordersShowComponentRoadLink_DetailPanelCustomization); }

	virtual void AddPropertysToRoomCategory(IDetailCategoryBuilder& RoomCategory) override;

};