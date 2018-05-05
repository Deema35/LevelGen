// Copyright 2018 Pavlov Dmitriy
#include "ActorStorage_AssetEditor.h"
#include "LevelGenRoad.h"
#include "IDetailsView.h"
#include "SGraphActionMenu.h"
#include "SSearchBox.h"
#include "SButton.h"
#include "SGraphPalette.h"
#include "EditorStyleSet.h"
#include "SDockTab.h"
#include "SImage.h"
#include "Rooms/LevelRoomActor.h"
#include "Rooms/RoomStorage.h"
#include "PropertyEditorModule.h"
#include "ModuleManager.h"
#include "LevelGenCore.h" 
#include "DetailLayoutBuilder.h"
#include "RoomStorage_EdGraphSchemaAction.h"
#include "MultiBoxBuilder.h"
#include "GenericCommands.h"
#include "MessageDialog.h"
#include "Towers/TowerStorage.h"
#include "ActorContainer.h"
#include "HoverCars/HoverCarStorage.h"
#include "AssetCommands_RoomStorage.h"
#include "SCustomEditorViewport.h"

const FName FCustomEditorTabs::DetailsID(TEXT("Details"));
const FName FCustomEditorTabs::ActionMenuID(TEXT("ActionMenu"));
const FName RoomEditorAppName = FName(TEXT("RoomEditorApp"));


//*************************************************
//FActorStorage_AssetEditorBase
//****************************************************


FActorStorage_AssetEditorBase::~FActorStorage_AssetEditorBase()
{
	EditedObject->pAssetEditor = nullptr;
}

FText FActorStorage_AssetEditorBase::GetToolkitName() const
{
	check(EditedObject != nullptr);

	return GetLabelForObject(EditedObject);
}

void FActorStorage_AssetEditorBase::InitAssetEditor_RoomStorage(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, UObject* ObjectToEdit)
{

	EditedObject = Cast<UActorsStorageBase>(ObjectToEdit);

	SAssignNew(FilterBox, SSearchBox)
		.OnTextChanged(this, &FActorStorage_AssetEditorBase::OnFilterTextChanged);


	GraphActionMenu = CreateActionMenuWidget();

	PreviewViewport = SNew(SCustomEditorViewport);


	const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("CustomEditor_Layout")
		->AddArea
		(
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.1f)
				->SetHideTabWell(true)
				->AddTab(GetToolbarTabId(), ETabState::OpenedTab)
			)
			->Split
			(
				FTabManager::NewSplitter()
				->SetOrientation(Orient_Horizontal)
				->SetSizeCoefficient(0.2f)
				
				->Split
				(

					FTabManager::NewStack()
					->SetSizeCoefficient(0.3f)
					->SetHideTabWell(true)
					->AddTab(FCustomEditorTabs::ActionMenuID, ETabState::OpenedTab)

				)
				->Split
				(

					FTabManager::NewStack()
					->SetSizeCoefficient(0.7f)
					->SetHideTabWell(true)
					->AddTab(FCustomEditorTabs::DetailsID, ETabState::OpenedTab)

				)


			)

		);

	
	BindCommands();

	InitAssetEditor(Mode, InitToolkitHost, RoomEditorAppName, StandaloneDefaultLayout, /*bCreateDefaultStandaloneMenu=*/ true, /*bCreateDefaultToolbar=*/ true, EditedObject);

}

void FActorStorage_AssetEditorBase::RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager)
{

	WorkspaceMenuCategory = TabManager->AddLocalWorkspaceMenuCategory(FText::FromString("Custom Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(TabManager);

	TabManager->RegisterTabSpawner(FCustomEditorTabs::ActionMenuID, FOnSpawnTab::CreateSP(this, &FActorStorage_AssetEditorBase::SpawnTab_ActionMenu))
		.SetDisplayName(FText::FromName(FCustomEditorTabs::ActionMenuID))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Viewports"));

	TabManager->RegisterTabSpawner(FCustomEditorTabs::DetailsID, FOnSpawnTab::CreateSP(this, &FActorStorage_AssetEditorBase::SpawnTab_Details))
		.SetDisplayName(FText::FromName(FCustomEditorTabs::DetailsID))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));


}

