// Copyright 2018 Pavlov Dmitriy
#include "RoomStorage_AssetFactory.h"
#include "Rooms/RoomStorage.h"

URoomStorageAssetFactory::URoomStorageAssetFactory(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = URoomStorage::StaticClass();
}

UObject* URoomStorageAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<URoomStorage>(InParent, Class, Name, Flags | RF_Transactional);
}