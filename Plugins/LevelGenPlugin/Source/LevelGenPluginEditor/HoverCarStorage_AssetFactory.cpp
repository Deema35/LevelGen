// Copyright 2018 Pavlov Dmitriy
#include "HoverCarStorage_AssetFactory.h"
#include "HoverCars/HoverCarStorage.h"

UHoverCarStorageAssetFactory::UHoverCarStorageAssetFactory(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UHoverCarStorage::StaticClass();
}

UObject* UHoverCarStorageAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UHoverCarStorage>(InParent, Class, Name, Flags | RF_Transactional);
}