TSharedRef<SDockTab> FActorStorage_AssetEditorBase::SpawnTab_ActionMenu(const FSpawnTabArgs& Args)
{
	
	return SNew(SDockTab)
		.Label(FText::FromString("SGAction Menu"))
		.TabColorScale(GetTabColorScale())
		[
			SNew(SVerticalBox)

			+ SVerticalBox::Slot()
			.AutoHeight()
			.VAlign(VAlign_Center)
			[
				FilterBox.ToSharedRef()
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			.VAlign(VAlign_Center)
			[
				
				GraphActionMenu.ToSharedRef()
				
				
			]


		];

}

TSharedRef<SDockTab> FActorStorage_AssetEditorBase::SpawnTab_Details(const FSpawnTabArgs& Args)
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	const FDetailsViewArgs DetailsViewArgs(false, false, true, FDetailsViewArgs::HideNameArea, true, this);
	TSharedRef<IDetailsView> PropertyEditorRef = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	PropertyEditor = PropertyEditorRef;
	PropertyEditor->OnFinishedChangingProperties().AddSP(this, &FActorStorage_AssetEditorBase::OnPropertiesChanging);

	ViewPortSpawnedTab = SNew(SDockTab)
		.Label(FText::FromString("Details"))
		[
			SAssignNew(ViewportContainer, SVerticalBox)
	
		];

	RefreshViewPortContainer();

	PreviewViewport->SetParentTab(ViewPortSpawnedTab);
	
	return ViewPortSpawnedTab.ToSharedRef();
}

void FActorStorage_AssetEditorBase::UnregisterTabSpawners(const TSharedRef<class FTabManager>& TabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(TabManager);

	TabManager->UnregisterTabSpawner(FCustomEditorTabs::ActionMenuID);
	TabManager->UnregisterTabSpawner(FCustomEditorTabs::DetailsID);
}



void FActorStorage_AssetEditorBase::BindCommands()
{
	ToolkitCommands = MakeShareable(new FUICommandList);

	ToolkitCommands->MapAction(FGenericCommands::Get().Delete, FExecuteAction::CreateSP(this, &FActorStorage_AssetEditorBase::DeleteActorContainer));

	ToolkitCommands->MapAction(FAssetCommands_RoomStorage::Get().DeleteBlankConteyners, FExecuteAction::CreateSP(this, &FActorStorage_AssetEditorBase::DeleteBlankConteyners));

	TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);

	ToolbarExtender->AddToolBarExtension("Asset", EExtensionHook::After, ToolkitCommands, FToolBarExtensionDelegate::CreateRaw(this, &FActorStorage_AssetEditorBase::AddToolbarExtension));
	AddToolbarExtender(ToolbarExtender);
}

void FActorStorage_AssetEditorBase::AddToolbarExtension(FToolBarBuilder &builder)
{
	FSlateIcon IconDeleteBlankConteyners = FSlateIcon(FEditorStyle::GetStyleSetName(), "Cascade.DeleteLOD", "Cascade.DeleteLOD.Small");

	builder.AddToolBarButton(FAssetCommands_RoomStorage::Get().DeleteBlankConteyners, NAME_None, FText::FromString("Delete Blank Conteyners"),
		FText::FromString("Delete Blank Conteyners"), IconDeleteBlankConteyners, NAME_None);
}


//*************************************************
//MenuActions
//****************************************************
void FActorStorage_AssetEditorBase::OnFilterTextChanged(const FText& InFilterText)
{
	GraphActionMenu->GenerateFilteredItems(false);
}

