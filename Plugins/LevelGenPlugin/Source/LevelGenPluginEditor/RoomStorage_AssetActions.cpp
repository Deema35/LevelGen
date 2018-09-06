// Copyright 2018 Pavlov Dmitriy
#include "RoomStorage_AssetActions.h"
#include "Rooms/RoomStorage.h"
#include "ActorStorage_AssetEditor.h"

FRoomStorageAssetActions::FRoomStorageAssetActions(EAssetTypeCategories::Type InAssetCategory) : LevelGenCategory(InAssetCategory)
{

}

UClass* FRoomStorageAssetActions::GetSupportedClass() const
{
	return URoomStorage::StaticClass();
}



void FRoomStorageAssetActions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
	const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;
	for (int i = 0; i < InObjects.Num(); i++)
	{
		URoomStorage* PropData = Cast<URoomStorage>(InObjects[i]);
		if (PropData)
		{
			if (!PropData->pAssetEditor)
			{
				TSharedRef<FRoomStorage_AssetEditor> NewCustEditor(new FRoomStorage_AssetEditor());
				PropData->pAssetEditor = &NewCustEditor.Get();
				NewCustEditor->InitAssetEditor_AssetEditorBase(Mode, EditWithinLevelEditor, PropData);
			}
		}
	}
}
