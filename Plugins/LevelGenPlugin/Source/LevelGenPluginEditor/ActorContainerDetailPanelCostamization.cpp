// Copyright 2018 Pavlov Dmitriy
#include "ActorContainerDetailPanelCostamization.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "IDetailsView.h"
#include "DetailCategoryBuilder.h"
#include "NotificationManager.h"
#include "SButton.h"
#include "STextBlock.h"
#include "AssetEditorManager.h"
#include "Rooms/RoomStorage.h"
#include "ActorStorage_AssetEditor.h"
#include "ActorContainer.h"
#include "LevelGenCore.h"

TSharedRef<IDetailCustomization> FActorContainerDetailPanelCostamization::MakeInstance()
{
	return MakeShareable(new FActorContainerDetailPanelCostamization);
}



void FActorContainerDetailPanelCostamization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	ULevelGenActorContainerBase* Object = GetCustomizationObject(&DetailBuilder);

	// Create a category so this is displayed early in the properties
	IDetailCategoryBuilder& ButtonCategory = DetailBuilder.EditCategory("Buttons", FText::GetEmpty(), ECategoryPriority::Important);
	IDetailsView& DetailsView = const_cast<IDetailsView&>(*DetailBuilder.GetDetailsView());

	ButtonCategory.AddCustomRow(FText::FromString("Buttons"))
		.ValueContent() //NameContent()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			[
				SNew(SButton)
				.Text(FText::FromString("Edit"))
				.OnClicked(FOnClicked::CreateStatic(&FActorContainerDetailPanelCostamization::ClickEdit, &DetailBuilder))
			]
			+ SHorizontalBox::Slot()
			[
				SNew(SButton)
				.Text(FText::FromString("Delete"))
				.OnClicked(FOnClicked::CreateStatic(&FActorContainerDetailPanelCostamization::ClickDelete, &DetailBuilder))
			]
			
		];

	IDetailCategoryBuilder& InfoCategory = DetailBuilder.EditCategory("Info", FText::GetEmpty(), ECategoryPriority::Important);

	ButtonCategory.AddCustomRow(FText::FromString("Info"))
		.ValueContent()
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(FText::FromString("Room type : "))
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
					
					.Text(FText::FromString(GetEnumValueAsString<ELevelGenActorType>("ELevelGenActorType", Object->GetType())))
				]
			]
			+ SVerticalBox::Slot()
			[
				SNew(SHorizontalBox)
				
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(FText::FromString("Room size : "))
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
					
					.Text(FText::FromString(Object->GetSize().ToString()))
				]
			]
		];

	/*MyCategory.AddProperty(GET_MEMBER_NAME_CHECKED(ALevelGenerator, SetGenerateSeed), ALevelGenerator::StaticClass());

	MyCategory.AddProperty(GET_MEMBER_NAME_CHECKED(ALevelGenerator, GenerateSeed), ALevelGenerator::StaticClass());

	MyCategory.AddProperty(GET_MEMBER_NAME_CHECKED(ALevelGenerator, ShowModuleLeadTime), ALevelGenerator::StaticClass());*/
}

ULevelGenActorContainerBase* FActorContainerDetailPanelCostamization::GetCustomizationObject(IDetailLayoutBuilder* DetailBuilder)
{
	TArray<TWeakObjectPtr<UObject>> OutObjects;
	DetailBuilder->GetObjectsBeingCustomized(OutObjects);
	ULevelGenActorContainerBase* Object = NULL;
	if (OutObjects.Num() > 0)
	{
		Object = Cast<ULevelGenActorContainerBase>(OutObjects[0].Get());
	}
	return Object;
}

FReply FActorContainerDetailPanelCostamization::ClickDelete(IDetailLayoutBuilder* DetailBuilder)
{
	ULevelGenActorContainerBase* Object = GetCustomizationObject(DetailBuilder);

	UActorsStorageBase* RoomStorage = Cast<UActorsStorageBase>(Object->GetOuter());
	
	RoomStorage->pAssetEditor->DeleteActorContainer();

	return FReply::Handled();
}


FReply FActorContainerDetailPanelCostamization::ClickEdit(IDetailLayoutBuilder* DetailBuilder)
{
	ULevelGenActorContainerBase* ActorContainer = GetCustomizationObject(DetailBuilder);

	if (ActorContainer)
	{
		TArray<FString> Name; 
		Name.Add(ActorContainer->GetPathName());
		FAssetEditorManager::Get().OpenEditorsForAssets(Name);
	}
	
	return FReply::Handled();
}