TSharedRef<SGraphActionMenu> FActorStorage_AssetEditorBase::CreateActionMenuWidget()
{


	// create the main action list piece of this widget
	TSharedRef<SGraphActionMenu> _GraphActionMenu = SNew(SGraphActionMenu, false)
		
		.OnGetFilterText(this, &FActorStorage_AssetEditorBase::GetFilterText)
		.OnCreateWidgetForAction(this, &FActorStorage_AssetEditorBase::OnCreateWidgetForAction)
		.OnCollectAllActions(this, &FActorStorage_AssetEditorBase::OnCollectAllActions)
		.OnCollectStaticSections(this, &FActorStorage_AssetEditorBase::OnCollectStaticSections)
		.OnActionSelected(this, &FActorStorage_AssetEditorBase::OnActionSelected)
		.OnContextMenuOpening(this, &FActorStorage_AssetEditorBase::OnContextMenuOpening)
		.OnCanRenameSelectedAction(this, &FActorStorage_AssetEditorBase::OnCanRenameSelectedAction)
		.OnGetSectionTitle(this, &FActorStorage_AssetEditorBase::OnGetSectionTitle)
		.OnGetSectionWidget(this, &FActorStorage_AssetEditorBase::OnGetSectionWidget)
		.AlphaSortItems(false)
		.UseSectionStyling(true);


	return _GraphActionMenu;
}

FText FActorStorage_AssetEditorBase::GetFilterText() const
{
	return FilterBox->GetText();
}

TSharedRef<SWidget> FActorStorage_AssetEditorBase::OnCreateWidgetForAction(FCreateWidgetForActionData* const InCreateData)
{

	TSharedRef<SGraphPaletteItem> ActionWiget = SNew(SGraphPaletteItem, InCreateData);
	return  ActionWiget;
}

void FActorStorage_AssetEditorBase::OnCollectStaticSections(TArray<int32>& StaticSectionIDs)
{

	int Max = AssetActorTyps.size() + 1;
	for (int i = 0; i < Max; i++)
	{
		StaticSectionIDs.Add(i);
	}

}

void FActorStorage_AssetEditorBase::OnActionSelected(const TArray< TSharedPtr<FEdGraphSchemaAction> >& InActions, ESelectInfo::Type InSelectionType)
{
	if (InSelectionType == ESelectInfo::OnMouseClick || InSelectionType == ESelectInfo::OnKeyPress || InSelectionType == ESelectInfo::OnNavigation || InActions.Num() == 0)
	{
		if (InActions.Num() != 0)
		{
			ULevelGenActorContainerBase* ActorContainer = ((FRoomStorage_EdGraphSchemaAction*)InActions[0].Get())->RoomActorContainer;
			CurrentSelectedActionMenuObject = ActorContainer;
			PropertyEditor->SetObject(ActorContainer);
			
			RebildPreviewViewPort(ActorContainer->GetActorClass());
			
		}
	}
}


TSharedPtr<SWidget> FActorStorage_AssetEditorBase::OnContextMenuOpening()
{

	FMenuBuilder MenuBuilder(true, GetToolkitCommands());

	MenuBuilder.AddMenuEntry(FGenericCommands::Get().Delete);

	return MenuBuilder.MakeWidget();
}

FText FActorStorage_AssetEditorBase::OnGetSectionTitle(int32 InSectionID)
{
	if (InSectionID == 0) return FText::FromString("");
	return FText::FromString(GetEnumValueAsString<ELevelGenActorType>("ELevelGenActorType", AssetActorTyps[(InSectionID - 1)]) + (FString("s")));
}


TSharedRef<SWidget> FActorStorage_AssetEditorBase::OnGetSectionWidget(TSharedRef<SWidget> RowWidget, int32 InSectionID)
{

	TWeakPtr<SWidget> WeakRowWidget = RowWidget;

	FString DataString = GetEnumValueAsString<ELevelGenActorType>("ELevelGenActorType", AssetActorTyps[(InSectionID - 1)]);

	FText AddNewText = FText::FromString(DataString);
	FName MetaDataTag = FName(*(FString("Add New ") + DataString));



	return CreateAddToSectionButton(InSectionID, WeakRowWidget, AddNewText, MetaDataTag);
}



