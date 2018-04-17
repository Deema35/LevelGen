// Copyright 2018 Pavlov Dmitriy
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "ProceduralMeshActor.generated.h"

class FProceduralFigureBase;
class FProceduralFigurBuffer;

UCLASS()
class LEVELGENPLUGINRUNTIME_API ALevelGenProceduralMeshActor : public AActor
{
	GENERATED_BODY()

public:
	ALevelGenProceduralMeshActor();
	
	void AddMesh(const FProceduralFigurBuffer& WallBuffer);

	void AddMesh(std::shared_ptr<FProceduralFigureBase> Mesh);

	void SetMaterial(int32 ElementIndex, UMaterialInterface* Material);

	void SetCollision(bool _Collision) { Collision = _Collision; }

private:
	UPROPERTY(VisibleAnywhere)
		UProceduralMeshComponent * ProceduralMesh;

	int CurrentElement = 0;

	bool Collision = false;
};