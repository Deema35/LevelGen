// Copyright 2018 Pavlov Dmitriy
#pragma once
#include "Toolkits/AssetEditorToolkit.h"
#include "CoreMinimal.h"
#include "NotifyHook.h"
#include <vector>

class SGraphActionMenu;
class IDetailsView;
class ALevelGenActorBace;
class SSearchBox;
class URoomStorage;
class ULevelGenActorContainerBase;
class UTowerStorage;
struct FEdGraphSchemaAction;
enum class ELevelGenActorType : uint8;
class UActorsStorageBase;
class SCustomEditorViewport;

struct FCustomEditorTabs
{
	static const FName DetailsID;
	static const FName ActionMenuID;
};


class FActorStorage_AssetEditorBase : public FAssetEditorToolkit, public FNotifyHook
{

public:

	virtual ~FActorStorage_AssetEditorBase();

	// IToolkit interface
	virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;

	virtual void UnregisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;

	virtual FText GetToolkitName() const override;

	virtual FName GetToolkitFName() const override { return FName("RoomStorage Editor"); }

	virtual FText GetBaseToolkitName() const override { return FText::FromString("RoomStorage Editor"); }

	virtual FLinearColor GetWorldCentricTabColorScale() const override { return FLinearColor::White; }

	virtual FString GetWorldCentricTabPrefix() const override { return TEXT("RoomStorage Editor"); }

	void InitAssetEditor_RoomStorage(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, UObject* ObjectToEdit);

	virtual void DeleteActorContainer() = 0;

	virtual TSharedRef<FTabManager::FLayout> CreateWindowTabs();

protected:
	
	TSharedRef<SWidget> CreateAddToSectionButton(int32 InSectionID, TWeakPtr<SWidget> WeakRowWidget, FText AddNewText, FName MetaDataTag);

	void RefreshActionMenuName();

	void RebildPreviewViewPort(UClass* ActorClass);

protected:

	TSharedPtr<SGraphActionMenu> GraphActionMenu;

	TSharedPtr<IDetailsView> PropertyEditor;

	TSharedPtr<SCustomEditorViewport> PreviewViewport;

	TSharedPtr<SSearchBox> FilterBox;

	ULevelGenActorContainerBase* CurrentSelectedActionMenuObject = nullptr;

	std::vector<ELevelGenActorType> AssetActorTyps;

	UActorsStorageBase * EditedObject = nullptr;

	TSharedPtr<SVerticalBox> ViewportContainer;

	TSharedPtr<SDockTab> ViewPortSpawnedTab;

private:
	TSharedRef<class SGraphActionMenu> CreateActionMenuWidget();

	TSharedRef<SDockTab> SpawnTab_Details(const FSpawnTabArgs& Args);

	TSharedRef<SDockTab> SpawnTab_ActionMenu(const FSpawnTabArgs& Args);

	void OnFilterTextChanged(const FText& InFilterText);

	FText GetFilterText() const; 

	TSharedRef<SWidget> OnCreateWidgetForAction(struct FCreateWidgetForActionData* const InCreateData);

	virtual void OnCollectAllActions(struct FGraphActionListBuilderBase& OutAllActions) = 0;

	void OnCollectStaticSections(TArray<int32>& StaticSectionIDs);

	void OnActionSelected(const TArray< TSharedPtr<FEdGraphSchemaAction> >& InActions, ESelectInfo::Type InSelectionType);

	TSharedPtr<SWidget> OnContextMenuOpening();

	bool OnCanRenameSelectedAction(TWeakPtr<struct FGraphActionNode> InSelectedNode) const { return false; }

	FText OnGetSectionTitle(int32 InSectionID);

	TSharedRef<SWidget> OnGetSectionWidget(TSharedRef<SWidget> RowWidget, int32 InSectionID);

	virtual FReply OnAddButtonClickedOnSection(int32 InSectionID) = 0;

	virtual void OnPropertiesChanging(const FPropertyChangedEvent& e) = 0;

	void BindCommands();

	void AddToolbarExtension(FToolBarBuilder &builder);

	virtual void DeleteBlankConteyners() = 0;

	void RefreshViewPortContainer();

};

class FRoomStorage_AssetEditor : public FActorStorage_AssetEditorBase
{
public:

	FRoomStorage_AssetEditor();

	


private:
	virtual void DeleteActorContainer() override;

	virtual void OnCollectAllActions(struct FGraphActionListBuilderBase& OutAllActions) override;
	
	virtual FReply OnAddButtonClickedOnSection(int32 InSectionID) override;

	virtual void OnPropertiesChanging(const FPropertyChangedEvent& e) override;

	virtual void DeleteBlankConteyners() override;
};


class FTowerStorage_AssetEditor : public FActorStorage_AssetEditorBase
{
public:
	FTowerStorage_AssetEditor();

	

private:
	virtual void DeleteActorContainer() override;

	virtual void OnCollectAllActions(struct FGraphActionListBuilderBase& OutAllActions) override;

	virtual FReply OnAddButtonClickedOnSection(int32 InSectionID) override;

	virtual void OnPropertiesChanging(const FPropertyChangedEvent& e) override;

	virtual void DeleteBlankConteyners() override;

};

class FHoverCarStorage_AssetEditor : public FActorStorage_AssetEditorBase
{
public:
	FHoverCarStorage_AssetEditor();

private:
	virtual void DeleteActorContainer() override;

	virtual void OnCollectAllActions(struct FGraphActionListBuilderBase& OutAllActions) override;

	virtual FReply OnAddButtonClickedOnSection(int32 InSectionID) override;

	virtual void OnPropertiesChanging(const FPropertyChangedEvent& e) override;

	virtual void DeleteBlankConteyners() override;

};