TSharedRef<SWidget> FActorStorage_AssetEditorBase::CreateAddToSectionButton(int32 InSectionID, TWeakPtr<SWidget> WeakRowWidget, FText AddNewText, FName MetaDataTag)
{
	return SNew(SButton)
		.ButtonStyle(FEditorStyle::Get(), "RoundButton")
		.ForegroundColor(FEditorStyle::GetSlateColor("DefaultForeground"))
		.ContentPadding(FMargin(2, 0))
		.OnClicked(this, &FActorStorage_AssetEditorBase::OnAddButtonClickedOnSection, InSectionID)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.AddMetaData<FTagMetaData>(FTagMetaData(MetaDataTag))
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(FMargin(0, 1))
			[
				SNew(SImage)
				.Image(FEditorStyle::GetBrush("Plus"))
			]

			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.AutoWidth()
			.Padding(FMargin(2, 0, 0, 0))
			[
				SNew(STextBlock)
				.Font(IDetailLayoutBuilder::GetDetailFontBold())
				.Text(AddNewText)
				.Visibility(EVisibility::Visible)
				.ShadowOffset(FVector2D(1, 1))
			]
		];
}


void FActorStorage_AssetEditorBase::RefreshActionMenuName()
{
	ULevelGenActorContainerBase* CurrentContainer = Cast<ULevelGenActorContainerBase>(CurrentSelectedActionMenuObject);
	if (CurrentContainer->GetActor())
	{
		CurrentContainer->ObjName = CurrentContainer->GetActor()->GetFName().ToString();
		CurrentContainer->ObjName.RemoveAt(CurrentContainer->ObjName.Len() - 2, 2);
		CurrentContainer->ObjName.RemoveAt(0, 9);
	}
	else
	{
		CurrentContainer->ObjName = "Blank";
	}
}

void FActorStorage_AssetEditorBase::RebildPreviewViewPort(UClass* ActorClass)
{
	if (!PreviewViewport->GetIsViewPortValid())
	{
		PreviewViewport = SNew(SCustomEditorViewport);

		RefreshViewPortContainer();

		PreviewViewport->SetParentTab(ViewPortSpawnedTab);
	}
	
	PreviewViewport->RebildScen(ActorClass);
}

void FActorStorage_AssetEditorBase::RefreshViewPortContainer()
{
	ViewportContainer->ClearChildren();
	ViewportContainer->AddSlot()
		[
			PreviewViewport.ToSharedRef()
		];

	ViewportContainer->AddSlot()
		.FillHeight(0.75)
		.VAlign(VAlign_Top)

		[
			PropertyEditor.ToSharedRef()
		];

	
}




//*************************************************
//FRoomStorage_AssetEditor
//****************************************************

FRoomStorage_AssetEditor::FRoomStorage_AssetEditor()
{
	AssetActorTyps.push_back(ELevelGenActorType::NodeRoom);
	AssetActorTyps.push_back(ELevelGenActorType::GroundLinkRoom);
	AssetActorTyps.push_back(ELevelGenActorType::RoadLinkRoom);
	AssetActorTyps.push_back(ELevelGenActorType::TerraceLinkRoom);
}



void FRoomStorage_AssetEditor::DeleteActorContainer()
{
	if (FMessageDialog::Open(EAppMsgType::OkCancel, FText::FromString("Delet object " + CurrentSelectedActionMenuObject->ObjName + "?")) == EAppReturnType::Ok)
	{
		URoomStorage* RoomStorage = Cast<URoomStorage>(EditedObject);

		switch (CurrentSelectedActionMenuObject->GetType())
		{

		case ELevelGenActorType::NodeRoom:

			RoomStorage->LevelRooms.RemoveSingle(Cast<URoomActorContainerRoomNode>(CurrentSelectedActionMenuObject));

			break;

		case ELevelGenActorType::GroundLinkRoom:

			RoomStorage->LevelLinks.RemoveSingle(Cast<URoomActorContainerRoomLink>(CurrentSelectedActionMenuObject));

			break;

		case ELevelGenActorType::RoadLinkRoom:

			RoomStorage->LevelRoadLinks.RemoveSingle(Cast<URoomActorContainerRoadLink>(CurrentSelectedActionMenuObject));

			break;

		case ELevelGenActorType::TerraceLinkRoom:

			RoomStorage->LevelTerraceLinks.RemoveSingle(Cast<URoomActorContainerTerraceLink>(CurrentSelectedActionMenuObject));

			break;

		default: throw;
		}

		CurrentSelectedActionMenuObject->ConditionalBeginDestroy();
		CurrentSelectedActionMenuObject = nullptr;
		GraphActionMenu->RefreshAllActions(true);
		EditedObject->Modify();
		PropertyEditor->SetObject(nullptr);
		RebildPreviewViewPort(nullptr);
	}
}


