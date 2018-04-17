// Copyright 2018 Pavlov Dmitriy
#pragma once
#include "IDetailCustomization.h"
#include "SNotificationList.h"



class FLevelGeneratorDetailPanelCustomization : public IDetailCustomization
{
public:

	static FReply ClickGenerate(IDetailLayoutBuilder* DetailBuilder);

	static FReply ClickTest(IDetailLayoutBuilder* DetailBuilder);

	static FReply ClickRebildAndGenerate(IDetailLayoutBuilder* DetailBuilder);

	static FReply ClickDeleteActors(IDetailLayoutBuilder* DetailBuilder);

	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

	void OnPropertiesChanging(const FPropertyChangedEvent& e, IDetailLayoutBuilder* DetailBuilder);

	static void ShowNotification(FString Text, SNotificationItem::ECompletionState State = SNotificationItem::CS_None);

	static UObject* GetCustomizationObject(IDetailLayoutBuilder* DetailBuilder);

};