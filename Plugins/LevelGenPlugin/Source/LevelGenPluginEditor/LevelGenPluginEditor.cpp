// Copyright 2018 Pavlov Dmitriy

#include "LevelGenPluginEditor.h"
#include "PropertyEditorModule.h"
#include "LevelGeneratorDetailPanelCustomization.h"
#include "RoomBordersShowComponentDetailPanelCustomization.h"
#include "TowerStorage_AssetActions.h"
#include "RoomStorage_AssetActions.h"
#include "HoverCarStorage_AssetActions.h"
#include "ActorContainerDetailPanelCostamization.h"
#include "AssetCommands_RoomStorage.h"

void FLevelGenPluginEditorModule::StartupModule()
{
	// Detail panel costamization regestration
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	PropertyModule.RegisterCustomClassLayout("LevelGenerator", FOnGetDetailCustomizationInstance::CreateStatic(&FLevelGeneratorDetailPanelCustomization::MakeInstance));

	PropertyModule.RegisterCustomClassLayout("RoomBordersShowComponentNode", FOnGetDetailCustomizationInstance::CreateStatic(&FRoomBordersShowComponentNode_DetailPanelCustomization::MakeInstance));
	PropertyModule.RegisterCustomClassLayout("RoomBordersShowComponentLink", FOnGetDetailCustomizationInstance::CreateStatic(&FRoomBordersShowComponentRoomLink_DetailPanelCustomization::MakeInstance));
	PropertyModule.RegisterCustomClassLayout("RoomBordersShowComponentRoadLink", FOnGetDetailCustomizationInstance::CreateStatic(&FRoomBordersShowComponentRoadLink_DetailPanelCustomization::MakeInstance));

	PropertyModule.RegisterCustomClassLayout("LevelGenActorContainerBase", FOnGetDetailCustomizationInstance::CreateStatic(&FActorContainerDetailPanelCostamization::MakeInstance));

	// Asset actions regestration
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	
	EAssetTypeCategories::Type LevelGenCategory = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("LevelGen")), NSLOCTEXT("LevelGen","LevelGenAssetCategory", "LevelGen"));
	
	AssetTools.RegisterAssetTypeActions(MakeShareable(new FTowerStorageAssetActions(LevelGenCategory)));
	AssetTools.RegisterAssetTypeActions(MakeShareable(new FRoomStorageAssetActions(LevelGenCategory)));
	AssetTools.RegisterAssetTypeActions(MakeShareable(new FHoverCarStorageAssetActions(LevelGenCategory)));

	//Registrate ToolBarCommand 
	FAssetCommands_RoomStorage::Register();

}

void FLevelGenPluginEditorModule::ShutdownModule()
{

}

IMPLEMENT_GAME_MODULE(FLevelGenPluginEditorModule, LevelGenPluginEditor);