void FRoomStorage_AssetEditor::OnCollectAllActions(FGraphActionListBuilderBase& OutAllActions)
{
	URoomStorage* RoomStorage = Cast<URoomStorage>(EditedObject);

	for (int i = 0; i < RoomStorage->LevelRooms.Num(); i++)
	{

		TSharedPtr<FRoomStorage_EdGraphSchemaAction> NewVarAction = MakeShareable(new FRoomStorage_EdGraphSchemaAction(FText::FromString(RoomStorage->LevelRooms[i]->Category),
			FText::FromString(RoomStorage->LevelRooms[i]->ObjName), FText::FromString("PropertyTooltip"), 0, 1, RoomStorage->LevelRooms[i]));

		OutAllActions.AddAction(NewVarAction);
	}

	for (int i = 0; i < RoomStorage->LevelLinks.Num(); i++)
	{

		TSharedPtr<FRoomStorage_EdGraphSchemaAction> NewVarAction = MakeShareable(new FRoomStorage_EdGraphSchemaAction(FText::FromString(RoomStorage->LevelLinks[i]->Category),
			FText::FromString(RoomStorage->LevelLinks[i]->ObjName), FText::FromString("PropertyTooltip"), 0, 2, RoomStorage->LevelLinks[i]));

		OutAllActions.AddAction(NewVarAction);
	}

	for (int i = 0; i < RoomStorage->LevelRoadLinks.Num(); i++)
	{

		TSharedPtr<FRoomStorage_EdGraphSchemaAction> NewVarAction = MakeShareable(new FRoomStorage_EdGraphSchemaAction(FText::FromString(RoomStorage->LevelRoadLinks[i]->Category),
			FText::FromString(RoomStorage->LevelRoadLinks[i]->ObjName), FText::FromString("PropertyTooltip"), 0, 3, RoomStorage->LevelRoadLinks[i]));

		OutAllActions.AddAction(NewVarAction);
	}

	for (int i = 0; i < RoomStorage->LevelTerraceLinks.Num(); i++)
	{

		TSharedPtr<FRoomStorage_EdGraphSchemaAction> NewVarAction = MakeShareable(new FRoomStorage_EdGraphSchemaAction(FText::FromString(RoomStorage->LevelTerraceLinks[i]->Category),
			FText::FromString(RoomStorage->LevelTerraceLinks[i]->ObjName), FText::FromString("PropertyTooltip"), 0, 4, RoomStorage->LevelTerraceLinks[i]));

		OutAllActions.AddAction(NewVarAction);
	}
}




FReply FRoomStorage_AssetEditor::OnAddButtonClickedOnSection(int32 InSectionID)
{
	URoomStorage* RoomStorage = Cast<URoomStorage>(EditedObject);

	switch (AssetActorTyps[InSectionID - 1])
	{
	case ELevelGenActorType::NodeRoom:


		RoomStorage->LevelRooms.Add(Cast<URoomActorContainerRoomNode>(ULevelGenActorContainerBase::Create(RoomStorage, ELevelGenActorType::NodeRoom)));
		break;

	case ELevelGenActorType::GroundLinkRoom:

		RoomStorage->LevelLinks.Add(Cast<URoomActorContainerRoomLink>(ULevelGenActorContainerBase::Create(RoomStorage, ELevelGenActorType::GroundLinkRoom)));
		break;

	case ELevelGenActorType::RoadLinkRoom:

		RoomStorage->LevelRoadLinks.Add(Cast<URoomActorContainerRoadLink>(ULevelGenActorContainerBase::Create(RoomStorage, ELevelGenActorType::RoadLinkRoom)));
		break;

	case ELevelGenActorType::TerraceLinkRoom:

		RoomStorage->LevelTerraceLinks.Add(Cast<URoomActorContainerTerraceLink>(ULevelGenActorContainerBase::Create(RoomStorage, ELevelGenActorType::TerraceLinkRoom)));
		break;

	default: throw;
	}

	GraphActionMenu->RefreshAllActions(true);
	EditedObject->Modify();
	return FReply::Handled();
}

