// Copyright 2018 Pavlov Dmitriy
#include "SearchGraph.h"
#include "LevelGenerator.h"
#include "Rooms/PlacedLevelRoom.h"
#include "DataStorage.h"

const FValidationResult FValidationResult::Zero(0, 0);


USearchGraph::USearchGraph(FJointPart& In, FJointPart& Out, const FDataStorage& _DataStorage, const ALevelGenerator& _LevelGenerator) : DataStorage(_DataStorage), LevelGenerator(_LevelGenerator)
{
	if (!DataStorage.LevelMap.IsCellExist(FVector2D(In.GetCoordinate())) || !DataStorage.LevelMap.GetCell(FVector2D(In.GetCoordinate()))->CellInst)
	{
		throw FString("Input joint part out of iniciate area");
	}
		
	if (!DataStorage.LevelMap.IsCellExist(FVector2D(Out.GetCoordinate())) || !DataStorage.LevelMap.GetCell(FVector2D(Out.GetCoordinate()))->CellInst)
	{
		throw FString("Out joint part out of iniciate area");
	}

	std::vector<std::pair<FValidationResult, std::shared_ptr<FSearchGraphNode>>> NewNods;

	CreateNewSearchGraphNods(NewNods, In, Out, std::shared_ptr<FSearchGraphNode>(nullptr), FValidationResult::Zero);

	for (int i = 0; i < NewNods.size(); i++)
	{
		Graph.insert(NewNods[i]);
	}

	if (NewNods.size() == 0) throw FString("Cann't create SearchGraph because not start nods");

	int i = 0;
	while (true)
	{
		if (Graph.find(FValidationResult::Zero) != Graph.end()) // if serch graph create return
		{
			auto it = Graph.find(FValidationResult::Zero);

			if (!IsSelfIntersectionPresent(it->second))
			{
				return;
			}
			else
			{
				throw FString("Cann't create SearchGraph because it has self intersection");
			}
		}
				
		NewNods.clear();

		std::shared_ptr<FSearchGraphNode> CurrentNode;
		FValidationResult LastValidationResult;

		for (auto it = Graph.begin(); it != Graph.end(); it++)
		{
			if (!it->second->IsProcessed())
			{
				CurrentNode = it->second;
				LastValidationResult = it->first;
				break;
			}
		}

		if (!CurrentNode)
		{
			throw FString("All SearchGraphNode nods already processed, no options available");
		}

		if (DataStorage.LevelMap.GetCell(FVector2D(CurrentNode->GetOutPutJointSlot()->ConnectedJoinPart->GetLinkedJointPart()->GetCoordinate()))->CellInst)
		{
			CreateNewSearchGraphNods(NewNods, *CurrentNode->GetOutPutJointSlot()->ConnectedJoinPart->GetLinkedJointPart(), Out, CurrentNode, LastValidationResult);
		}

		CurrentNode->SetProcessed();

		for (int i = 0; i < NewNods.size(); i++)
		{
			Graph.insert(NewNods[i]);
		}

		if (i++ > 200) throw FString("Cycle limit, very long link");
	}

}




void USearchGraph::CreateNewSearchGraphNods(std::vector<std::pair<FValidationResult, std::shared_ptr<FSearchGraphNode>>>& NewNods, FJointPart& InPortal, const FJointPart& OutPortal,
	std::weak_ptr<FSearchGraphNode> LinkedNode, FValidationResult LastValidationResult)
{
	
	const std::vector<std::shared_ptr<FLevelRoomLink>>& LinkRooms = LevelGenerator.GetLevelLinks(InPortal.GetCoordinate(), InPortal.GetDirection());

	
	if (LinkRooms.size() == 0) throw FString("Cann't create link, because have not necessary link rooms");

	for (int i = 0; i < LinkRooms.size(); i++)
	{
		FVector NewNodeStartCoordinate = InPortal.GetCoordinate() - LinkRooms[i]->GetInPutSlot()->RelativeCoordinate;

		if (LinkRooms[i]->IsRoomFit(NewNodeStartCoordinate, &InPortal, &OutPortal, DataStorage, LevelGenerator.LevelSettings))
		{
			
			std::shared_ptr<FSearchGraphNode> NewLinkRoom = std::shared_ptr<FSearchGraphNode>(new FSearchGraphNode(LinkRooms[i].get(), InPortal, NewNodeStartCoordinate, LinkedNode));
			FValidationResult NewLinkRoomCount = ValuationFunction(*NewLinkRoom, OutPortal, LastValidationResult);

			if (NewLinkRoomCount.GetSize() != 0) NewLinkRoom->CreateJointForBlankSlots();

			NewNods.push_back(std::make_pair(NewLinkRoomCount, NewLinkRoom));
			
		}
		
		
	}

}

