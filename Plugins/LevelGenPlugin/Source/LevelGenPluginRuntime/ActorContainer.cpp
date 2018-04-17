// Copyright 2018 Pavlov Dmitriy
#include "ActorContainer.h"
#include "Rooms/LevelRoomActor.h"
#include "Towers/LevelTowerActor.h"

static_assert(ELevelGenActorType::end == (ELevelGenActorType)7, "Check ULevelGenActorContainerBase::Create");

ULevelGenActorContainerBase* ULevelGenActorContainerBase::Create(UObject* PerrentObject, ELevelGenActorType ContainerType)
{

	switch (ContainerType)
	{
	case ELevelGenActorType::NodeRoom:

		return NewObject<ULevelGenActorContainerBase>(PerrentObject, URoomActorContainerRoomNode::StaticClass());

	case ELevelGenActorType::GroundLinkRoom:

		return NewObject<ULevelGenActorContainerBase>(PerrentObject, URoomActorContainerRoomLink::StaticClass());

	case ELevelGenActorType::RoadLinkRoom:

		return NewObject<ULevelGenActorContainerBase>(PerrentObject, URoomActorContainerRoadLink::StaticClass());

	case ELevelGenActorType::TerraceLinkRoom:

		return NewObject<ULevelGenActorContainerBase>(PerrentObject, URoomActorContainerTerraceLink::StaticClass());

	case ELevelGenActorType::Tower:

		return NewObject<UTowerActorContainer>(PerrentObject, UTowerActorContainer::StaticClass());

	case ELevelGenActorType::Bilding:

		return NewObject<UBildingActorContainer>(PerrentObject, UBildingActorContainer::StaticClass());

	case ELevelGenActorType::HoverCar:

		return NewObject<UHoverCarActorContainer>(PerrentObject, UHoverCarActorContainer::StaticClass());

	default: throw;
	}
}

//.....................................................
//URoomActorContainerRoomNode
//.....................................................

FVector URoomActorContainerRoomNode::GetSize() const
{
	if (!LevelRoom) return FVector::ZeroVector;

	ALevelRoomActorNode* CurrentRoom = Cast<ALevelRoomActorNode>(LevelRoom->GetDefaultObject());
	return CurrentRoom->RoomBordersShowerNode->Room.GetSize();

}

AActor* URoomActorContainerRoomNode::GetActor() const
{ 
	if (LevelRoom)
	{
		return Cast<AActor>(LevelRoom->GetDefaultObject());
	}
	return nullptr;
}

FString URoomActorContainerRoomNode::GetPathName() const
{
	return LevelRoom->GetPathName();
}

//.....................................................
//URoomActorContainerRoomLin
//.....................................................

FVector URoomActorContainerRoomLink::GetSize() const
{
	if (!LevelLink) return FVector::ZeroVector;

	ALevelRoomActorLink* CurrentRoom = Cast<ALevelRoomActorLink>(LevelLink->GetDefaultObject());


	return CurrentRoom->RoomBordersShowerLink->Room.GetSize();

}

AActor* URoomActorContainerRoomLink::GetActor() const
{
	if (LevelLink)
	{
		return Cast<AActor>(LevelLink->GetDefaultObject());
	}
	return nullptr;
}

FString URoomActorContainerRoomLink::GetPathName() const
{
	return LevelLink->GetPathName();
}

//.....................................................
//URoomActorContainerRoadLink
//.....................................................

FVector URoomActorContainerRoadLink::GetSize() const
{
	if (!RoadLink) return FVector::ZeroVector;

	ALevelRoomActorRoadLink* CurrentRoom = Cast<ALevelRoomActorRoadLink>(RoadLink->GetDefaultObject());

	return CurrentRoom->RoomBordersShowerRoadLink->Room.GetSize();

}

AActor* URoomActorContainerRoadLink::GetActor() const
{
	if (RoadLink)
	{
		return Cast<AActor>(RoadLink->GetDefaultObject());
	}
	return nullptr;
}

FString URoomActorContainerRoadLink::GetPathName() const
{
	return RoadLink->GetPathName();
}

//.....................................................
//URoomActorContainerTerraceLink
//.....................................................

FVector URoomActorContainerTerraceLink::GetSize() const
{
	if (!TerrasLink) return FVector::ZeroVector;

	ALevelRoomActorTerraceLink* CurrentRoom = Cast<ALevelRoomActorTerraceLink>(TerrasLink->GetDefaultObject());

	return CurrentRoom->RoomBordersShowerTerraceLink->Room.GetSize();

}

AActor* URoomActorContainerTerraceLink::GetActor() const
{
	if (TerrasLink)
	{
		return Cast<AActor>(TerrasLink->GetDefaultObject());
	}
	return nullptr;
}

FString URoomActorContainerTerraceLink::GetPathName() const
{
	return TerrasLink->GetPathName();
}
//.....................................................
//UTowerActorContainer
//.....................................................

FVector UTowerActorContainer::GetSize() const
{
	if (!Tower) return FVector::ZeroVector;

	ALevelTowerActorTower* CurrentTower = Cast<ALevelTowerActorTower>(Tower->GetDefaultObject());

	return FVector(CurrentTower->TowerBordersShower->Tower.GetSize(),0);

}

AActor* UTowerActorContainer::GetActor() const
{
	if (Tower)
	{
		return Cast<AActor>(Tower->GetDefaultObject());
	}
	return nullptr;
}

FString UTowerActorContainer::GetPathName() const
{
	return Tower->GetPathName(); 
}

//.....................................................
//UBildingActorContainer
//.....................................................

FVector UBildingActorContainer::GetSize() const
{
	if (!Bilding) return FVector::ZeroVector;

	ALevelTowerActorBilding* CurrentBilding = Cast<ALevelTowerActorBilding>(Bilding->GetDefaultObject());

	return FVector(CurrentBilding->BildingBordersShower->Bilding.GetSize(),0);

}

AActor* UBildingActorContainer::GetActor() const
{
	if (Bilding)
	{
		return Cast<AActor>(Bilding->GetDefaultObject());
	}
	return nullptr;
}

FString UBildingActorContainer::GetPathName() const
{
	return Bilding->GetPathName();
}

//.....................................................
//UBildingActorContainer
//.....................................................

AActor* UHoverCarActorContainer::GetActor() const
{
	
	if (HoverCar)
	{
		return Cast<AActor>(HoverCar->GetDefaultObject());
	}
	return nullptr;
}

FString UHoverCarActorContainer::GetPathName() const
{
	return HoverCar->GetPathName();
}