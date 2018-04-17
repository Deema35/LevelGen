// Copyright 2018 Pavlov Dmitriy
#include "TowerStorage_AssetFactory.h"
#include "Towers/TowerStorage.h"

UTowerStorageAssetFactory::UTowerStorageAssetFactory(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UTowerStorage::StaticClass();
}

UObject* UTowerStorageAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UTowerStorage>(InParent, Class, Name, Flags | RF_Transactional);
}