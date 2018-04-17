// Copyright 2018 Pavlov Dmitriy
#pragma once

#pragma once
#include "Developer/AssetTools/Public/AssetToolsModule.h"
#include "Developer/AssetTools/Public/AssetTypeActions_Base.h"

class FHoverCarStorageAssetActions : public FAssetTypeActions_Base
{
public:
	FHoverCarStorageAssetActions(EAssetTypeCategories::Type InAssetCategory) : LevelGenCategory(InAssetCategory) {}

	virtual FText GetName() const override { return FText::FromString("HoverCars storage"); }

	virtual FColor GetTypeColor() const override { return FColor::Purple; }

	virtual UClass* GetSupportedClass() const override;

	virtual bool HasActions(const TArray<UObject*>& InObjects) const override { return false; }

	virtual uint32 GetCategories() { return LevelGenCategory; }

	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;

private:

	EAssetTypeCategories::Type LevelGenCategory;

};