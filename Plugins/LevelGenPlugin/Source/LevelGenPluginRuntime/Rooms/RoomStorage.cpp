// Copyright 2018 Pavlov Dmitriy
#include "RoomStorage.h"
#include "LevelGraphNode.h"
#include "LevelRoomActor.h"
#include "LevelCell.h"
#include "LevelGenerator.h"
#include "LevelGeneratorSettings.h"
#include "ActorContainer.h"

//.....................................................
//URoomStorage
//.....................................................



void URoomStorage::GetLevelRooms(std::vector<FLevelRoomNode*>& CurrentLevelRooms, int MinLinkNumber, int GraphNodsFrequency) const
{
	CurrentLevelRooms.reserve(LevelRooms.Num());
	for (int i = 0; i < LevelRooms.Num(); i++)
	{
		if (LevelRooms[i]->LevelRoom)
		{
			ALevelRoomActorNode* CurrentRoom = Cast<ALevelRoomActorNode>(LevelRooms[i]->LevelRoom->GetDefaultObject());
			FVector Size = CurrentRoom->RoomBordersShowerNode->Room.GetSize();

			if (!(Size.X >= GraphNodsFrequency) || !(Size.Y >= GraphNodsFrequency))
			{
				std::vector<const FJointSlot*> Slots;
				CurrentRoom->RoomBordersShowerNode->Room.GetJointSlots(Slots);

				if (Slots.size() >= MinLinkNumber)
				{
					CurrentLevelRooms.push_back(&CurrentRoom->RoomBordersShowerNode->Room);
				}
			}
		}
		

	}


}



const std::vector<std::shared_ptr<FLevelRoomLink>>& URoomStorage::GetLevelLinks(bool IsRoadLinks, EDirection LinkDirection) const
{
	if (LinkDirection >= EDirection::end) throw;

	if (IsRoadLinks)
	{

		return AllDirectionsLevelRoadLinks[(int)LinkDirection];

	}
	else
	{

		return  AllDirectionsLevelLinks[(int)LinkDirection];

	}


}

void URoomStorage::CreateLinksForAllDirections()
{
	static_assert(EDirection::end == (EDirection)4, "Check directions");

	AllDirectionsLevelLinks.resize((int)EDirection::end);

	for (int i = 0; i < AllDirectionsLevelLinks.size(); i++)
	{
		AllDirectionsLevelLinks[i].reserve(LevelLinks.Num());
	}

	for (int i = 0; i < LevelLinks.Num(); i++)
	{
		for (int j = 0; j < (int)EDirection::end; j++)
		{
			if (LevelLinks[i]->LevelLink)
			{
				ALevelRoomActorLink* LevelRoomActorLink = static_cast<ALevelRoomActorLink*>(LevelLinks[i]->LevelLink.GetDefaultObject());

				AllDirectionsLevelLinks[j].push_back(std::shared_ptr<FLevelRoomLink>(new FLevelRoomLink(&LevelRoomActorLink->RoomBordersShowerLink->Room,
					LevelRoomActorLink->RoomBordersShowerLink->Room.GetInPutSlot()->Direction, (EDirection)(j))));
			}


		}

	}

	AllDirectionsLevelRoadLinks.resize((int)EDirection::end);

	for (int i = 0; i < AllDirectionsLevelRoadLinks.size(); i++)
	{
		AllDirectionsLevelRoadLinks[i].reserve(LevelRoadLinks.Num() + LevelTerraceLinks.Num());
	}

	for (int i = 0; i < LevelRoadLinks.Num(); i++)
	{
		for (int j = 0; j < (int)EDirection::end; j++)
		{
			if (LevelRoadLinks[i]->RoadLink)
			{
				ALevelRoomActorRoadLink* LevelRoomActorLink = static_cast<ALevelRoomActorRoadLink*>(LevelRoadLinks[i]->RoadLink.GetDefaultObject());

				AllDirectionsLevelRoadLinks[j].push_back(std::shared_ptr<FLevelRoomRoadLink>(new FLevelRoomRoadLink(&LevelRoomActorLink->RoomBordersShowerRoadLink->Room,
					LevelRoomActorLink->RoomBordersShowerRoadLink->Room.GetInPutSlot()->Direction, (EDirection)(j))));
			}

		}

	}

	for (int i = 0; i < LevelTerraceLinks.Num(); i++)
	{
		for (int j = 0; j < (int)EDirection::end; j++)
		{
			if (LevelTerraceLinks[i]->TerrasLink)
			{
				ALevelRoomActorTerraceLink* LevelRoomActorLink = static_cast<ALevelRoomActorTerraceLink*>(LevelTerraceLinks[i]->TerrasLink.GetDefaultObject());

				AllDirectionsLevelRoadLinks[j].push_back(std::shared_ptr<FLevelRoomTerraceLink>(new FLevelRoomTerraceLink(&LevelRoomActorLink->RoomBordersShowerTerraceLink->Room,
					LevelRoomActorLink->RoomBordersShowerTerraceLink->Room.GetInPutSlot()->Direction, (EDirection)(j))));
			}

		}

	}

}