// Copyright 2018 Pavlov Dmitriy
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorContainer.generated.h"

class ALevelRoomActorNode;
class ALevelRoomActorLink;
class ALevelRoomActorRoadLink;
class ALevelTowerActorTower;
class ALevelTowerActorBilding;
class ALevelGenActorBace;
class AHoverCarActor;
class ALevelRoomActorTerraceLink;

UENUM()
enum class ELevelGenActorType : uint8
{
	NodeRoom,
	GroundLinkRoom,
	RoadLinkRoom,
	TerraceLinkRoom,
	Tower,
	Bilding,
	HoverCar,
	end
};

UCLASS()
class LEVELGENPLUGINRUNTIME_API ULevelGenActorContainerBase : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY()
		FString ObjName = "Blank";

	UPROPERTY(EditAnywhere)
		FString Category = "Default";

	UPROPERTY(EditAnywhere)
		FString Comment;

	static ULevelGenActorContainerBase* Create(UObject* PerrentObject, ELevelGenActorType ContainerType);

	virtual ELevelGenActorType GetType() const { throw; }

	virtual FVector GetSize() const { return FVector::ZeroVector; }

	virtual AActor* GetActor() const { throw; }

	virtual FString GetPathName() const { throw; }

	virtual ~ULevelGenActorContainerBase() {}
};

UCLASS()
class LEVELGENPLUGINRUNTIME_API URoomActorContainerRoomNode : public ULevelGenActorContainerBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		TSubclassOf<ALevelRoomActorNode> LevelRoom;

	virtual ELevelGenActorType GetType() const override { return ELevelGenActorType::NodeRoom; }

	virtual FVector GetSize() const override;

	virtual AActor* GetActor() const override;

	virtual FString GetPathName() const override;
};

UCLASS()
class LEVELGENPLUGINRUNTIME_API URoomActorContainerRoomLink : public ULevelGenActorContainerBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		TSubclassOf<ALevelRoomActorLink> LevelLink;

	virtual ELevelGenActorType GetType() const override { return ELevelGenActorType::GroundLinkRoom; }

	virtual FVector GetSize() const override;

	virtual AActor* GetActor() const override;

	virtual FString GetPathName() const override;
};

UCLASS()
class LEVELGENPLUGINRUNTIME_API URoomActorContainerRoadLink : public ULevelGenActorContainerBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		TSubclassOf<ALevelRoomActorRoadLink> RoadLink;

	virtual ELevelGenActorType GetType() const override { return ELevelGenActorType::RoadLinkRoom; }

	virtual FVector GetSize() const override;

	virtual AActor* GetActor() const override;

	virtual FString GetPathName() const override;
};

UCLASS()
class LEVELGENPLUGINRUNTIME_API URoomActorContainerTerraceLink : public ULevelGenActorContainerBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		TSubclassOf<ALevelRoomActorTerraceLink> TerrasLink;

	virtual ELevelGenActorType GetType() const override { return ELevelGenActorType::TerraceLinkRoom; }

	virtual FVector GetSize() const override;

	virtual AActor* GetActor() const override;

	virtual FString GetPathName() const override;
};

UCLASS()
class LEVELGENPLUGINRUNTIME_API UTowerActorContainer : public ULevelGenActorContainerBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		TSubclassOf<ALevelTowerActorTower> Tower;

	virtual ELevelGenActorType GetType() const override { return ELevelGenActorType::Tower; }

	virtual FVector GetSize() const override;

	virtual AActor* GetActor() const;

	virtual FString GetPathName() const override;
};

UCLASS()
class LEVELGENPLUGINRUNTIME_API UBildingActorContainer : public ULevelGenActorContainerBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		TSubclassOf<ALevelTowerActorBilding> Bilding;

	virtual ELevelGenActorType GetType() const override { return ELevelGenActorType::Bilding; }

	virtual FVector GetSize() const override;

	virtual AActor* GetActor() const override;

	virtual FString GetPathName() const override;
};

UCLASS()
class LEVELGENPLUGINRUNTIME_API UHoverCarActorContainer : public ULevelGenActorContainerBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		TSubclassOf<AHoverCarActor> HoverCar;

	virtual ELevelGenActorType GetType() const override { return ELevelGenActorType::HoverCar; }

	virtual AActor* GetActor() const override;

	virtual FString GetPathName() const override;
};
