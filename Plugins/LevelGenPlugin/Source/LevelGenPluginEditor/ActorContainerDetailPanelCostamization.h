// Copyright 2018 Pavlov Dmitriy
#pragma once
#include "IDetailCustomization.h"
#include "SNotificationList.h"
class ULevelGenActorContainerBase;

class FActorContainerDetailPanelCostamization : public IDetailCustomization
{
public:

	static FReply ClickDelete(IDetailLayoutBuilder* DetailBuilder);

	static FReply ClickEdit(IDetailLayoutBuilder* DetailBuilder);


	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

	static ULevelGenActorContainerBase* GetCustomizationObject(IDetailLayoutBuilder* DetailBuilder);

};