void FRoomStorage_AssetEditor::OnPropertiesChanging(const FPropertyChangedEvent& e)
{
	FName PropertyName = (e.Property != NULL) ? e.Property->GetFName() : NAME_None;

	TArray<TWeakObjectPtr<UObject>> EditObjects;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(ULevelGenActorContainerBase, Category))
	{
		GraphActionMenu->RefreshAllActions(true);
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(URoomActorContainerRoomNode, LevelRoom))
	{
		RefreshActionMenuName();
		GraphActionMenu->RefreshAllActions(true);
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(URoomActorContainerRoomLink, LevelLink))
	{
		RefreshActionMenuName();
		GraphActionMenu->RefreshAllActions(true);
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(URoomActorContainerRoadLink, RoadLink))
	{
		RefreshActionMenuName();
		GraphActionMenu->RefreshAllActions(true);
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(URoomActorContainerTerraceLink, TerrasLink))
	{
		RefreshActionMenuName();
		GraphActionMenu->RefreshAllActions(true);
	}

	

	EditedObject->Modify();

	PropertyEditor->SetObject(CurrentSelectedActionMenuObject, true);
}


void FRoomStorage_AssetEditor::DeleteBlankConteyners()
{
	URoomStorage* RoomStorage = Cast<URoomStorage>(EditedObject);

	if (!RoomStorage) throw;

	for (int i = 0; i < RoomStorage->LevelRooms.Num(); i++)
	{
		if (!RoomStorage->LevelRooms[i]->GetActor())
		{
			RoomStorage->LevelRooms.RemoveAt(i--);
		}
	}

	for (int i = 0; i < RoomStorage->LevelLinks.Num(); i++)
	{
		if (!RoomStorage->LevelLinks[i]->GetActor())
		{
			RoomStorage->LevelLinks.RemoveAt(i--);
		}
	}

	for (int i = 0; i < RoomStorage->LevelRoadLinks.Num(); i++)
	{
		if (!RoomStorage->LevelRoadLinks[i]->GetActor())
		{
			RoomStorage->LevelRoadLinks.RemoveAt(i--);
		}
	}

	for (int i = 0; i < RoomStorage->LevelTerraceLinks.Num(); i++)
	{
		if (!RoomStorage->LevelTerraceLinks[i]->GetActor())
		{
			RoomStorage->LevelTerraceLinks.RemoveAt(i--);
		}
	}

	GraphActionMenu->RefreshAllActions(true);
	EditedObject->Modify();
}

//*************************************************
//FTowerStorage_AssetEditor
//****************************************************

FTowerStorage_AssetEditor::FTowerStorage_AssetEditor()
{
	AssetActorTyps.push_back(ELevelGenActorType::Tower);
	AssetActorTyps.push_back(ELevelGenActorType::Bilding);
}




void FTowerStorage_AssetEditor::DeleteActorContainer()
{
	UTowerStorage* TowerStorage = Cast<UTowerStorage>(EditedObject);

	if (FMessageDialog::Open(EAppMsgType::OkCancel, FText::FromString("Delet object " + CurrentSelectedActionMenuObject->ObjName + "?")) == EAppReturnType::Ok)
	{
		switch (CurrentSelectedActionMenuObject->GetType())
		{

		case ELevelGenActorType::Tower:

			TowerStorage->Towers.RemoveSingle(Cast<UTowerActorContainer>(CurrentSelectedActionMenuObject));

			break;

		case ELevelGenActorType::Bilding:

			TowerStorage->Bildings.RemoveSingle(Cast<UBildingActorContainer>(CurrentSelectedActionMenuObject));

			break;

		default: throw;
		}

		CurrentSelectedActionMenuObject->ConditionalBeginDestroy();
		CurrentSelectedActionMenuObject = nullptr;
		GraphActionMenu->RefreshAllActions(true);
		EditedObject->Modify();
		PropertyEditor->SetObject(nullptr);
		RebildPreviewViewPort(nullptr);
	}
}


