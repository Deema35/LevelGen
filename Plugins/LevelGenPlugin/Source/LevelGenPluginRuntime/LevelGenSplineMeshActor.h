// Copyright 2018 Pavlov Dmitriy
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <vector>
#include "LevelGenSplineMeshActor.generated.h"

class USplineMeshComponent;
class UStaticMesh;

UCLASS()
class LEVELGENPLUGINRUNTIME_API ALevelGenSplineMeshActor : public AActor
{
	GENERATED_BODY()

public:

	ALevelGenSplineMeshActor();

	UPROPERTY(VisibleAnywhere)
	TArray<USplineMeshComponent*> SplineMeshComponents;

	UPROPERTY(VisibleAnywhere)
	UStaticMesh* Mesh;

	USceneComponent* Root;

	void AddPoints(const std::vector<FVector>& Coordinate);

	void SetStaticMesh(UStaticMesh* _Mesh) { Mesh = _Mesh; }

	void SetScale(FVector2D _Scale) { Scale = _Scale; }
private:
	int CurrentLenghtSpline = 0;

	FVector2D Scale = FVector2D(1,1);

};