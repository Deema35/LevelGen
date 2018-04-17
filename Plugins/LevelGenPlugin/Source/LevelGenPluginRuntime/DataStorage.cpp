// Copyright 2018 Pavlov Dmitriy
#include "DataStorage.h"
#include "LevelGeneratorSettings.h"
#include "LevelCell.h"

//******************************************************
//FDataStorage
//*******************************************************

FDataStorage::FDataStorage(const FLevelGeneratorSettings& _LevelSettings) : LevelSettings(_LevelSettings)
{ 
	
}

FDataStorage::~FDataStorage() = default;