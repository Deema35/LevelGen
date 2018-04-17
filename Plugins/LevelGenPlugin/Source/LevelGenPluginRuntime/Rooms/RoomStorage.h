// Copyright 2018 Pavlov Dmitriy
#pragma once
#include "CoreMinimal.h"
#include "LevelGenCore.h"
#include <memory>
#include <vector>
#include "RoomStorage.generated.h"

struct FLevelRoomBase;
struct FLevelRoomNode;
struct FLevelRoomLink;
struct FLevelBildingZoneLinkRoom;
struct FLevelCellBase;
struct FLevelGeneratorSettings;
struct FDataStorage;
enum class EDirection : uint8;
class FActorStorage_AssetEditorBase;
class URoomActorContainerRoomNode;
class URoomActorContainerRoomLink;
class URoomActorContainerRoadLink;
class URoomActorContainerTerraceLink;

UCLASS()
class LEVELGENPLUGINRUNTIME_API UActorsStorageBase : public UObject
{
	GENERATED_BODY()
public:

#if WITH_EDITORONLY_DATA

		FActorStorage_AssetEditorBase* pAssetEditor;

#endif //WITH_EDITORONLY_DATA
};

UCLASS()
class LEVELGENPLUGINRUNTIME_API URoomStorage : public UActorsStorageBase
{
	GENERATED_BODY()
public:

	URoomStorage() {}

	void GetLevelRooms(std::vector<FLevelRoomNode*>& CurrentLevelRooms, int MinLinkNumber, int GraphNodsFrequency) const;

	const std::vector<std::shared_ptr<FLevelRoomLink>>& GetLevelLinks(bool IsRoadLinks, EDirection LinkDirection) const;


	void CreateLinksForAllDirections();




	UPROPERTY()
	TArray<URoomActorContainerRoomNode*> LevelRooms;

	UPROPERTY()
	TArray<URoomActorContainerRoomLink*> LevelLinks;

	UPROPERTY()
	TArray<URoomActorContainerRoadLink*> LevelRoadLinks;

	UPROPERTY()
	TArray<URoomActorContainerTerraceLink*> LevelTerraceLinks;

private:
	std::vector<std::vector<std::shared_ptr<FLevelRoomLink>>> AllDirectionsLevelLinks;

	std::vector<std::vector<std::shared_ptr<FLevelRoomLink>>> AllDirectionsLevelRoadLinks;

	
};