void USearchGraph::GetRoomChain(std::vector<std::shared_ptr<FSearchGraphNode>>& RoomChain)
{
	auto it = Graph.find(FValidationResult::Zero);
	std::shared_ptr<FSearchGraphNode> CurrentRoom = it->second;

	while (CurrentRoom)
	{
		RoomChain.push_back(CurrentRoom);
		CurrentRoom = CurrentRoom->LastNode.lock();
	}
}



bool USearchGraph::IsSelfIntersectionPresent(std::shared_ptr<FSearchGraphNode> LastRoom)
{
	std::set<FVector, FVectorLess> SearchGrapOwnCoordinats;

	std::shared_ptr<FSearchGraphNode> CurrentRoom = LastRoom;

	while (CurrentRoom)
	{
		std::vector<FVector> CellCoordinats;

		CurrentRoom->GetRoomCellsCoordinats(CellCoordinats);

		for (auto CurrentCoordinate : CellCoordinats)
		{
			if (SearchGrapOwnCoordinats.find(CurrentCoordinate) != SearchGrapOwnCoordinats.end())
			{
				return true;
			}
			else
			{
				SearchGrapOwnCoordinats.insert(CurrentCoordinate);
			}
		}
		CurrentRoom = CurrentRoom->LastNode.lock();
	}

	return false;
}

FValidationResult USearchGraph::ValuationFunction(const FSearchGraphNode& CurrentNode, const FJointPart& SecondPart, FValidationResult LastValidationResult)
{
	FVector OutPutJointCoordinate = CurrentNode.GetStartCoordinate() + CurrentNode.GetOutPutJointSlot()->JointSlot->RelativeCoordinate;
	
	float HorizontalDist = FVector::Dist2D(OutPutJointCoordinate, SecondPart.GetCoordinate());
	float VerticalDist = std::abs(OutPutJointCoordinate.Z - SecondPart.GetCoordinate().Z);

	FValidationResult Result(HorizontalDist, VerticalDist);

	if (LastValidationResult.VerticalDist < Result.VerticalDist && LastValidationResult.GetSize() != 0)
	{
		Result.VerticalDist += 10;
	}

	
	

	switch (CurrentNode.GetOutPutJointSlot()->JointSlot->Direction)
	{
	case EDirection::XP:
	{
		float DistX = SecondPart.GetCoordinate().X - OutPutJointCoordinate.X;
		if (DistX > 0) Result.HorizontalDist += 5;
		break;
	}

	case EDirection::XM:
	{
		float DistX = SecondPart.GetCoordinate().X - OutPutJointCoordinate.X;
		if (DistX < 0) Result.HorizontalDist += 5;
		break;
	}

	case EDirection::YM:
	{
		float DistY = SecondPart.GetCoordinate().Y - OutPutJointCoordinate.Y;
		if (DistY < 0) Result.HorizontalDist += 5;
		break;
	}

	case EDirection::YP:
	{
		float DistY = SecondPart.GetCoordinate().Y - OutPutJointCoordinate.Y;
		if (DistY > 0) Result.HorizontalDist += 5;
		break;
	}

	default : throw;
	}
	
	

	if (Result.GetSize() == 0)
	{
		if (!SecondPart.IsPartFitForSlot(*CurrentNode.GetOutPutJointSlot()))
		{
			Result.VerticalDist = 1;
		}
	}
	return Result;
}