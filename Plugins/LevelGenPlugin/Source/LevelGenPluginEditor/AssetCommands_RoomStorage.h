// Copyright 2018 Pavlov Dmitriy
#pragma once
#include "SlateBasics.h"
#include "Commands.h"
#include "EditorStyle.h"

class FAssetCommands_RoomStorage : public TCommands<FAssetCommands_RoomStorage>
{
public:

	FAssetCommands_RoomStorage() : TCommands<FAssetCommands_RoomStorage>(TEXT("LevelGen Commands"), FText::FromString("LevelGen Commands"), NAME_None, FEditorStyle::GetStyleSetName())
	{}

	virtual void RegisterCommands() override;
	TSharedPtr<FUICommandInfo> DeleteBlankConteyners;
	
};
