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
	AddMesh(WallBuffer.GetBuffer());
}

void ALevelGenProceduralMeshActor::AddMesh(const std::map<const FLevelGeneratorMaterialSettings*, std::shared_ptr<FProceduralFigureBase>>& Meshs)
{

	for (auto MeshIT = Meshs.begin(); MeshIT != Meshs.end(); MeshIT++)
	{
		MeshIT->second->SetCollision(Collision);

		ProceduralMesh->CreateMeshSection(*MeshIT->second);


		if (MeshIT->second->GetMaterial())
		{
			ProceduralMesh->SetMaterial(CurrentElement, const_cast<UMaterialInterface*>(MeshIT->second->GetMaterial()));
		}

		CurrentElement++;
	}
	for (int i = 0; i < Meshs.size(); i++)
	{
		
	}


	ProceduralMesh->UpdateMeshComponent();
}


void ALevelGenProceduralMeshActor::SetMaterial(int32 ElementIndex, UMaterialInterface* Material)
{
	ProceduralMesh->SetMaterial(ElementIndex, Material);
}