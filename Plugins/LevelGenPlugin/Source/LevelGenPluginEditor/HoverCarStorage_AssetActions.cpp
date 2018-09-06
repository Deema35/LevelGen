// Copyright 2018 Pavlov Dmitriy
#include "HoverCarStorage_AssetActions.h"
#include "HoverCars/HoverCarStorage.h"
#include "ActorStorage_AssetEditor.h"



UClass* FHoverCarStorageAssetActions::GetSupportedClass() const
{
	return UHoverCarStorage::StaticClass();
}



void FHoverCarStorageAssetActions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
	const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;
	for (int i = 0; i < InObjects.Num(); i++)
	{
		UHoverCarStorage* PropData = Cast<UHoverCarStorage>(InObjects[i]);
		if (PropData)
		{
			if (!PropData->pAssetEditor)
			{
				TSharedRef<FHoverCarStorage_AssetEditor> NewCustEditor(new FHoverCarStorage_AssetEditor());
				PropData->pAssetEditor = &NewCustEditor.Get();
				NewCustEditor->InitAssetEditor_AssetEditorBase(Mode, EditWithinLevelEditor, PropData);
			}
		}
	}
}