void FTowerStorage_AssetEditor::OnCollectAllActions(FGraphActionListBuilderBase& OutAllActions)
{
	UTowerStorage* TowerStorage = Cast<UTowerStorage>(EditedObject);

	for (int i = 0; i < TowerStorage->Towers.Num(); i++)
	{

		TSharedPtr<FRoomStorage_EdGraphSchemaAction> NewVarAction = MakeShareable(new FRoomStorage_EdGraphSchemaAction(FText::FromString(TowerStorage->Towers[i]->Category),
			FText::FromString(TowerStorage->Towers[i]->ObjName), FText::FromString("PropertyTooltip"), 0, 1, TowerStorage->Towers[i]));

		OutAllActions.AddAction(NewVarAction);
	}

	for (int i = 0; i < TowerStorage->Bildings.Num(); i++)
	{

		TSharedPtr<FRoomStorage_EdGraphSchemaAction> NewVarAction = MakeShareable(new FRoomStorage_EdGraphSchemaAction(FText::FromString(TowerStorage->Bildings[i]->Category),
			FText::FromString(TowerStorage->Bildings[i]->ObjName), FText::FromString("PropertyTooltip"), 0, 2, TowerStorage->Bildings[i]));

		OutAllActions.AddAction(NewVarAction);
	}

}



FReply FTowerStorage_AssetEditor::OnAddButtonClickedOnSection(int32 InSectionID)
{
	UTowerStorage* TowerStorage = Cast<UTowerStorage>(EditedObject);

	switch (AssetActorTyps[InSectionID  - 1])
	{
	case ELevelGenActorType::Tower:


		TowerStorage->Towers.Add(Cast<UTowerActorContainer>(ULevelGenActorContainerBase::Create(TowerStorage, ELevelGenActorType::Tower)));
		break;

	case ELevelGenActorType::Bilding:

		TowerStorage->Bildings.Add(Cast<UBildingActorContainer>(ULevelGenActorContainerBase::Create(TowerStorage, ELevelGenActorType::Bilding)));
		break;

	default: throw;
	}

	GraphActionMenu->RefreshAllActions(true);
	EditedObject->Modify();
	return FReply::Handled();
}

void FTowerStorage_AssetEditor::OnPropertiesChanging(const FPropertyChangedEvent& e)
{
	FName PropertyName = (e.Property != NULL) ? e.Property->GetFName() : NAME_None;

	TArray<TWeakObjectPtr<UObject>> EditObjects;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(ULevelGenActorContainerBase, Category))
	{
		GraphActionMenu->RefreshAllActions(true);
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UTowerActorContainer, Tower))
	{
		RefreshActionMenuName();
		GraphActionMenu->RefreshAllActions(true);
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UBildingActorContainer, Bilding))
	{
		RefreshActionMenuName();
		GraphActionMenu->RefreshAllActions(true);
	}

	

	EditedObject->Modify();

	PropertyEditor->SetObject(CurrentSelectedActionMenuObject, true);
}


void FTowerStorage_AssetEditor::DeleteBlankConteyners()
{
	UTowerStorage* TowerStorage = Cast<UTowerStorage>(EditedObject);

	if (!TowerStorage) throw;

	for (int i = 0; i < TowerStorage->Towers.Num(); i++)
	{
		if (!TowerStorage->Towers[i]->GetActor())
		{
			TowerStorage->Towers.RemoveAt(i--);
		}
	}

	for (int i = 0; i < TowerStorage->Bildings.Num(); i++)
	{
		if (!TowerStorage->Bildings[i]->GetActor())
		{
			TowerStorage->Bildings.RemoveAt(i--);
		}
	}

	

	GraphActionMenu->RefreshAllActions(true);
	EditedObject->Modify();
}

