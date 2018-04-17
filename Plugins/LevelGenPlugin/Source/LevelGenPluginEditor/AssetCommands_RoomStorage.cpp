// Copyright 2018 Pavlov Dmitriy
#include "AssetCommands_RoomStorage.h"

void FAssetCommands_RoomStorage::RegisterCommands()
{
#define LOCTEXT_NAMESPACE "LevelGen_Commands"
	UI_COMMAND(DeleteBlankConteyners, "Delete Blank Conteyners", "Delete Blank Conteyners", EUserInterfaceActionType::Button, FInputChord());
	
#undef LOCTEXT_NAMESPACE
}