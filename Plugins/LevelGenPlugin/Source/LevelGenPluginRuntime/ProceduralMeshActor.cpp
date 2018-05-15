// Copyright 2018 Pavlov Dmitriy
#include "ProceduralMeshActor.h"
#include "ProceduralFigure.h"
#include "LevelGeneratorSettings.h"



ALevelGenProceduralMeshActor::ALevelGenProceduralMeshActor()
{
	ProceduralMesh = CreateDefaultSubobject<ULevelGenProceduralMeshComponent>(TEXT("GeneratedMesh"));
	RootComponent = ProceduralMesh;
	ProceduralMesh->bCastStaticShadow = false;
}

void ALevelGenProceduralMeshActor::AddMesh(const FProceduralFigurBuffer& WallBuffer)
{
	const std::map<const FLevelGeneratorMaterialSettings*, std::shared_ptr<FProceduralFigureBase>>& Meshes = WallBuffer.GetBuffer();

	std::vector<std::shared_ptr<FProceduralFigureBase>> MeshesBuffer;

	for (auto CurrentMesh = Meshes.begin(); CurrentMesh != Meshes.end(); CurrentMesh++)
	{
		MeshesBuffer.push_back(CurrentMesh->second);
		
	}

	AddMesh(MeshesBuffer);
}

void ALevelGenProceduralMeshActor::AddMesh(const std::vector<std::shared_ptr<FProceduralFigureBase>>& Meshs)
{
	for (int i = 0; i < Meshs.size(); i++)
	{
		Meshs[i]->SetCollision(Collision);
		ProceduralMesh->CreateMeshSection(*Meshs[i]);


		if (Meshs[i]->GetMaterial())
		{
			ProceduralMesh->SetMaterial(CurrentElement, const_cast<UMaterialInterface*>(Meshs[i]->GetMaterial()));
		}

		CurrentElement++;
	}


	ProceduralMesh->UpdateMeshComponent();
}


void ALevelGenProceduralMeshActor::SetMaterial(int32 ElementIndex, UMaterialInterface* Material)
{
	ProceduralMesh->SetMaterial(ElementIndex, Material);
}