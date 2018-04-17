// Copyright 2018 Pavlov Dmitriy
#pragma once
#include "EdGraph/EdGraphSchema.h"
#include "CoreMinimal.h"

class ULevelGenActorContainerBase;

struct FRoomStorage_EdGraphSchemaAction : public FEdGraphSchemaAction
{
	FRoomStorage_EdGraphSchemaAction(const FText& InNodeCategory, const FText& InMenuDesc, const FText& InToolTip, const int32 InGrouping, const int32 InSectionID,
		ULevelGenActorContainerBase* _RoomActorContainer)
		: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping, FText(), InSectionID), RoomActorContainer(_RoomActorContainer)
	{}

	ULevelGenActorContainerBase* RoomActorContainer;
};