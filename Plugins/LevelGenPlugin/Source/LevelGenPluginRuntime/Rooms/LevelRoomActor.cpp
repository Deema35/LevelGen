// Copyright 2018 Pavlov Dmitriy
#include "LevelRoomActor.h"
#include "LevelGenerator.h"

bool ALevelGenActorBace::GetLevelGenSettings(FLevelGeneratorSettings& Settings)
{
	if (LevelGenerator)
	{
		Settings = LevelGenerator->LevelSettings;
		return true;
	}
		
	return false;
}

ALevelRoomActorNode::ALevelRoomActorNode()
{

	RoomBordersShowerNode = CreateDefaultSubobject<URoomBordersShowComponentNode>(TEXT("RoomBordersShowerNode"));
	RootComponent = RoomBordersShowerNode;
	RoomBordersShowerNode->Room.RoomActorClass = this->GetClass();
#if WITH_EDITORONLY_DATA
	RoomBordersShowerNode->DrawRoom();
#endif //WITH_EDITORONLY_DATA
}

ALevelRoomActorLink::ALevelRoomActorLink()
{

	RoomBordersShowerLink = CreateDefaultSubobject<URoomBordersShowComponentLink>(TEXT("RoomBordersShowerLink"));
	RootComponent = RoomBordersShowerLink;
	RoomBordersShowerLink->Room.RoomActorClass = this->GetClass();
#if WITH_EDITORONLY_DATA
	RoomBordersShowerLink->DrawRoom();
#endif //WITH_EDITORONLY_DATA
}

ALevelRoomActorRoadLink::ALevelRoomActorRoadLink()
{

	RoomBordersShowerRoadLink = CreateDefaultSubobject<URoomBordersShowComponentRoadLink>(TEXT("RoomBordersShowerRoadLink"));
	RootComponent = RoomBordersShowerRoadLink;
	RoomBordersShowerRoadLink->Room.RoomActorClass = this->GetClass();
#if WITH_EDITORONLY_DATA
	RoomBordersShowerRoadLink->DrawRoom();
#endif //WITH_EDITORONLY_DATA
}

ALevelRoomActorTerraceLink::ALevelRoomActorTerraceLink()
{

	RoomBordersShowerTerraceLink = CreateDefaultSubobject<URoomBordersShowComponentTerraceLink>(TEXT("RoomBordersShowerRoadLink"));
	RootComponent = RoomBordersShowerTerraceLink;
	RoomBordersShowerTerraceLink->Room.RoomActorClass = this->GetClass();
#if WITH_EDITORONLY_DATA
	RoomBordersShowerTerraceLink->DrawRoom();
#endif //WITH_EDITORONLY_DATA
}