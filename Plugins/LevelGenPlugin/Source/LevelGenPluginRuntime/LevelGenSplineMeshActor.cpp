// Copyright 2018 Pavlov Dmitriy
#include"LevelGenSplineMeshActor.h"
#include "Components/SplineMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

ALevelGenSplineMeshActor::ALevelGenSplineMeshActor()
{
	Root = CreateDefaultSubobject<USceneComponent>(*FString("Root"));
	RootComponent = Root;
	
}

void ALevelGenSplineMeshActor::AddPoints(const std::vector<FVector>& Coordinate)
{
	if (Coordinate.size() < 2) return;

	FVector StartPosition = Coordinate[0];
	FVector StartTangens = Coordinate[1] - StartPosition;

	for (int i = 1; i < Coordinate.size(); i++)
	{
		SplineMeshComponents.Add(NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass()));
		SplineMeshComponents.Last()->CreationMethod = EComponentCreationMethod::Native;
		SplineMeshComponents.Last()->SetMobility(EComponentMobility::Movable);
		SplineMeshComponents.Last()->bCastDynamicShadow = true;

		FVector EndPosition = Coordinate[i];

		FVector EndTangens = StartTangens;
		SplineMeshComponents.Last()->SetStartAndEnd(StartPosition, StartTangens, EndPosition, EndTangens);
		SplineMeshComponents.Last()->SetStartScale(Scale);
		SplineMeshComponents.Last()->SetEndScale(Scale);
		SplineMeshComponents.Last()->SetupAttachment(Root);
		if (Mesh)
		{
			SplineMeshComponents.Last()->SetStaticMesh(Mesh);
		}

		else throw ("Spline actor without mesh");

		UMaterialInstanceDynamic* DynamicMat = SplineMeshComponents.Last()->CreateDynamicMaterialInstance(0);
		DynamicMat->SetScalarParameterValue(FName("start"), CurrentLenghtSpline);

		DynamicMat->SetScalarParameterValue(FName("end"), CurrentLenghtSpline + (SplineMeshComponents.Last()->GetEndPosition() - SplineMeshComponents.Last()->GetStartPosition()).Size());

		CurrentLenghtSpline = CurrentLenghtSpline + (SplineMeshComponents.Last()->GetEndPosition() - SplineMeshComponents.Last()->GetStartPosition()).Size();

		for (auto CMeshComponent : SplineMeshComponents)
		{
			CMeshComponent->bCastStaticShadow = false;
		}

		if (i != Coordinate.size() - 1)
		{
			StartTangens = Coordinate[i + 1] - StartPosition;
		}
		
	}
}