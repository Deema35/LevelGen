// Copyright 2018 Pavlov Dmitriy
#include "LevelGeneratorDetailPanelCustomization.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "IDetailsView.h"
#include "DetailCategoryBuilder.h"
#include "NotificationManager.h"
#include "LevelGenerator.h"
#include "Widgets/Input/SButton.h"



TSharedRef<IDetailCustomization> FLevelGeneratorDetailPanelCustomization::MakeInstance()
{
	return MakeShareable(new FLevelGeneratorDetailPanelCustomization);
}



void FLevelGeneratorDetailPanelCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{

	// Create a category so this is displayed early in the properties
	IDetailCategoryBuilder& MyCategory = DetailBuilder.EditCategory("DebugPanell", FText::GetEmpty(), ECategoryPriority::Important);
	IDetailsView& DetailsView = const_cast<IDetailsView&>(*DetailBuilder.GetDetailsView());

	DetailsView.OnFinishedChangingProperties().AddSP(this, &FLevelGeneratorDetailPanelCustomization::OnPropertiesChanging, &DetailBuilder);
	
	MyCategory.AddCustomRow(FText::FromString("Button"))
		.ValueContent() //NameContent()
		[
			SNew(SButton)
			.Text(FText::FromString("Generate"))
			.OnClicked(FOnClicked::CreateStatic(&FLevelGeneratorDetailPanelCustomization::ClickGenerate, &DetailBuilder))
		];

	MyCategory.AddCustomRow(FText::FromString("Button"))
		.ValueContent() //NameContent()
		[
			SNew(SButton)
			.Text(FText::FromString("Rebild level"))
		.OnClicked(FOnClicked::CreateStatic(&FLevelGeneratorDetailPanelCustomization::ClickRebildAndGenerate, &DetailBuilder))
		];

	MyCategory.AddProperty(GET_MEMBER_NAME_CHECKED(ALevelGenerator, StartGenerateCoordinate), ALevelGenerator::StaticClass());

	MyCategory.AddCustomRow(FText::FromString("Button"))
		.ValueContent() //NameContent()
		[
			SNew(SButton)
			.Text(FText::FromString("Delete level"))
			.OnClicked(FOnClicked::CreateStatic(&FLevelGeneratorDetailPanelCustomization::ClickDeleteActors, &DetailBuilder))
		];

	MyCategory.AddCustomRow(FText::FromString("Test"))
		.ValueContent() //NameContent()
		[
			SNew(SButton)
			.Text(FText::FromString("Test"))
		.OnClicked(FOnClicked::CreateStatic(&FLevelGeneratorDetailPanelCustomization::ClickTest, &DetailBuilder))
		];
	
	MyCategory.AddProperty(GET_MEMBER_NAME_CHECKED(ALevelGenerator, SetGenerateSeed), ALevelGenerator::StaticClass());

	MyCategory.AddProperty(GET_MEMBER_NAME_CHECKED(ALevelGenerator, GenerateSeed), ALevelGenerator::StaticClass());

	MyCategory.AddProperty(GET_MEMBER_NAME_CHECKED(ALevelGenerator, ShowModuleLeadTime), ALevelGenerator::StaticClass());

	MyCategory.AddProperty(GET_MEMBER_NAME_CHECKED(ALevelGenerator, MainMenuLevel), ALevelGenerator::StaticClass());
}


FReply FLevelGeneratorDetailPanelCustomization::ClickGenerate(IDetailLayoutBuilder* DetailBuilder)
{
	ALevelGenerator* LevelGenerator = Cast<ALevelGenerator>(GetCustomizationObject(DetailBuilder));

	LevelGenerator->GenerateSceneClick();
	
	return FReply::Handled();
}

FReply FLevelGeneratorDetailPanelCustomization::ClickTest(IDetailLayoutBuilder* DetailBuilder)
{
	ALevelGenerator* LevelGenerator = Cast<ALevelGenerator>(GetCustomizationObject(DetailBuilder));

	LevelGenerator->TestClick();

	return FReply::Handled();
}

FReply FLevelGeneratorDetailPanelCustomization::ClickRebildAndGenerate(IDetailLayoutBuilder* DetailBuilder)
{
	ALevelGenerator* LevelGenerator = Cast<ALevelGenerator>(GetCustomizationObject(DetailBuilder));

	LevelGenerator->ReGenerateSceneClick();

	return FReply::Handled();
}

FReply FLevelGeneratorDetailPanelCustomization::ClickDeleteActors(IDetailLayoutBuilder* DetailBuilder)
{
	ALevelGenerator* LevelGenerator = Cast<ALevelGenerator>(GetCustomizationObject(DetailBuilder));

	LevelGenerator->DestroyAllMeshesClick();

	return FReply::Handled();
}



void FLevelGeneratorDetailPanelCustomization::ShowNotification(FString Text, SNotificationItem::ECompletionState State)
{
	FNotificationInfo Info(FText::FromString(Text));
	Info.bFireAndForget = true;
	Info.FadeOutDuration = 1.0f;
	Info.ExpireDuration = 2.0f;

	TWeakPtr<SNotificationItem> NotificationPtr = FSlateNotificationManager::Get().AddNotification(Info);
	if (NotificationPtr.IsValid())
	{
		NotificationPtr.Pin()->SetCompletionState(State);
	}
}

UObject* FLevelGeneratorDetailPanelCustomization::GetCustomizationObject(IDetailLayoutBuilder* DetailBuilder)
{
	TArray<TWeakObjectPtr<UObject>> OutObjects;
	DetailBuilder->GetObjectsBeingCustomized(OutObjects);
	ALevelGenerator* Object = NULL;
	if (OutObjects.Num() > 0)
	{
		Object = Cast<ALevelGenerator>(OutObjects[0].Get());
	}
	return Object;
}

void FLevelGeneratorDetailPanelCustomization::OnPropertiesChanging(const FPropertyChangedEvent& e, IDetailLayoutBuilder* DetailBuilder)
{
	FName PropertyName = (e.Property != NULL) ? e.Property->GetFName() : NAME_None;

	ALevelGenerator* LevelGenerator = Cast<ALevelGenerator>(GetCustomizationObject(DetailBuilder));

	if (PropertyName == GET_MEMBER_NAME_CHECKED(ALevelGenerator, ShowModuleLeadTime))
	{
		LevelGenerator->SwitchShowModuleLeadTime();
	}
}