// Copyright 2018 Pavlov Dmitriy
#pragma once
#include "GameFramework/Actor.h"
#include "LevelRooms.h"
#include "RoomBordersShowComponent.h"
#include "LevelGeneratorSettings.h"
#include "LevelRoomActor.generated.h"

class ALevelGenerator;

UCLASS(Abstract)

class LEVELGENPLUGINRUNTIME_API ALevelGenActorBace : public AActor
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = "LevelGen")
		bool GetLevelGenSettings(FLevelGeneratorSettings& Settings);

	virtual ULevelGenBordersShowComponentBace* GetBordersShowComponent() { return nullptr; }

public:

	const ALevelGenerator* LevelGenerator = nullptr;

};


UCLASS(Abstract)
class LEVELGENPLUGINRUNTIME_API ALevelRoomActorNode : public ALevelGenActorBace
{
	GENERATED_BODY()
public:
	ALevelRoomActorNode();

	virtual ULevelGenBordersShowComponentBace* GetBordersShowComponent() override { return RoomBordersShowerNode; }
public:

	UPROPERTY(VisibleAnywhere)
		URoomBordersShowComponentNode* RoomBordersShowerNode = nullptr;

	
};

UCLASS(Abstract)
class LEVELGENPLUGINRUNTIME_API ALevelRoomActorLink : public ALevelGenActorBace
{
	GENERATED_BODY()
public:
	ALevelRoomActorLink();

	virtual ULevelGenBordersShowComponentBace* GetBordersShowComponent() override { return RoomBordersShowerLink; }

	UPROPERTY(VisibleAnywhere)
		URoomBordersShowComponentLink* RoomBordersShowerLink = nullptr;

	
};

UCLASS(Abstract)
class LEVELGENPLUGINRUNTIME_API ALevelRoomActorRoadLink : public ALevelGenActorBace
{
	GENERATED_BODY()
public:
	ALevelRoomActorRoadLink();

	virtual ULevelGenBordersShowComponentBace* GetBordersShowComponent() override { return RoomBordersShowerRoadLink; }

	UPROPERTY(VisibleAnywhere)
		URoomBordersShowComponentRoadLink* RoomBordersShowerRoadLink = nullptr;


};

UCLASS(Abstract)
class LEVELGENPLUGINRUNTIME_API ALevelRoomActorTerraceLink : public ALevelGenActorBace
{
	GENERATED_BODY()
public:
	ALevelRoomActorTerraceLink();

	virtual ULevelGenBordersShowComponentBace* GetBordersShowComponent() override { return RoomBordersShowerTerraceLink; }

	UPROPERTY(VisibleAnywhere)
		URoomBordersShowComponentTerraceLink* RoomBordersShowerTerraceLink = nullptr;


};