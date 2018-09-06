// Copyright 2018 Pavlov Dmitriy
#include "TowerStorage_AssetActions.h"
#include "Towers/TowerStorage.h"
#include "ActorStorage_AssetEditor.h"



UClass* FTowerStorageAssetActions::GetSupportedClass() const
{
	return UTowerStorage::StaticClass();
}

uint32 FTowerStorageAssetActions::GetCategories()
{ 
	return LevelGenCategory;
}

void FTowerStorageAssetActions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
	const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;
	for (int i = 0; i < InObjects.Num(); i++)
	{
		UTowerStorage* PropData = Cast<UTowerStorage>(InObjects[i]);
		if (PropData)
		{
			if (!PropData->pAssetEditor)
			{
				TSharedRef<FTowerStorage_AssetEditor> NewCustEditor(new FTowerStorage_AssetEditor());
				PropData->pAssetEditor = &NewCustEditor.Get();
				NewCustEditor->InitAssetEditor_AssetEditorBase(Mode, EditWithinLevelEditor, PropData);
				FAssetEditorManager::Get().NotifyAssetOpened(PropData, PropData->pAssetEditor);
			}
		}
	}
}