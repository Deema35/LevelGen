// Copyright 2018 Pavlov Dmitriy
#pragma once

#include "CoreMinimal.h"
#include <vector>
#include <memory>
#include <utility>
#include <tuple>
#include <map>
#include "LevelGenCore.h"
#include "LevelRooms.generated.h"

struct FLevelCellBase;
struct FLevelGeneratorSettings;
struct FJointPart;
struct FDataStorage;

UENUM()
enum class ERoomType : uint8
{
	NodeRoom,
	GroundLinkRoom,
	RoadLinkRoom,
	TerraceLinkRoom,
	end
};


USTRUCT()
struct FJointSlot
{
	GENERATED_BODY()

	FJointSlot() = default;

	FJointSlot(FVector _RelativeCoordinate, EDirection _Direction) : RelativeCoordinate(_RelativeCoordinate), Direction(_Direction) {}

	UPROPERTY(EditAnywhere)
	EDirection Direction = EDirection::end;

	UPROPERTY(EditAnywhere)
	FVector RelativeCoordinate = FVector(0,0,0);

};



USTRUCT()
struct  FLevelRoomBase
{
	GENERATED_BODY()
public:

	FLevelRoomBase() = default;

	FLevelRoomBase(FLevelRoomBase* Room, EDirection BaseDirection, EDirection NewDirection);

	virtual ERoomType GetType() const { return ERoomType::end; }

	virtual void GetJointSlots(std::vector<const FJointSlot*>& JoinSlots) const { throw; }

	void  GetWalls(std::vector<const FJointSlot*>& RoomWalls) const;

	const FVector GetSize() const { return RoomSize; }

	static FVector RotateVector(const FVector& V, EDirection BaseDirection, EDirection NewDirection);

	static FRotator GetNewRotation(FRotator V, EDirection BaseDirection, EDirection NewDirection);

	static int GetDeltaRotation(EDirection BaseDirection, EDirection NewDirection);

	inline FVector GetRoomBegin(FVector RoomCoordinate) const;

	inline FVector GetRoomEnd(FVector RoomCoordinate) const;

	virtual const FJointSlot& GetJointSlot(int Num) const { throw; }

	UClass* GetRoomActorClass() const { return RoomActorClass; }

	int GetRoomRotation() const { return RoomRotation; }

	bool GetCanPlayerSpawn() const { return CanPlayerSpawn; }

	virtual ~FLevelRoomBase() {}
public:
	

	UPROPERTY(EditAnywhere)
		FVector RoomSize = FVector(1, 1, 1);

	UPROPERTY(EditAnywhere)
		TArray<FJointSlot> RoomWalls;

	UPROPERTY(EditAnywhere)
	bool CanPlayerSpawn = true;

	UPROPERTY()
	UClass* RoomActorClass = nullptr;

private:

	int RoomRotation = 0;

	

};

USTRUCT()
struct  FLevelRoomNode : public FLevelRoomBase
{
	GENERATED_BODY()
public:
	
	bool IsRoomFit(FVector RoomCoordinate, const FDataStorage& DataStorage);

	virtual ERoomType GetType() const override { return ERoomType::NodeRoom; }

	virtual const FJointSlot& GetJointSlot(int Num)  const override { return JoinSlots[Num]; }

	virtual void GetJointSlots(std::vector<const FJointSlot*>& _JoinSlots) const override;


public:

	UPROPERTY(EditAnywhere)
		TArray<FJointSlot> JoinSlots;

	

private:

	bool IsCellFit(int X, int Y, int Z, const FDataStorage& DataStorage);

};





USTRUCT()
struct  FLevelRoomLink : public FLevelRoomBase
{
	GENERATED_BODY()
public:
	FLevelRoomLink()
	{
		AddInputSlot(FVector(0, 0, 0), EDirection::XP);
		AddOutputSlot(FVector(0, 0, 0), EDirection::XM);
	};

	FLevelRoomLink(FLevelRoomLink* Room, EDirection BaseDirection, EDirection NewDirection);
	

	const FJointSlot* GetInPutSlot() const
	{
		return &GetJointSlot(InSlot);
	}

	const FJointSlot* GetOutPutSlot() const
	{
		return &GetJointSlot(OutSlot);
	}

	virtual ERoomType GetType() const override { return ERoomType::GroundLinkRoom; }

	void AddInputSlot(FVector RelatevCoordinate, EDirection Direction) { InSlot = JoinSlots.Add(FJointSlot(RelatevCoordinate, Direction)); }

	void AddOutputSlot(FVector RelatevCoordinate, EDirection Direction) { OutSlot = JoinSlots.Add(FJointSlot(RelatevCoordinate, Direction)); }

	int GetInSlotNum() const { return InSlot; }

	int GetOutSlotNum() const { return OutSlot; }

	virtual bool IsRoomFit(const FVector& RoomCoordinate, const FJointPart* InJointPart, const FJointPart* OutJointPart,
		const FDataStorage& DataStorage, const FLevelGeneratorSettings& LevelSettings);

	virtual const FJointSlot& GetJointSlot(int Num)  const override { return JoinSlots[Num]; }

	virtual void GetJointSlots(std::vector<const FJointSlot*>& _JoinSlots) const override;

	

public:

	UPROPERTY(EditAnywhere, EditFixedSize)
		TArray<FJointSlot> JoinSlots;

	
	
protected:

	bool IsCellFit(int X, int Y, int Z, const FJointPart* InJointPart, const FJointPart* OutJointPart,
		const FDataStorage& DataStorage, const FLevelGeneratorSettings& LevelSettings);

private:

UPROPERTY()
	int InSlot = 0;
UPROPERTY()
	int OutSlot = 0;
};

USTRUCT()
struct LEVELGENPLUGINRUNTIME_API FLevelRoomRoadLink : public FLevelRoomLink
{
	GENERATED_BODY()
public:

	FLevelRoomRoadLink() = default;

	FLevelRoomRoadLink(FLevelRoomRoadLink* Room, EDirection BaseDirection, EDirection NewDirection) : FLevelRoomLink(Room, BaseDirection, NewDirection) {}

	
	virtual ERoomType GetType() const override { return ERoomType::RoadLinkRoom; }

private:

	virtual bool IsRoomFit(const FVector& RoomCoordinate, const FJointPart* InJointPart, const FJointPart* OutJointPart,
		const FDataStorage& DataStorage, const FLevelGeneratorSettings& LevelSettings) override;


};


USTRUCT()
struct LEVELGENPLUGINRUNTIME_API FLevelRoomTerraceLink : public FLevelRoomLink
{
	GENERATED_BODY()
public:

	FLevelRoomTerraceLink() = default;

	FLevelRoomTerraceLink(FLevelRoomTerraceLink* Room, EDirection BaseDirection, EDirection NewDirection) : FLevelRoomLink(Room, BaseDirection, NewDirection) {}


	virtual ERoomType GetType() const override { return ERoomType::TerraceLinkRoom; }

private:


	virtual bool IsRoomFit(const FVector& RoomCoordinate, const FJointPart* InJointPart, const FJointPart* OutJointPart,
		const FDataStorage& DataStorage, const FLevelGeneratorSettings& LevelSettings) override;

};