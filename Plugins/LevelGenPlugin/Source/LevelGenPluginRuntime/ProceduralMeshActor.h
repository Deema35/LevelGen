// Copyright 2018 Pavlov Dmitriy
#pragma once

#include "CoreMinimal.h"
#include <memory>
#include "GameFramework/Actor.h"
#include "LevelGenProceduralMeshComponent.h"
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

	void AddMesh(const std::vector<std::shared_ptr<FProceduralFigureBase>>& Meshs);

	void SetMaterial(int32 ElementIndex, UMaterialInterface* Material);

	void SetCollision(bool _Collision) { Collision = _Collision; }

private:
	UPROPERTY(VisibleAnywhere)
		ULevelGenProceduralMeshComponent* ProceduralMesh;

	int CurrentElement = 0;

	bool Collision = false;
};