//*************************************************
//FHoverCarStorage_AssetEditor
//****************************************************

FHoverCarStorage_AssetEditor::FHoverCarStorage_AssetEditor()
{
	AssetActorTyps.push_back(ELevelGenActorType::HoverCar);
}




void FHoverCarStorage_AssetEditor::DeleteActorContainer()
{
	UHoverCarStorage* HoverCarStorage = Cast<UHoverCarStorage>(EditedObject);

	if (FMessageDialog::Open(EAppMsgType::OkCancel, FText::FromString("Delet object " + CurrentSelectedActionMenuObject->ObjName + "?")) == EAppReturnType::Ok)
	{
		HoverCarStorage->HoverCars.RemoveSingle(Cast<UHoverCarActorContainer>(CurrentSelectedActionMenuObject));
		
		CurrentSelectedActionMenuObject->ConditionalBeginDestroy();
		CurrentSelectedActionMenuObject = nullptr;
		GraphActionMenu->RefreshAllActions(true);
		EditedObject->Modify();
		PropertyEditor->SetObject(nullptr);
		RebildPreviewViewPort(nullptr);
	}
}


void FHoverCarStorage_AssetEditor::OnCollectAllActions(FGraphActionListBuilderBase& OutAllActions)
{
	UHoverCarStorage* HoverCarStorage = Cast<UHoverCarStorage>(EditedObject);

	for (int i = 0; i < HoverCarStorage->HoverCars.Num(); i++)
	{

		TSharedPtr<FRoomStorage_EdGraphSchemaAction> NewVarAction = MakeShareable(new FRoomStorage_EdGraphSchemaAction(FText::FromString(HoverCarStorage->HoverCars[i]->Category),
			FText::FromString(HoverCarStorage->HoverCars[i]->ObjName), FText::FromString("PropertyTooltip"), 0, 1, HoverCarStorage->HoverCars[i]));

		OutAllActions.AddAction(NewVarAction);
	}

	

}



FReply FHoverCarStorage_AssetEditor::OnAddButtonClickedOnSection(int32 InSectionID)
{
	UHoverCarStorage* HoverCarStorage = Cast<UHoverCarStorage>(EditedObject);

	HoverCarStorage->HoverCars.Add(Cast<UHoverCarActorContainer>(ULevelGenActorContainerBase::Create(HoverCarStorage, ELevelGenActorType::HoverCar)));
	
	GraphActionMenu->RefreshAllActions(true);
	EditedObject->Modify();
	return FReply::Handled();
}

void FHoverCarStorage_AssetEditor::OnPropertiesChanging(const FPropertyChangedEvent& e)
{
	FName PropertyName = (e.Property != NULL) ? e.Property->GetFName() : NAME_None;

	TArray<TWeakObjectPtr<UObject>> EditObjects;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(ULevelGenActorContainerBase, Category))
	{
		GraphActionMenu->RefreshAllActions(true);
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UHoverCarActorContainer, HoverCar))
	{
		RefreshActionMenuName();
		GraphActionMenu->RefreshAllActions(true);
	}
	

	

	EditedObject->Modify();

	PropertyEditor->SetObject(CurrentSelectedActionMenuObject, true);
}

void FHoverCarStorage_AssetEditor::DeleteBlankConteyners()
{
	UHoverCarStorage* HoverCarStorage = Cast<UHoverCarStorage>(EditedObject);

	if (!HoverCarStorage) throw;

	for (int i = 0; i < HoverCarStorage->HoverCars.Num(); i++)
	{
		if (!HoverCarStorage->HoverCars[i]->GetActor())
		{
			HoverCarStorage->HoverCars.RemoveAt(i--);
		}
	}


	GraphActionMenu->RefreshAllActions(true);
	EditedObject->Modify();
}
